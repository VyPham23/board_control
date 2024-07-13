#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <DFRobot_SHT20.h>
#include <string>


#define LED1 D1
#define LED23 D2
#define FOG1 D3
#define FOG23 D4
#define FUME D5

#define RECV_PIN D6


// WiFi settings
const char *ssid = "Thanh Hai t2";
const char *password = "anninhtrattu";

// MQTT Broker settings
const char *mqtt_broker = "broker.emqx.io";
const char *mqtt_topic = "emqx/esp8266/led";
const char *mqtt_username = "nguyenngannn0308";
const char *mqtt_password = "A12345678n";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient mqtt_client(espClient);

void connectToWiFi();
void connectToMQTTBroker();
void mqttCallback(char *topic, byte *payload, unsigned int length);

void setup() {
  pinMode(LED1, OUTPUT);
  pinMode(LED23, OUTPUT);
  pinMode(FOG1, OUTPUT);
  pinMode(FOG23, OUTPUT);
  pinMode(FUME, OUTPUT);
    
  // Set default state to off
  digitalWrite(LED1, LOW);
  digitalWrite(LED23, LOW);
  digitalWrite(FOG1, LOW);
  digitalWrite(FOG23, LOW);
  digitalWrite(FUME, LOW);
  Serial.begin(9600);
  
  connectToWiFi();
  mqtt_client.setServer(mqtt_broker, mqtt_port);
  mqtt_client.setCallback(mqttCallback);
  connectToMQTTBroker();
}

void connectToWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to the WiFi network");
}

void connectToMQTTBroker() {
  while (!mqtt_client.connected()) {
    String client_id = "esp8266-client-" + String(WiFi.macAddress());
    // String client_id = 'mqttx_fa6f623a';
    Serial.printf("Connecting to MQTT Broker as %s.....\n", client_id.c_str());
    delay(10000);
    if (mqtt_client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("Connected to MQTT broker");
      mqtt_client.subscribe(mqtt_topic);
      mqtt_client.publish(mqtt_topic, "Hi EMQX I'm ESP8266 ^^");
    } else {
      Serial.print("Failed to connect to MQTT broker, rc=");
      Serial.print(mqtt_client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void mqttCallback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message received on topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  char msg[length +1];
  strncpy(msg, (char*)payload, length);
  msg[length] = '\0';

  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, msg);

  if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.f_str());
        return;
    }
  String type = doc["type"].as<String>();
  String message = doc["msg"].as<String>();

  if (type == "fog") {
        processFog(message);
    } else if (type == "temperature") {
        processTemperature(message);
    } else if (type == "fume") {
        processFume(message);
    }
}

void processFog(const String message){
  
  // Control the LED based on the message received
  if (message == "ON1") {
    digitalWrite(FOG1, HIGH);
    digitalWrite(FOG23, LOW);
  } else if (message == "ON2") {
    digitalWrite(FOG1, LOW);
    digitalWrite(FOG23, HIGH);
  } else if (message == "ON3") {
    digitalWrite(FOG1, HIGH);
    digitalWrite(FOG23, HIGH);
  } else {
    digitalWrite(FOG1, LOW);
    digitalWrite(FOG23, LOW);
  }
}
  
void processTemperature(const String message){
  
  // Control the LED based on the message received
  if (message == "ON1") {
    digitalWrite(LED1, HIGH);
    digitalWrite(LED23, LOW);
  } else if (message == "ON2") {
    digitalWrite(LED1, LOW);
    digitalWrite(LED23, HIGH);
  } else if (message == "ON3") {
    digitalWrite(LED1, HIGH);
    digitalWrite(LED23, HIGH);
  } else {
    digitalWrite(LED1, LOW);
    digitalWrite(LED23, LOW);
  }
}

void processFume(const String message) {
  if (message == "ON1") {
    digitalWrite(FUME, HIGH);
    delay(5000);
    digitalWrite(FUME, LOW);
  } else if (message == "ON2") {
      digitalWrite(FUME, HIGH);
      delay(10000);
      digitalWrite(FUME, LOW);
  } else if (message == "ON3") {
      digitalWrite(FUME, HIGH);
      delay(15000);
      digitalWrite(FUME, LOW);
  } else {
      digitalWrite(FUME, LOW);
  }
}

void mqttPublish(){
  // Read from sht sensor
  float t = sht20.readTemperature();
  float h = sht20.readHumidity();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Create json
  StaticJsonDocument<200> tempDoc;
  tempDoc["msg"] = t;
  tempDoc["type"] = "temperature";
  char tempBuffer[512];
  serializeJson(tempDoc, tempBuffer);

  StaticJsonDocument<200> humDoc;
  humDoc["msg"] = h;
  humDoc["type"] = "humidity";
  char humBuffer[512];
  serializeJson(humDoc, humBuffer);

  // Print and publish the temperature data
  Serial.print("Publishing temperature data: ");
  Serial.println(tempBuffer);
  mqtt_client.publish(mqtt_topic, tempBuffer);
  
  // Print and publish the humidity data
  Serial.print("Publishing humidity data: ");
  Serial.println(humBuffer);
  mqtt_client.publish(mqtt_topic, humBuffer);
  
  // Wait for a few seconds before publishing again
  delay(5000);
}

void loop() {
  if (!mqtt_client.connected()) {
    connectToMQTTBroker();
  }
  mqtt_client.loop();

  // Read and publish sensor data every 10 seconds
  static unsigned long lastPublishTime = 0;
  unsigned long now = millis();
  if (now - lastPublishTime > 10000) {
    mqttPublish();
    lastPublishTime = now;
  }
}
