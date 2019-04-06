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

int user_count = 0;
User users[30];

uint8_t last_uid[UID_LENGTH];

const uint8_t user0_id[UID_LENGTH] = {0xFC, 0x86, 0xAD, 0x08};
const uint8_t user1_id[UID_LENGTH] = {0xB4, 0x25, 0xD9, 0x1E};

char username;

InterfaceState interface_state;


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
}


void loop() {
    // Reset the loop if no new card present on the sensor/reader. This saves
    // the entire process when idle.
    if (!rfid.PICC_IsNewCardPresent()) {
        return;
    }

    // Select one of the cards
    if (!rfid.PICC_ReadCardSerial()) {
        return;
    }

    // If we got a new card ID
    if(memcmp(rfid.uid.uidByte, last_uid, UID_LENGTH) != 0) {
        memcpy(last_uid, rfid.uid.uidByte, UID_LENGTH);
        if(interface_state == WAITING) {
            for(size_t i = 0; i < user_count; i++) {
                if(memcmp(rfid.uid.uidByte, users[i].uid, UID_LENGTH) == 0) {
                    Serial.println(users[i].liu_id);
                }
            }
        }
    }
}

void dump_byte_array(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}
