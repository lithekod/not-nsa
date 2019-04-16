#include <SPI.h>
#include <MFRC522.h>
#include <EEPROM.h>

#include "interface.hpp"
#include "attendee.hpp"
#include "constants.hpp"
#include "registration.hpp"

int addr = 1;

MFRC522 rfid(SS_PIN, RST_PIN);

struct User {
    uint8_t uid[UID_LENGTH];
    // Length of liu id + null terminator
    char liu_id[LIU_ID_LEN + 1];
};


int user_count = 0;
int event_count = 0;
User users[300];
EventAttendee event_attendees[100];

uint8_t last_uid[UID_LENGTH];

char username;

Interface interface;


void setup() {
    Serial.begin(115200);
    SPI.begin();
    EEPROM.begin(512);
    rfid.PCD_Init();
    rfid.PCD_DumpVersionToSerial();
    // clear_eeprom(); // DANGER! DANGER! Uncomment only if you want to clear the eeprom
    Serial.println("Done");

    // pinMode(15, OUTPUT);
    // pinMode(YELLOW_PIN, OUTPUT);
    // pinMode(RED_PIN, OUTPUT);

    // digitalWrite(GREEN_PIN, LOW);
    // digitalWrite(YELLOW_PIN, LOW);
    // digitalWrite(RED_PIN, LOW);

    addr = end_addr();
    init_users();
}

enum class CardState {
    RECOGNISED,
    UNRECOGNISED,
    SAME_CARD,
    NO_CARD
};


void loop() {
    CardState state = CardState::NO_CARD;
    char* liu_id;
    uint8_t* uid;

    // Reset the loop if no new card present on the sensor/reader. This saves
    // the entire process when idle.
    if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
        state = CardState::UNRECOGNISED;
        uid = rfid.uid.uidByte;
        // If we got a new card ID
        if(memcmp(rfid.uid.uidByte, last_uid, UID_LENGTH) != 0) {
            memcpy(last_uid, rfid.uid.uidByte, UID_LENGTH);
            for(size_t i = 0; i < user_count; i++) {
                if(memcmp(rfid.uid.uidByte, users[i].uid, UID_LENGTH) == 0) {
                    state = CardState::RECOGNISED;
                    liu_id = users[i].liu_id;
                }
            }
        }
        else {
            state = CardState::SAME_CARD;
        }
    }

    if(state == CardState::RECOGNISED) {
        RegistrationState registration_state;
        const char* note;
        // TODO: Replace with c++17 structured binding
        std::tie(registration_state, note) = check_registration(liu_id, event_attendees, event_count);

        Serial.print("LIU id: ");
        Serial.println(liu_id);
        if(registration_state == RegistrationState::SUCCESS) {
            Serial.println("Registered!");
            Serial.println(note);
        }
        else {
            Serial.println("Not registered for event");
        }
    }
    if(state == CardState::UNRECOGNISED) {
        if(interface.state == InterfaceState::WAIT_FOR_CARD) {
            memcpy(users[user_count].uid, rfid.uid.uidByte, UID_LENGTH);
            strcpy(users[user_count].liu_id, interface.liu_id_to_register);
            interface.state = InterfaceState::WAITING;

            // Write to eeprom
            save_uid(rfid.uid.uidByte, UID_LENGTH);
            save_liu_id(interface.liu_id_to_register);
            EEPROM.write(END_ADDR_ADDR, addr);
            EEPROM.commit();

            Serial.println("User registered");
            user_count += 1;
            reset_last_uid();
        }
        else {
            Serial.println("Unrecognised LIU id");
        }
    }

    while(Serial.available() > 0) {
        char output_buf[128];

        interface.handle_input(Serial.read(), output_buf);
        Serial.write(output_buf);
        Serial.flush();

        auto new_attendee = interface.get_new_attendee();
        if(new_attendee != nullptr) {
            strcpy(event_attendees[event_count].liu_id, new_attendee->liu_id);
            event_attendees[event_count].note = new_attendee->note;
            event_count += 1;
        }
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
    EEPROM.commit();
}

void init_users() {
    // Initializes the users array by copying the users
    // stored in the eeprom to the users array.
    int ptr = 1;
    uint8_t temp_uid[UID_LENGTH];
    // Length of liu ID + null terminator
    char temp_liu_id[LIU_ID_LEN+1];
    
    while (ptr < addr) {
        // Get UID
        for (int i = 0; i < UID_LENGTH; i++) {
            temp_uid[i] = EEPROM.read(i+ptr);
        }

        Serial.print(" ");

        // Get Liu ID
        for (int i = 0; i < LIU_ID_LEN; i++) {
            temp_liu_id[i] = EEPROM.read(i+UID_LENGTH+ptr);
        }
        temp_liu_id[LIU_ID_LEN] = '\0';


        memcpy(users[user_count].uid, temp_uid, UID_LENGTH);
        strcpy(users[user_count].liu_id, temp_liu_id);
        user_count+=1;
        ptr+=12;
    }
}


