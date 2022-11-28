#include "M5StickCPlus.h"

void setup() {
  //Initialize the M5
  M5.begin();
  
  pinMode(32, INPUT);
  pinMode(33, OUTPUT);
  
  Serial.begin(115200, SERIAL_8N1, 32, 33);
  M5.Lcd.println("Setup complete!");
}

void loop() {
  while (Serial.available() > 0) {
    //Do not compare whatever character is received, reply first and clear it from RX buffer.
    Serial.write('b');
    Serial.read();
  }
}