#include <M5Stack.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <M5UnitENV.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "SHT3X.h"
#include "QMP6988.h"

// Wi-Fi credentials
const char* ssid = "OPPO A54";
const char* password = "waitforwifi";

// MQTT broker
const char* mqtt_server = "e87ade77417e4192b935704fa90aa325.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;

const char* mqtt_user = "photo_mqtt1";
const char* mqtt_password = "PHOTO_mqtt1";

// Root certificate for HiveMQ (or Let's Encrypt)
const char* root_ca = \
  "-----BEGIN CERTIFICATE-----\n"
  "MIIFazCCA1OgAwIBAgISA41TtSxp5dd6ICRs+DJ7ODv2MA0GCSqGSIb3DQEBCwUA\n"
  "MEoxCzAJBgNVBAYTAlVTMRYwFAYDVQQKEw1MZXQncyBFbmNyeXB0MRMwEQYDVQQD\n"
  "EwpMRVRFUy1FTlJPT1QwHhcNMjMxMDE5MDAwMDAwWhcNMjYwMTE1MjM1OTU5WjAX\n"
  "MRUwEwYDVQQDEwxIb3ZhbGEtUk9PVDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCC\n"
  "AQoCggEBANZQsqUEYvZvqTazGtuj4NjV+PgR9l4OcukX1FYDjDpnPvDGzLEH+Im1\n"
  "t+GdyGuwfOztBRfrKsBslKpYx7oEjD0EHNR/fRqDfU5JBM3HDlNUw5Hxhhb8htVQ\n"
  "P6+/LTvX1jcsG0+xekRqtEdLyzVkZREbFgciPbCo0eBW5FmsFgCmBB2Ai8OYRT5f\n"
  "sUdAjN1UhzgoNfqGq5ALPsMxPSFb84eL9EKV9ohkzQoXhZm4Jqi0FSntZ3ksORCb\n"
  "lBgnjP98cIvz8cHnt+mPf/Nf/nZVQOCzzcGJr5Vs93oJKpKf9H5dd71yWVffBkBn\n"
  "O7cVD56V6BNFeJrMgpGzJe2EGp8TW8MCec8CAwEAAaOCAc8wggHLMA4GA1UdDwEB\n"
  "/wQEAwIBhjAdBgNVHSUEFjAUBggrBgEFBQcDAQYIKwYBBQUHAwIwDAYDVR0TAQH/\n"
  "BAIwADAdBgNVHQ4EFgQUQspBfHGfToCpGhXE+0N0+dcb5hUwHwYDVR0jBBgwFoAU\n"
  "QspBfHGfToCpGhXE+0N0+dcb5hUwggEXBgNVHREEggEOggxIb3ZhbGEtUk9PVDCC\n"
  "ECkGCSsGAQQB2kcBAQwGaG92YWxhgQweaG92YWxhLmNvbTAOBgNVHQ8BAf8EBAMC\n"
  "BaAwGgYIKwYBBQUHAQEENjA0MDoGCCsGAQUFBzABhi5odHRwOi8vcGtpLnN5bWFj\n"
  "bG91ZC5jb20vY2EvdGVzdGNhLnBlbS5kcmVhbTANBgkqhkiG9w0BAQsFAAOCAQEA\n"
  "YbTAmxTfEcHxaYsbFx/EuEyY8B1+FzMT6U8Rhx4hgDE6rZukMezCbb8E1nsvPguI\n"
  "RgAfm5lZa6kxX/dkAz3/Io9LhA98n9TPpS0VJ5AIFw5HTJXX/Ou0Tn7zhF3S2b2/\n"
  "pRsdORJ56Mgfi1Aj8mDj+jXq2xOnJ2xJoT7KTCA2nxPRpIot/ZddGbZfLKqMyXYG\n"
  "lBJZruCL1GcbAQD1DBQQTf/bDDCugdcW1xoD6qi2NRP7Xbtd5Dz79HegD/b3QnvE\n"
  "2BfUjfQnCqRwcS5kXOUvPi1FgQW7MNXaThSNd3y1JDGUmOhqqYY5PtVwFH/JIbpw\n"
  "0+cnMYuhVzQsik06pRfKXVc4eA==\n"
  "-----END CERTIFICATE-----\n";

