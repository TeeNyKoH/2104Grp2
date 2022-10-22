#include "M5StickCPlus.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>

#define MSG_BUFFER_SIZE 50

//Change this later!
const char* ssid        = "test";
const char* password    = "test";
const char* mqtt_server = "192.168.1.1";
const int port = 1883;

const char* topic       = "car";

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;
char msg[MSG_BUFFER_SIZE];
int value = 0;

void wifiSetup(){
  delay(10);

  M5.Lcd.print("Connecting to MQTT server");
  WiFi.mode(WIFI_STA);          //Configure as WiFi station
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    M5.Lcd.print(".");;
  }
  
  M5.Lcd.println("Connected.");
}

// Loop until we're reconnected
void reconnect(){
  while (!client.connected()) {
    M5.Lcd.println("Reconnecting...");
    // Create a random client ID
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      M5.Lcd.println("Reconnected.");
      // Once connected, publish (sending) an announcement...
      client.publish(topic, "Reconnected.");
      // ... and resubscribe (receiving)
      client.subscribe(topic);
    } else {
      M5.Lcd.println("failed, rc=");
      M5.Lcd.println(client.state());
      M5.Lcd.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

//I2C event handler
void receiveEvent(int count){
  uint8_t *tmp = (uint8_t *) malloc(count);
  memset(tmp, '\0', count);
  int i = 0;
  while(1 < Wire.available()) // loop through all but the last
  {
    char c = Wire.read(); // receive byte as a character
    tmp[i] = (uint8_t)c;
    ++i;
  }
  free(tmp);
  client.publish(topic, tmp, count, false);
}

void setup() {
  //Initialize the M5
  M5.begin();
  wifiSetup();
  M5.Lcd.setRotation(3);
  M5.Lcd.setTextColor(YELLOW);

  //Initialize MQTT
  M5.Lcd.println("ICT2104 Car");
  client.setServer(mqtt_server, port);

  //Initialize the I2C as slave with address 0x55
  Wire.begin(0x55, 32, 33, 100000);
  Wire.onReceive(receiveEvent);
  M5.Lcd.println("Setup complete!");
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
