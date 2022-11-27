#include "M5StickCPlus.h"
#include <WiFi.h>
#include <PubSubClient.h>
#define MQTT_MAX_LENGTH 1024

const char* ssid        = "Pixel_8129";
const char* password    = "yr5vs7kgbeb5nnc";
const char* mqtt_server = "192.168.137.1";
const int port          = 1883;

const char* recvTopic   = "send";
const char* picoTopic   = "car";

uint8_t mqttBuffer[MQTT_MAX_LENGTH];   // an array to store the received data

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length) {
  if (strncmp(topic, recvTopic, 4) == 0) {

    // The pico only has a 32 byte RX buffer for UART. Restrict all messages to 30 bytes and less.
    // Leave 1 byte for header and 1 byte for null terminator
    unsigned int callbackLength = 0;
    if (length < 31) {
      callbackLength = length + 2;
    } else {
      callbackLength = 32;
    }

    uint8_t* msgReceived = (uint8_t *) malloc(callbackLength);
    memset(msgReceived, '\0', callbackLength);
    msgReceived[0] = 0x01;
    for (int i = 0; i < (callbackLength - 2); i++){
      msgReceived[i + 1] = payload[i];
    }
    //strncat(msgReceived, payload, (int)(callbackLength - 2));
    msgReceived[callbackLength - 1] = 0x04;
    Serial.write(msgReceived, callbackLength);
    M5.Lcd.printf("Message received!\n");
	free(msgReceived);
  }
}

void wifiSetup() {
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
void reconnect() {
  while (!client.connected()) {
    M5.Lcd.println("Reconnecting...");
    // Create a random client ID
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      M5.Lcd.println("Reconnected.");
      // Once connected, publish (sending) an announcement...
      client.publish(picoTopic, "Reconnected.");
      // ... and resubscribe (receiving)
      client.subscribe(recvTopic);
    } else {
      M5.Lcd.println("failed, rc=");
      M5.Lcd.println(client.state());
      M5.Lcd.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void uartProcess() {
  static int charCount = 0;
  static char ch;

  while (Serial.available() > 0) {
    ch = Serial.read();

    if (ch != 0x04) {
      mqttBuffer[charCount] = ch;
      charCount++;
      if (charCount >= MQTT_MAX_LENGTH) {
        charCount = 0;
      }
    }
    else {
      mqttBuffer[charCount] = '\0';
      client.publish(picoTopic, mqttBuffer, charCount, false);
      charCount = 0;
    }
  }
}

void setup() {
  //Initialize the M5
  M5.begin();
  pinMode(32, INPUT);
  pinMode(33, OUTPUT);
  wifiSetup();
  M5.Lcd.setRotation(3);
  M5.Lcd.setTextColor(YELLOW);

  //Initialize MQTT
  M5.Lcd.println("ICT2104 Car");
  client.setBufferSize(1024);
  client.setServer(mqtt_server, port);
  client.setCallback(callback);
  
  //Initialize UART for analysis
  //No parity, 1 stop bit
  Serial.begin(115200, SERIAL_8N1, 32, 33);
  M5.Lcd.println("Setup complete!");
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  uartProcess();
}
