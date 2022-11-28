#include "M5StickCPlus.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>

const char* ssid        = "SINGTEL-4A7C";
const char* password    = "eiyoacaivo";
const char* mqtt_server = "192.168.1.118";
const int port          = 1883;

const char* recvTopic   = "send";
const char* picoTopic   = "car";

WiFiClient espClient;
PubSubClient client(espClient);

typedef struct node{
  uint8_t msgLength;
  uint8_t* message;
  struct node *next;
} NODE;

uint8_t* msgReceived;
uint8_t msgCount = 0;
uint8_t i2cLength[1] = {0};
bool i2c_tick_tock = false;
bool do_once_request = false;
NODE *queueHead = NULL;
NODE *queueTail = NULL;

void enqueue(uint8_t len, uint8_t* msg){
  NODE *new_node = (NODE*)malloc(sizeof(NODE));
  
  if(new_node == NULL) {
    return;
  }

  new_node->msgLength = len;
  new_node->message = msg;
  new_node->next = NULL;

  if(queueHead == NULL) {
    queueHead = new_node;
    queueTail = new_node;
  } else {
    queueTail->next = new_node;
    queueTail = new_node;
  }
  ++msgCount;
}

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
    String clientId = "ESP32Client-02";
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

// Received data from I2C
void receiveEvent(int count){
  uint8_t *tmp = (uint8_t *) malloc(count);
  memset(tmp, '\0', count);
  int i = 0;
  while(1 < Wire.available()) //Exclude the last character (null terminator)
  {
    char c = Wire.read(); // receive byte as a character
    tmp[i] = (uint8_t)c;
    ++i;
  }
  client.publish(picoTopic, tmp, i, false);
  free(tmp);
}

// Callback is when there is a MQTT message
void callback(char* topic, byte* payload, unsigned int length){
  if(strncmp(topic, recvTopic, 4) == 0){
    msgReceived = (uint8_t *) malloc(length);
    memcpy(msgReceived, payload, (int)length);
    enqueue(length, msgReceived);
    for(int i = 0; i < length; i++){
      M5.Lcd.print((char)msgReceived[i]);
    }
    M5.Lcd.println('\0');
  }
}

void requestEvent(){
  if(do_once_request == false){
    Wire.write(msgCount);
    if(queueHead != NULL)
      do_once_request = true;
    return;
  }

  if(queueHead != NULL){  
    if(i2c_tick_tock == false){
      Wire.write(queueHead->msgLength);
      i2c_tick_tock = true;
      return;
    } else {
      // Send the actual message
      Wire.write(queueHead->message, queueHead->msgLength);
      i2c_tick_tock = false;
      if(queueHead == queueTail){
        free(queueHead);
        queueHead = NULL;
        queueTail = NULL;
        msgCount = 0;
        do_once_request = false;
      } else {
        NODE *tmp = queueHead;
        queueHead = queueHead->next;
        free(tmp);
      }
      return;
    }
  }
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
  client.setCallback(callback);

  //Initialize the I2C as slave with address 0x55
  Wire.begin(0x55, 32, 33, 100000);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
