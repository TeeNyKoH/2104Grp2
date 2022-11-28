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
  uart_set_fifo_enabled(UART_INSTANCE, true);
  gpio_set_function(16, GPIO_FUNC_UART);
  gpio_set_function(17, GPIO_FUNC_UART);
  
  uint8_t ch_send[127];
  uint8_t ch_recv[127];
  
  memset(ch_send, 'a', 127);
  memset(ch_recv, 0, 127);
  
  //Clear UART RX
  while(uart_is_readable(UART_INSTANCE))
    uart_getc(UART_INSTANCE);
  
  for(int i = 0; i < 25; i++){
    uart_write_blocking(UART_INSTANCE, ch_send, 127);
    uart_read_blocking(UART_INSTANCE, ch_recv, 127);
    sleep_ms(1);
  }
  
}
