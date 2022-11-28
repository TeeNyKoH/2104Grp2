#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"

static int addr = 0x55;

uint8_t ch_send[127];
uint8_t ch_recv[127];

int main() {
  
  //I2C uses pins GP18 and GP19, speed 100000 bps
  i2c_init(i2c0, 100000);
  gpio_set_function(16, GPIO_FUNC_I2C);
  gpio_set_function(17, GPIO_FUNC_I2C);
  gpio_pull_up(16);
  gpio_pull_up(17);
  
  memset(ch_send, 'a', 127);
  memset(ch_recv, 0, 127);
  
  for (int i = 0; i < 25; i++){
    i2c_write_blocking(i2c0, addr, ch_send, 127, false);
    i2c_read_blocking(i2c0, addr, ch_recv, 127, false);
    sleep_ms(1);
  }
}