#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define RECV_PIN D4;

// WiFi settings
const char *ssid = "Thanh Hai t2";
const char *password = "anninhtrattu";

// MQTT Broker settings
const char *mqtt_broker = "broker.emqx.io";
const char *mqtt_topic = "emqx/esp8266/led";
const char *mqtt_username = "nguyenngannn0308";
const char *mqtt_password = "A12345678n";
const int mqtt_port = 1883;

IRrecv irrecv(RECV_PIN);
decode_results results;
WiFiClient espClient;
PubSubClient mqtt_client(espClient);

void connectToWiFi();
void connectToMQTTBroker();

void setup() {
  Serial.begin(9600);
  irrecv.enableIRIn(); // Start the receiver
  connectToWiFi();
  mqtt_client.setServer(mqtt_broker, mqtt_port);
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

void decodeAndPublishIRData() {
  if (irrecv.decode(&results)) {
    char msg[50];
    sprintf(msg, "0x%X", results.value);
    Serial.println(msg);
    client.publish(mqtt_topic, msg);
    irrecv.resume(); // Receive the next value
  }
}

void loop() {
  if (!mqtt_client.connected()) {
    connectToMQTTBroker();
  }
  mqtt_client.loop();

  decodeAndPublishIRData();

  delay(100);
  }
}
