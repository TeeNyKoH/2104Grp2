#include "M5StickCPlus.h"
#include "Wire.h"

uint8_t ch_send[127];

/*void receiveEvent(int count){
  //Do not compare whatever character is received, just receive and clear it from RX buffer.
  while(0 < Wire.available()){
    Wire.read();
  }
}*/

void requestEvent(){
  Wire.write(ch_send, 127);
}

void setup() {
  //Initialize the M5
  M5.begin();

  //Wire.setBufferSize(1536);
  Wire.begin(0x55, 32, 33, 100000);
  //Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  memset(ch_send, 'b', 127);
  
  M5.Lcd.println("Setup complete!");
}

void loop() {
  Wire.read();
}