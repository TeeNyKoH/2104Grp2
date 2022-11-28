#include "M5StickCPlus.h"

char ch_send[127];

void setup() {
  //Initialize the M5
  M5.begin();

  pinMode(32, INPUT);
  pinMode(33, OUTPUT);
  memset(ch_send, 'b', 127);

  Serial.begin(115200, SERIAL_8N1, 32, 33);
  M5.Lcd.println("Setup complete!");
}

void loop() {
  int i = 0;
  while (i < 127) {
    while (Serial.available() > 0) {
      //Do not compare whatever character is received, reply first and clear it from RX buffer.
      Serial.read();
      i++;
    }
  }
  Serial.write(ch_send, 127);
}