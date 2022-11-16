#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#define MAX_LENGTH 512

static int addr = 0x55;

uint8_t msgHeader[4] = {0x23, 0x60, 0x71, 0x95};
uint8_t recvHeader[4] = {0x00, 0x00, 0x00, 0x00};

void i2cprintf(const char* fmt, ...);
void receive();
uint8_t recvLength[1];

int main() {
  printf("Pico Pins: %d, %d\n", PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN);
  // This example will use I2C0 on the default SDA and SCL pins (GP4, GP5 on a Pico)
  i2c_init(i2c_default, 100 * 1000);
  gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
  gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
  gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
  gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
  // Make the I2C pins available to picotool
  bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));
  
  int pico = 5000;

  while(1){
    i2cprintf("Raspberry Pi Pico %d", pico);
    sleep_ms(1000);
    receive();
    sleep_ms(1000);
  }
}

void i2cprintf(const char* fmt, ...){
  char buffer[MAX_LENGTH];
  va_list args;
  va_start(args, fmt);
  int msgLength = vsnprintf(buffer, sizeof(buffer), fmt, args);
  va_end(args);
  i2c_write_blocking(i2c_default, addr, buffer, msgLength + 1, false);
}

void receive(){
  /*repeat:
  do {
    i2c_read_blocking(i2c_default, addr, recvLength, 1, true);  
  }
  while(recvLength[0] != 0x95);
  
  i2c_read_blocking(i2c_default, addr, recvHeader, 4, false);    
  if(strncmp(recvHeader, msgHeader, 4) != 0){
    goto repeat;
  }*/
  
  // Get the amount of messages
  i2c_read_blocking(i2c_default, addr, recvLength, 1, false);  
  int msgAmount = recvLength[0];
  printf("msgAmount: %d\n", msgAmount);
  
  if(msgAmount == 0){
    return;
  } else {
    for(int i = 0; i < msgAmount; i++){     
      // Get message length
      i2c_read_blocking(i2c_default, addr, recvLength, 1, false);
      printf("recvLength: %d\n", recvLength[0]);
      
      uint8_t* buf = (uint8_t*) malloc(recvLength[0]);
      
      // Read the actual message
      i2c_read_blocking(i2c_default, addr, buf, recvLength[0], false);
      
      for(int i = 0; i < recvLength[0]; i++){
        printf("%c", buf[i]);
      }
      
      free(buf);
    }
  }
}
