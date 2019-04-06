#include <SPI.h>
#include <MFRC522.h>

#include "interface.hpp"

const int RST_PIN = 16;
const int SS_PIN = 2;
const int UID_LENGTH = 4;

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
    rfid.PCD_Init();
    rfid.PCD_DumpVersionToSerial();
    Serial.println("Done");

    memcpy(users[0].uid, user0_id, UID_LENGTH);
    strcpy(users[0].liu_id, "frask812");
    memcpy(users[1].uid, user1_id, UID_LENGTH);
    strcpy(users[1].liu_id, "yoloo123");
    // users[1].uid = [0xB4, 0x25, 0xD9, 0x1E];
    // users[1].liu_id = "yoloo420";
    user_count = 2;

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
