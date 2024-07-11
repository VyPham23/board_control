#include <ESP8266WiFi.h>
#include <PubSubClient.h>

int LED1 = 0; // (D3)
int LED2 = 2; // (D4)
int LED3 = 14; // (D5)
bool ledState = false;

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
  pinMode(LED2, OUTPUT);
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  Serial.begin(115200);
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
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];  // Convert *byte to string
  }
    
  // Control the LED based on the message received
  if (message == "ON1") {
    digitalWrite(LED1, HIGH);  // Turn on 1 LED
    digitalWrite(LED2, LOW); 
    digitalWrite(LED3, LOW);
    ledState = true;
    Serial.println("1 LED turned on");
  }

  if (message == "ON2") {
    digitalWrite(LED1, HIGH);  // Turn on 2 LED
    digitalWrite(LED2, HIGH); 
    digitalWrite(LED3, LOW);
    ledState = true;
    Serial.println("2 LEDs turned on");
  }

  
  if (message == "ON3") {
    digitalWrite(LED1, HIGH);  // Turn on 2 LED
    digitalWrite(LED2, HIGH); 
    digitalWrite(LED3, HIGH);
    ledState = true;
    Serial.println("3 LEDs turned on");
  }
  
  if (message == "OFF" && ledState) {
    digitalWrite(LED, LOW);  // Turn off the LED
    ledState = false;
    Serial.println("LEDs turned off");
  }
  Serial.println();
  Serial.println("-----------------------");
}

void mqttPublish(){
  // Read from DHT sensor
  float t = dht.readTemperature();
  float h = dht.readHumidity();

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
  client.publish(mqtt_topic, tempBuffer);
  
  // Print and publish the humidity data
  Serial.print("Publishing humidity data: ");
  Serial.println(humBuffer);
  client.publish(mqtt_topic, humBuffer);
  
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
    readAndPublishSensorData();
    lastPublishTime = now;
  }
}