// Initialize ENV unit (I2C)
QMP6988 qmp6988;
SHT3X sht30;

// MQTT setup
WiFiClientSecure espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  M5.Lcd.println("Connecting to WiFi");
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    M5.Lcd.print(".");
  }
  if (WiFi.status() != WL_CONNECTED) {
    M5.Lcd.println("Wi-Fi not connected");
  } else {
    M5.Lcd.print("Wi-Fi connected, IP: ");
    M5.Lcd.println(WiFi.localIP());
  }
}

void setup() {
  // Initialize M5GO and ENV unit
  M5.begin();
  Wire.begin(21, 22); // SDA = 21, SCL = 22 for I2C (Port A)
  
  M5.Lcd.println("Initializing ENV unit...");
  M5.IMU.Init();
  qmp6988.begin();
  sht30.begin();
  if (!sht30.begin(&Wire, 0x5C, 21, 22)) {
    M5.Lcd.println("Failed to initialize SHT30 sensor!");
  }

  if (!qmp6988.begin(&Wire, 0x76, 21, 22)) {
    M5.Lcd.println("Failed to initialize QMP6988 sensor!");
  }

  // Connect to WiFi
  setup_wifi();
  
  // Set up the root CA for secure connection
  espClient.setInsecure();
  
  // Set up MQTT (use port 8883 for secure MQTT)
  client.setServer(mqtt_server, mqtt_port);
}

void reconnect() {
  // Reconnect to MQTT broker
  while (!client.connected()) {
    if (client.connected()) {
      return;  // Exit if already connected
    }
    M5.Lcd.println("Attempting MQTT connection...");
    if (client.connect("M5GOClientID", mqtt_user, mqtt_password)) {
      M5.Lcd.println("MQTT connected");
      client.subscribe("esp32/m5go/env");
    } else {
      M5.Lcd.print("Failed, rc=");
      M5.Lcd.println(client.state());
      delay(5000);
    }
  }
}

static float lastTemperature = 0, lastHumidity = 0, lastPressure = 0;
float accX, accY, accZ;

void loop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop();
    
    // Read data from ENV sensor
    if (sht30.update()) {  // Ensure to call update() to get fresh readings
        float temperature = sht30.cTemp;
        float humidity = sht30.humidity;
        float pressure = qmp6988.calcPressure();

        // Only update display and publish if values have changed significantly
        if (temperature != lastTemperature || humidity != lastHumidity || pressure != lastPressure) {
            M5.Lcd.clear();
            M5.Lcd.setTextSize(2);
            M5.Lcd.setCursor(0, 0);
            M5.Lcd.printf("Temp: %.2f °C\n", temperature/-1);
            M5.Lcd.printf("Humidity: %.2f %%\n", humidity);
            M5.Lcd.printf("Pressure: %.2f hPa\n", pressure/-100);
            
            // Update last values
            lastTemperature = temperature;
            lastHumidity = humidity;
            lastPressure = pressure;

            // Create a JSON-like payload to send via MQTT
            String payload = "{";
            payload += "\"temperature\":" + String(temperature/-1, 2) + ",";
            payload += "\"humidity\":" + String(humidity) + ",";
            payload += "\"pressure\":" + String(pressure/-100, 2);
            payload += "}";
            client.publish("esp32/m5go/sensor/env", payload.c_str());
        }
    } else {
        M5.Lcd.println("Failed to update sensor data!");
    }

    // Read accelerometer data
    M5.IMU.getAccelData(&accX, &accY, &accZ); // Use the IMU to get accelerometer data
    String accelPayload = "{";
    accelPayload += "\"x_axis\":" + String(accX, 2) + ",";
    accelPayload += "\"y_axis\":" + String(accY, 2) + ",";
    accelPayload += "\"z_axis\":" + String(accZ, 2);
    accelPayload += "}";

    // Publish accelerometer data
    client.publish("esp32/m5go/sensor/accelero", accelPayload.c_str());

    // Wait for 5 seconds before the next reading
    delay(5000);
}
