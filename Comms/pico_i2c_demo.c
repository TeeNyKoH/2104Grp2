#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#define MAX_LENGTH 512
#define I2C_INSTANCE (i2c0)

static int addr = 0x55;

void i2cprintf(const char* fmt, ...);

void receive();
uint8_t recvLength[1];

int main() {
  
  //I2C uses pins GP16 and GP17, speed 100000 bps
  i2c_init(I2C_INSTANCE, 100000);
  gpio_set_function(16, GPIO_FUNC_I2C);
  gpio_set_function(17, GPIO_FUNC_I2C);
  gpio_pull_up(16);
  gpio_pull_up(17);
  
  // Fix for I2C junk data when M5Stick boots
  uint8_t* tmp = malloc(512);
  i2c_read_blocking(I2C_INSTANCE, addr, tmp, 512, false);  
  free(tmp);
  
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
  i2c_write_blocking(I2C_INSTANCE, addr, buffer, msgLength + 1, false);
}

void receive(){
  // Get the amount of messages
  i2c_read_blocking(I2C_INSTANCE, addr, recvLength, 1, false);  
  int msgAmount = recvLength[0];
  
  if(msgAmount == 0){
    return;
  } else {
    printf("msgAmount: %d\n", msgAmount);
    
    for(int i = 0; i < msgAmount; i++){
      
      // Get message length
      i2c_read_blocking(I2C_INSTANCE, addr, recvLength, 1, false);
      printf("recvLength: %d\n", recvLength[0]);
      
      //Allocate memory for message
      uint8_t* buf = (uint8_t*) malloc(recvLength[0]);
      
      // Read the actual message
      i2c_read_blocking(I2C_INSTANCE, addr, buf, recvLength[0], false);
      
      //Print message to debugger
      for(int i = 0; i < recvLength[0]; i++){
        printf("%c", buf[i]);
      }
      printf("\n");
      
      //Determine if message is a command
      if(strncmp(buf, "GOTO", 4) == 0){
        printf("Received GOTO!\n");
      }
      
      //Frees memory used for message
      free(buf);
    }
  }
}