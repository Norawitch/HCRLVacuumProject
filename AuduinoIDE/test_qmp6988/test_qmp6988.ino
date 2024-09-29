#include <Wire.h>
#include "QMP6988.h"

QMP6988 qmp6988;

void setup() {
    Serial.begin(115200);
    Wire.begin(21, 22); // SDA = 21, SCL = 22
    Serial.println("Initializing QMP6988...");
    if (!qmp6988.begin(&Wire, 0x68)) { // Change address if needed
        Serial.println("Failed to initialize QMP6988!");
        while (1); // Stop execution if failed
    }
}

void loop() {
    float pressure = qmp6988.calcPressure();
    Serial.printf("Pressure: %.2f hPa\n", pressure);
    delay(2000); // Read every 2 seconds
}

