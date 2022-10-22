#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"

static int addr = 0x55;

void send(char* msg, int length);

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
    
    char pico_ad[50] = "Raspberry Pi Pico\0";
    while(1){
      send(pico_ad, 50);
      sleep_ms(5000);
    }
}

void send(char* msg, int length){
  uint8_t *tmp = (uint8_t *) malloc(length + 1);
  memset(tmp, '\0', length + 1);
  for(int i = 0; i < length; i++){
    tmp[i] = (uint8_t)msg[i];
  }
  tmp[length] = '\0';
  i2c_write_blocking(i2c_default, addr, tmp, length + 1, false);
}
