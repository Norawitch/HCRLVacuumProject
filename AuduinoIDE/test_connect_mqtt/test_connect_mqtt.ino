#include <M5Stack.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

// Wi-Fi and MQTT settings
const char* ssid = "OPPO A54";
const char* password = "waitforwifi";
const char* mqtt_server = "e87ade77417e4192b935704fa90aa325.s1.eu.hivemq.cloud";
const char* mqtt_user = "photo_mqtt1";
const char* mqtt_password = "PHOTO_mqtt1";

WiFiClientSecure espClient; // Use WiFiClientSecure for secure connection
PubSubClient client(espClient);

void setup() {
  M5.begin();
  Serial.begin(115200);
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Secure connection settings
  espClient.setInsecure(); // Use this for testing without cert validation, should be removed for production

  // Delay to ensure Wi-Fi connection stabilizes
  delay(2000);

  // Generate unique client ID
  String clientId = "M5GOClientID-";
  clientId += String(random(0xffff), HEX);  // Ensure unique client ID

  // Connect to MQTT
  client.setServer(mqtt_server, 8883); // Use port 8883 for secure connection
  
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
      Serial.println("MQTT connected");
    } else {
      Serial.print("Failed, rc=");
      Serial.println(client.state()); // Print error code
      Serial.print("Reason: ");
      switch (client.state()) {
        case -1: Serial.println("Connection refused, unacceptable protocol version"); break;
        case -2: Serial.println("Connection refused, identifier rejected"); break;
        case -3: Serial.println("Connection refused, server unavailable"); break;
        case -4: Serial.println("Connection refused, bad username or password"); break;
        case -5: Serial.println("Connection refused, not authorized"); break;
        default: Serial.println("Unknown error"); break;
      }
      delay(5000);
    }
  }
}

void loop() {
  client.loop();
}
