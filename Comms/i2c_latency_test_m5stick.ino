#include "M5StickCPlus.h"
#include "Wire.h"

void setup() {
  //Initialize the M5
  M5.begin();

  Wire.begin(0x55, 32, 33, 100000);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  M5.Lcd.println("Setup complete!");
}

void receiveEvent(int count){
  //Do not compare whatever character is received, just receive and clear it from RX buffer.
  Wire.read();
}

void requestEvent(){
  Wire.write('b');
}

void loop() {

}