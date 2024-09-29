#include <Wire.h>
#include <SHT3X.h>

SHT3X sht30;

void setup() {
    Serial.begin(115200);
    Wire.begin();
    Serial.println("Initializing SHT30...");

    if (!sht30.begin(&Wire, 0x5C, 21, 22, 100000)) {
        Serial.println("Failed to initialize SHT30 sensor!");
    } else {
        Serial.println("SHT30 sensor initialized successfully.");
    }
}

void loop() {
    // Put the reading code here if initialization is successful
    while (!sht30.begin(&Wire, 0x5C, 21, 22, 100000)) {
      Serial.println("Failed to initialize SHT30 sensor!");
      if (sht30.begin(&Wire, 0x5C, 21, 22, 100000)) {
        Serial.println("SHT30 sensor initialized successfully.");
        break;
      }
    }
}


