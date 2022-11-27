#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#define MQTT_MAX_LENGTH 1024

const char* ssid        = "SINGTEL-4A7C";
const char* password    = "eiyoacaivo";
const char* mqtt_server = "192.168.1.118";
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
	free(msgReceived);
  }
}

void wifiSetup() {
  delay(10);

  WiFi.mode(WIFI_STA);          //Configure as WiFi station
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

// Loop until we're reconnected
void reconnect() {
  while (!client.connected()) {
    String clientId = "ESP8266Client-01";
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      // Once connected, publish (sending) an announcement...
      client.publish(picoTopic, "Reconnected.");
      // ... and resubscribe (receiving)
      client.subscribe(recvTopic);
    } else {
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
  wifiSetup();

  //Initialize MQTT
  client.setBufferSize(1024);
  client.setServer(mqtt_server, port);
  client.setCallback(callback);
  
  //Initialize UART for analysis
  //No parity, 1 stop bit
  Serial.begin(115200, SERIAL_8N1);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  uartProcess();
}
