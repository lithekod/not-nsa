#include <SPI.h>
#include <MFRC522.h>

const int RST_PIN = 16;
const int SS_PIN = 2;

MFRC522 rfid(SS_PIN, RST_PIN);

struct User {
    uint8_t[5] hash;
    uint8_t[8] liu_id;
}

void setup() {
    Serial.begin(115200);
    SPI.begin();
    rfid.PCD_Init();
    rfid.PCD_DumpVersionToSerial();
    Serial.println("Done");
}

void loop() {
    int user_count = 0;
    User users[30];

    while(true) {
        // Reset the loop if no new card present on the sensor/reader. This saves
        // the entire process when idle.
        if ( ! rfid.PICC_IsNewCardPresent()) {
            break;
        }

        // Select one of the cards
        if ( ! rfid.PICC_ReadCardSerial()) {
            break;
        }

        // Dump debug info about the card; PICC_HaltA() is automatically called
        // rfid.PICC_DumpToSerial(&(rfid.uid));

        dump_byte_array(rfid.uid.uidByte, rfid.uid.size);
        Serial.println("");
    }
}

void dump_byte_array(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}
