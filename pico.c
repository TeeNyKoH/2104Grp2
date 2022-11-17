#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#define MAX_LENGTH 512

static int addr = 0x55;
uint8_t testNum[1] = {0x8F};

void i2cprintf(const char* fmt, ...);
void uartprintf(const char* fmt, ...);

void receive();
uint8_t recvLength[1];
uint32_t i2cLatency;
uint32_t uartLatency;

int main() {
  
  //I2C uses pins GP18 and GP19, speed 100000 bps
  i2c_init(i2c1, 100000);
  gpio_set_function(18, GPIO_FUNC_I2C);
  gpio_set_function(19, GPIO_FUNC_I2C);
  gpio_pull_up(18);
  gpio_pull_up(19);
  
  //UART uses pins GP16 and GP17, speed 112500 bps
  uart_init(uart0, 115200);
  //8 data bits, 1 stop bit, 0 parity bits
  uart_set_format(uart0, 8, 1, UART_PARITY_NONE);
  gpio_set_function(16, GPIO_FUNC_UART);
  gpio_set_function(17, GPIO_FUNC_UART);
  
  // Jank fix for I2C junk data when M5Stick boots
  uint8_t* tmp = malloc(512);
  i2c_read_blocking(i2c1, addr, tmp, 512, false);  
  free(tmp);
  
  int pico = 5000;
  while(1){
    i2cprintf("Raspberry Pi Pico %d", pico);
    sleep_ms(1000);
    receive();
    sleep_ms(1000);
  }
}

void latencyTest(){
  printf("Start");
  uint32_t currentTime = 0;
  uint32_t receiveTime = 0;
  i2c_write_blocking(i2c1, addr, testNum, 1, false);
  currentTime = time_us_32();
  i2c_read_blocking_until(i2c1, addr, recvLength, 1, false, 1000000);
  receiveTime = time_us_32();
  i2cLatency = receiveTime - currentTime;
  i2c_read_blocking(i2c1, addr, recvLength, 1, false);
   
  printf("Start2");
  uart_write_blocking(uart0, testNum, 1);
  currentTime = time_us_32();
  while(uart_is_readable(uart0) == 0);
  receiveTime = time_us_32();
  uartLatency = receiveTime - currentTime;
  uart_read_blocking(uart0, recvLength, 1);
  printf("%d %d", i2cLatency, uartLatency);
  printf("End");
}

void i2cprintf(const char* fmt, ...){
  char buffer[MAX_LENGTH];
  va_list args;
  va_start(args, fmt);
  int msgLength = vsnprintf(buffer, sizeof(buffer), fmt, args);
  va_end(args);
  i2c_write_blocking(i2c1, addr, buffer, msgLength + 1, false);
}

void uartprintf(const char* fmt, ...){
  char buffer[MAX_LENGTH];
  va_list args;
  va_start(args, fmt);
  int msgLength = vsnprintf(buffer, sizeof(buffer), fmt, args);
  va_end(args);
  i2c_write_blocking(i2c1, addr, buffer, msgLength + 1, false);
}

void receive(){
  /*repeat:
  do {
    i2c_read_blocking(i2c1, addr, recvLength, 1, true);  
  }
  while(recvLength[0] != 0x95);
  
  i2c_read_blocking(i2c1, addr, recvHeader, 4, false);    
  if(strncmp(recvHeader, msgHeader, 4) != 0){
    goto repeat;
  }*/
  
  // Get the amount of messages
  i2c_read_blocking(i2c1, addr, recvLength, 1, false);  
  int msgAmount = recvLength[0];
  
  if(msgAmount == 0){
    return;
  } else {
    printf("msgAmount: %d\n", msgAmount);
    
    for(int i = 0; i < msgAmount; i++){
      
      // Get message length
      i2c_read_blocking(i2c1, addr, recvLength, 1, false);
      printf("recvLength: %d\n", recvLength[0]);
      
      //Allocate memory for message
      uint8_t* buf = (uint8_t*) malloc(recvLength[0]);
      
      // Read the actual message
      i2c_read_blocking(i2c1, addr, buf, recvLength[0], false);
      
      //Print message to debugger
      for(int i = 0; i < recvLength[0]; i++){
        printf("%c", buf[i]);
      }
      printf("\n");
      
      //Determine if message is a command
      if(strncmp(buf, "GOTO", 4) == 0){
        printf("Received GOTO!\n");
      }
      
      if(strncmp(buf, "LATENCY", 7) == 0){
        latencyTest();
        i2cprintf("I2C Latency: %d microseconds", i2cLatency);
        i2cprintf("UART Latency: %d microseconds", uartLatency);
      }
      
      //Frees memory used for message
      free(buf);
    }
  }
}
