#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#define UART_INSTANCE (uart0)
#define UART_SPEED 115200
#define DATA_BIT 8
#define STOP_BIT 1
#define PARITY_BIT (UART_PARITY_NONE)

int main() {
  
  //UART uses pins GP16 and GP17, speed 115200 bps (standard max)
  uart_init(UART_INSTANCE, UART_SPEED);
  //8 data bits, 1 stop bit, 0 parity bits
  uart_set_format(UART_INSTANCE, DATA_BIT, STOP_BIT, PARITY_BIT);
  uart_set_fifo_enabled(UART_INSTANCE, false);
  gpio_set_function(16, GPIO_FUNC_UART);
  gpio_set_function(17, GPIO_FUNC_UART);
  
  static char ch_send = 'a';
  
  //Clear UART RX
  while(uart_is_readable(UART_INSTANCE))
    uart_getc(UART_INSTANCE);
  
  for(int i = 0; i < 25; i++){
    //Send 'a'
    uart_putc_raw(UART_INSTANCE, ch_send);
    //Do not compare whatever character is received, just receive and clear it from RX buffer.
    while(!uart_is_readable(UART_INSTANCE));
    uart_getc(UART_INSTANCE);
    sleep_ms(1);
  }
  
}
