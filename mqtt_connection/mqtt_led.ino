#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define LED 16  // GPIO 5 (D1) for LED
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
  pinMode(LED, OUTPUT);
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
  if (message == "on" && !ledState) {
    digitalWrite(LED, HIGH);  // Turn on the LED
    ledState = true;
    Serial.println("LED is turned on");
  }
  if (message == "off" && ledState) {
    digitalWrite(LED, LOW);  // Turn off the LED
    ledState = false;
    Serial.println("LED is turned off");
  }
  Serial.println();
  Serial.println("-----------------------");
}

void loop() {
  if (!mqtt_client.connected()) {
    connectToMQTTBroker();
  }
  mqtt_client.loop();
}
