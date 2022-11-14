#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#define MAX_LENGTH 512

static int addr = 0x55;

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
  // Get the length of the message
  i2c_read_blocking(i2c_default, addr, recvLength, 1, false);  
  if(recvLength[0] == '\0')
    return;
  uint8_t* buf = (uint8_t*) malloc(recvLength[0]);
  
  // Read the actual message
  i2c_read_blocking(i2c_default, addr, buf, recvLength[0], false);
  for(int i = 0; i < recvLength[0]; i++){
    printf("%c", buf[i]);
  }
  free(buf);
}
