#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"

static int addr = 0x55;

uint8_t ch_send[1] = {'a'};
uint8_t ch_recv[1] = {0};

int main() {
  
  //I2C uses pins GP18 and GP19, speed 100000 bps
  i2c_init(i2c0, 100000);
  gpio_set_function(16, GPIO_FUNC_I2C);
  gpio_set_function(17, GPIO_FUNC_I2C);
  gpio_pull_up(16);
  gpio_pull_up(17);
  
  // Clear I2C RX buffer
  uint8_t* tmp = malloc(i2c_get_read_available(i2c0));
  i2c_read_blocking(i2c0, addr, tmp, i2c_get_read_available(i2c0), false);
  
  for(int i = 0; i < 25; i++){
    i2c_write_blocking(i2c0, addr, ch_send, 1, false);
    i2c_read_blocking(i2c0, addr, ch_recv, 1, false);
    sleep_ms(1);
  }
}