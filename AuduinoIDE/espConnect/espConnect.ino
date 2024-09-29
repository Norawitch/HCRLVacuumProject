#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// Replace with your WiFi credentials
const char* ssid = "OPPO A54";
const char* password = "waitforwifi";

const char* mqtt_server = "e87ade77417e4192b935704fa90aa325.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;

// MQTT credentials (username and password from HiveMQ Cloud)
const char* mqtt_user = "photo_mqtt1";
const char* mqtt_password = "PHOTO_mqtt1";

WiFiClientSecure espClient;
PubSubClient client(espClient);

const int ledPin = 2;  // ESP32 onboard LED
// Motor
int motor_speed = 0;
const int in1Pin = 18;  // first motor
const int in2Pin = 19;
const int in3Pin = 22;  // second motor
const int in4Pin = 23;
const int enablePin1 = 21; // PWM first motor
const int enablePin2 = 5;  // PWM second motor
unsigned long motor_start_time = 0;
unsigned long motor_run_time = 50;
bool is_motor_run = false;
String motor_direction = "none";

//Select mode
String mode = "manual";


void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  // JSON
  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, message);
  // Check if the message was valid JSON
  if (!error) {
    // Handle as JSON object
    if (doc.containsKey("speed")) {
      motor_speed = doc["speed"];
      Serial.print("Motor speed set to: ");
      Serial.println(motor_speed);
      analogWrite(enablePin1, motor_speed);  // Apply the speed to motor 1
      analogWrite(enablePin2, motor_speed);  // Apply the speed to motor 2
    }
    if (doc.containsKey("mode")){
      mode = doc["mode"].as<String>();
      if (mode == "manual") {
        Serial.println("Mode switched to MANUAL");
      }
      else if (mode == "auto") {
        Serial.println("Mode switched to AUTO");
      }
    }

    if (doc.containsKey("direction")) {
      motor_direction = doc["direction"].as<String>();
    }
  
    // Manual
    if (mode == "manual") {
    // Control based on message
    // if (message == "on") {
    //   digitalWrite(ledPin, HIGH);
    // } else if (message == "off") {
    //   digitalWrite(ledPin, LOW);
    // }
      if (motor_direction == "a"){
        Serial.println("Turning left");
        motor_start_time = millis();
        is_motor_run = true;
        turnLeft();
      }
      else if (motor_direction == "d"){
        Serial.println("Turning right");
        motor_start_time = millis();
        is_motor_run = true;
        turnRight();
      }
      else if (motor_direction == "w") {
        Serial.println("Moving forward");
        motor_start_time = millis();
        is_motor_run = true;
        forward();
      }
      else if (motor_direction == "s") {
        Serial.println("Moving backward");
        motor_start_time = millis();
        is_motor_run = true;
        backward();
      }
    }
    // Auto
    else if (mode == "auto") {
      Serial.println("Auto mode active");
      if (motor_direction == "a"){
        Serial.println("Turning left");
        motor_start_time = millis();
        is_motor_run = true;
        turnLeft();
      }
      else if (motor_direction == "d"){
        Serial.println("Turning right");
        motor_start_time = millis();
        is_motor_run = true;
        turnRight();
      }
      else if (motor_direction == "w") {
        Serial.println("Moving forward");
        motor_start_time = millis();
        is_motor_run = true;
        forward();
      }
      else if (motor_direction == "s") {
        Serial.println("Moving backward");
        motor_start_time = millis();
        is_motor_run = true;
        backward();
      }
    }
  }
}

void reconnect() {
  static long lastReconnectAttempt = 0; // Track last reconnect attempt time
  if (client.connected()) {
      return;  // Exit if already connected
  }

  long now = millis();
  if (now - lastReconnectAttempt > 5000) {  // Attempt to reconnect every 5 seconds
    lastReconnectAttempt = now;
    Serial.print("Attempting MQTT connection...");
     // Attempt to connect with username and password
    if (client.connect("ESP32Client", mqtt_user, mqtt_password)) {
      Serial.println("connected");
      client.subscribe("esp32/#");  // Subscribe to the topic
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
    }
  }
}

void turnLeft() {
  digitalWrite(in1Pin, HIGH);
  digitalWrite(in2Pin, LOW);
  digitalWrite(in3Pin, LOW);
  digitalWrite(in4Pin, HIGH);
}
void turnRight() {
  digitalWrite(in1Pin, LOW);
  digitalWrite(in2Pin, HIGH);
  digitalWrite(in3Pin, HIGH);
  digitalWrite(in4Pin, LOW);

}
void forward() {
  digitalWrite(in1Pin, HIGH);
  digitalWrite(in2Pin, LOW);
  digitalWrite(in3Pin, HIGH);
  digitalWrite(in4Pin, LOW);
}
void backward() {
  digitalWrite(in1Pin, LOW);
  digitalWrite(in2Pin, HIGH);
  digitalWrite(in3Pin, LOW);
  digitalWrite(in4Pin, HIGH);
}
void stopmotor() {
  analogWrite(enablePin1, 0);
  analogWrite(enablePin2, 0);
  digitalWrite(in1Pin, LOW);
  digitalWrite(in2Pin, LOW);
  digitalWrite(in3Pin, LOW);
  digitalWrite(in4Pin, LOW);
}

// Initial setup
void initialSetup() {
  stopmotor();
}


void setup() {
  pinMode(ledPin, OUTPUT);
  // Motor
  pinMode(in1Pin, OUTPUT);
  pinMode(in2Pin, OUTPUT);
  pinMode(in3Pin, OUTPUT);
  pinMode(in4Pin, OUTPUT);
  pinMode(enablePin1, OUTPUT);
  pinMode(enablePin2, OUTPUT);

  initialSetup();

  Serial.begin(115200);
  setup_wifi();
  
  espClient.setInsecure();

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if (millis() - motor_start_time > motor_run_time) {
    stopmotor();
    is_motor_run = false;
  }
}
