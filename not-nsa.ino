#include <SPI.h>
#include <MFRC522.h>

#include "interface.hpp"
#include <EEPROM.h>

const int RST_PIN = 16;
const int SS_PIN = 2;
const int UID_LENGTH = 4;
const int END_ADDR_ADDR = 0;

int addr = 1;

MFRC522 rfid(SS_PIN, RST_PIN);

struct User {
    uint8_t uid[UID_LENGTH];
    char liu_id[9];
};

struct Event_attendee {
  char liu_id[9];
  String note;
  bool logged = false;
};

int user_count = 0;
int event_count = 0;
User users[30];
Event_attendee events_attendees[100];

uint8_t last_uid[UID_LENGTH];

const uint8_t user0_id[UID_LENGTH] = {0xFC, 0x86, 0xAD, 0x08};
const uint8_t user1_id[UID_LENGTH] = {0xB4, 0x25, 0xD9, 0x1E};

char username;

Interface interface;


void setup() {
    Serial.begin(115200);
    SPI.begin();
    EEPROM.begin(512);
    rfid.PCD_Init();
    rfid.PCD_DumpVersionToSerial();
    //clear_eeprom(); // DANGER! DANGER! Uncomment only if you want to clear the eeprom
    Serial.println("Done");

    addr = end_addr();
    init_users();

    strcpy(events_attendees[0].liu_id, "frask812");
    events_attendees[0].note = " hej, jag tål inte emacs";
    event_count = 1;
}


void loop() {
    // Reset the loop if no new card present on the sensor/reader. This saves
    // the entire process when idle.
    if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
        // If we got a new card ID
        if(memcmp(rfid.uid.uidByte, last_uid, UID_LENGTH) != 0) {
            memcpy(last_uid, rfid.uid.uidByte, UID_LENGTH);
            bool recognised_card = false;
            for(size_t i = 0; i < user_count; i++) {
                if(memcmp(rfid.uid.uidByte, users[i].uid, UID_LENGTH) == 0) {
                    Serial.println(users[i].liu_id);
                    check_event(users[i].liu_id);
                    recognised_card = true;
                }
            }

            if(!recognised_card) {
                if(interface.state == InterfaceState::WAIT_FOR_CARD) {
                    memcpy(users[user_count].uid, rfid.uid.uidByte, UID_LENGTH);
                    strcpy(users[user_count].liu_id, interface.liu_id_to_register);
                    interface.state = InterfaceState::WAITING;

                    // Write to eeprom
                    save_uid(rfid.uid.uidByte, UID_LENGTH);
                    save_liu_id(interface.liu_id_to_register);
                    EEPROM.write(END_ADDR_ADDR, addr);

                    Serial.println("User registered");
                    user_count += 1;
                    reset_last_uid();
                }
                else {
                    Serial.println("Unrecognised LIU ID");
                }
            }
        }
    }
    else {
    }

    while(Serial.available() > 0) {
        char output_buf[128];

        interface.handle_input(Serial.read(), output_buf);
        Serial.write(output_buf);
        Serial.flush();
    }
    if(interface.state == InterfaceState::WAIT_FOR_CARD) {
        reset_last_uid();
    }
    EEPROM.commit();
}


void reset_last_uid() {
    memset(last_uid, 0, UID_LENGTH);
}


void dump_byte_array(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}

void save_uid(byte *buffer, byte bufferSize) {
    // Saves uid to the eeprom
    for (byte i = 0; i < bufferSize; i++) {
        EEPROM.write(addr, buffer[i]);
        addr = addr + 1;
    }
}

void save_liu_id(char *buffer) {
    // Saves liu id to the eeprom
    for (int i = 0; i < strlen(buffer); i++) {
        EEPROM.write(addr, buffer[i]);
        addr = addr + 1;
    }
}

int end_addr() {
    // Gets the last address of the users in the eeprom
    int read_value = EEPROM.read(END_ADDR_ADDR);
    return read_value;
}

void clear_eeprom() {
    // DANGER! DANGER!
    // Running this function will result in loss of user data!
    EEPROM.write(END_ADDR_ADDR, 1);
}

void init_users() {
    // Initializes the users array by copying the users
    // stored in the eeprom to the users array.
    int ptr = 1;
    uint8_t temp_uid[UID_LENGTH];
    char temp_liu_id[9];
    
    while (ptr < addr) {
        // Get UID
        for (int i = 0; i < UID_LENGTH; i++) {
            temp_uid[i] = EEPROM.read(i+ptr);
        }

        // Get Liu ID
        for (int i = 0; i < 8; i++) {
            temp_liu_id[i] = EEPROM.read(i+UID_LENGTH+ptr);
        }
        temp_liu_id[8] = '\0';

        
        memcpy(users[user_count].uid, temp_uid, UID_LENGTH);
        strcpy(users[user_count].liu_id, temp_liu_id);
        user_count+=1;
        ptr+=12;
    }
}


void check_event(char* liu_id) {
      bool found = false;
      for(int i = 0; i < event_count; i++) {
          if(strcmp(liu_id, events_attendees[i].liu_id) == 0) {
                if(!events_attendees[i].logged){
                    events_attendees[i].logged = true;
                    Serial.print(liu_id);
                    Serial.println(events_attendees[i].note);
                    found = true;  
                } else {
                    Serial.println("Allready logged");
                    found = true;
                }
                
          }
      }
      if(!found){
          Serial.println("Not registered for this event!");
      }
}
