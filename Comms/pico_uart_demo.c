#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#define UART_INSTANCE (uart0)
#define UART_TX 16
#define UART_RX 17
#define UART_SPEED 115200
#define DATA_BIT 8
#define STOP_BIT 1
#define PARITY_BIT (UART_PARITY_NONE)
#define BUFFER_SIZE 128
#define MQTT_MAX_LENGTH 1024
#define EOT 0x04

void uartprintf(const char* fmt, ...);
void long_printf(const char* fmt, ...);

void readyPrintf();
void stopPrintf();
void receive();

char buffer[BUFFER_SIZE];
char receiveBuffer[BUFFER_SIZE];
static int mqtt_printStatus = 0;
char mqtt[MQTT_MAX_LENGTH];
static int mqtt_length = 0;

int main() {
  
  //UART uses pins GP16 and GP17, speed 115200 bps (standard max)
  uart_init(UART_INSTANCE, UART_SPEED);
  //8 data bits, 1 stop bit, 0 parity bits
  uart_set_format(UART_INSTANCE, DATA_BIT, STOP_BIT, PARITY_BIT);
  uart_set_fifo_enabled(UART_INSTANCE, true);
  gpio_set_function(UART_TX, GPIO_FUNC_UART);
  gpio_set_function(UART_RX, GPIO_FUNC_UART);
  
  int pico = 5000;
  while(1){
    /*readyPrintf();
    for(int i = 0; i < 20; i++){
      long_printf("Raspberry Pi Pico %d\n", pico);
    }
    stopPrintf();*/
    printf("%d\n", uart_is_readable(UART_INSTANCE));
    if (uart_is_readable(UART_INSTANCE)){
      receive();
    }
    sleep_ms(1000);
    uartprintf("Still alive!");
    sleep_ms(1000);
  }
}

// Directly print a statement to MQTT
void uartprintf(const char* fmt, ...){
  memset(buffer, '\0', BUFFER_SIZE);
  va_list args;
  va_start(args, fmt);
  int msgLength = vsnprintf(buffer, sizeof(buffer), fmt, args);
  va_end(args);
  buffer[msgLength] = EOT;
  uart_write_blocking(UART_INSTANCE, buffer, msgLength + 1);
}

// For printing long messages that require multiple "printf" 's
void readyPrintf(){
  //Clear previous messages
  memset(mqtt, '\0', MQTT_MAX_LENGTH);
  mqtt_length = 0;
  mqtt_printStatus = 1;
}

void long_printf(const char* fmt, ...){
  if (mqtt_printStatus == 0){
    // Did not call readyPrintf first, so return.
    return;
  }
  // Keep concatenating strings
  va_list args;
  va_start(args, fmt);
  memset(buffer, '\0', BUFFER_SIZE);
  int msgLength = vsnprintf(buffer, sizeof(buffer), fmt, args);
  va_end(args);
  
  if (mqtt_length == 0){
    strncpy(mqtt, buffer, msgLength);
    mqtt_length += msgLength;
  } else if (mqtt_length + msgLength < MQTT_MAX_LENGTH){
    strncat(mqtt, buffer, msgLength);
    mqtt_length += msgLength;
  } else {
    printf("long_printf overflow!\n");
  }
}

void stopPrintf(){
  if (mqtt_printStatus == 0){
    // Did not call readyPrintf first, so return.
    return;
  }
  // Attach EOT and send
  mqtt[mqtt_length] = EOT;
  ++mqtt_length;
  uart_write_blocking(UART_INSTANCE, mqtt, mqtt_length);
  mqtt_printStatus = 0;
}

void receive(){
  
  if (!uart_is_readable(UART_INSTANCE)){
      return;
  }
  
  static char ch;
  static int charCount = 0;
  
  memset(receiveBuffer, '\0', BUFFER_SIZE);
  
  // Check for message header 0x01 (Start of Header)
  ch = uart_getc(UART_INSTANCE);
  if (ch != 0x01){
    return;
  }
  
  while(uart_is_readable(UART_INSTANCE)){
    ch = uart_getc(UART_INSTANCE);
    // Check and continue if not 0x04 (End of Transmission)
    if(ch != EOT && charCount < BUFFER_SIZE){
      receiveBuffer[charCount] = ch;
      charCount++;
    } 
  }
  
  if (charCount == 8){
    if (strncmp(receiveBuffer, "GOTO", 4) == 0){
      if(isdigit(receiveBuffer[5]) && isdigit(receiveBuffer[7])){
        int x = receiveBuffer[5] - '0';
        int y = receiveBuffer[7] - '0';
        
        //Insert mapping goto here.
        uartprintf("COMMAND RECEIVED: GOTO %d %d\n", x, y);
      }
    }      
  }
  
  charCount = 0;
}
