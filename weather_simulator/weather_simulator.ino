// #include <ESP8266WiFi.h>

// const char* ssid = "VNUK4-2";         // Replace with your Wi-Fi SSID
// const char* password = "Z@q12wsx"; // Replace with your Wi-Fi password

// void setup() {
//   Serial.begin(9600);    // Start the Serial communication at 115200 baud rate
//   delay(10);

//   // Connect to Wi-Fi network
//   Serial.println();
//   Serial.print("Connecting to ");
//   Serial.println(ssid);

//   WiFi.begin(ssid, password);

//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }

//   Serial.println("");
//   Serial.println("WiFi connected.");
  
//   // Print the IP address
//   Serial.print("IP address: ");
//   Serial.println(WiFi.localIP());
// }

// void loop() {
//   // You can put your main code here, to run repeatedly:
// }

#include <ESP8266WiFi.h>

const char* ssid = "VNUK2-3";
const char* password = "Z@q12wsx";
//172.16.4.101
int LED1 = 0; // (D3)
int LED2 = 2; // (D4)
WiFiServer server(80);

void setup(){
  Serial.begin(9600);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  Serial.print("Connecting to the Newtork");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print("Connecting~");
  }
  Serial.println("WiFi connected"); 
  server.begin();
  Serial.println("Server started");
  Serial.print("IP Address of network: "); // will IP address on Serial Monitor
  Serial.println(WiFi.localIP());
  Serial.print("Copy and paste the following URL: http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
}

void loop(){
  WiFiClient client = server.available();
  if (!client){
    return;}
    
  Serial.println("Waiting for new client");
  while(!client.available())
  {
    delay(1);
  }
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
  
  int value = HIGH;
  if(request.indexOf("/LED=ON1") != -1){
    
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, LOW);
    value = HIGH;
  }

  if(request.indexOf("/LED=ON2") != -1){
    
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, HIGH);
    value = HIGH;
  }

  if(request.indexOf("/LED=OFF") != -1){

    digitalWrite(LED1, LOW); // Turn LED OFF
    digitalWrite(LED2, LOW);
    value = LOW;
  } 
  
//*------------------HTML Page Code---------------------*//

  client.println("HTTP/1.1 200 OK"); //
  client.println("Content-Type: text/html");
  client.println("");
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.print(" CONTROL LED: ");
  if(value == HIGH){
    client.print("ON");
  }
  else
  {
    client.print("OFF");
  }
  client.println("<br><br>");
  client.println("<a href=\"/LED=ON1\"\"><button>ON1</button></a>");
  client.println("<a href=\"/LED=ON2\"\"><button>ON2</button></a>");
  client.println("<a href=\"/LED=OFF\"\"><button>OFF</button></a><br />");
  client.println("</html>");
  
  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");
}
