#include <stdio.h>
#include <ctype.h>
#include "communications.h"

char buffer[BUFFER_SIZE];
char receiveBuffer[BUFFER_SIZE];
static int mqtt_printStatus = 0;
char mqtt[MQTT_MAX_LENGTH];
static int mqtt_length = 0;

void initializeComms(){
  //UART uses UART0 pins GP12 and GP13, speed 115200 bps (standard max)
  uart_init(UART_INSTANCE, UART_SPEED);
  //8 data bits, 1 stop bit, 0 parity bits
  uart_set_format(UART_INSTANCE, DATA_BIT, STOP_BIT, PARITY_BIT);
  uart_set_fifo_enabled(UART_INSTANCE, true);
  gpio_set_function(UART_TX, GPIO_FUNC_UART);
  gpio_set_function(UART_RX, GPIO_FUNC_UART);
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
  
  // Retrieves the messages until End of Transmission (EOT) is received
  while(uart_is_readable(UART_INSTANCE)){
    ch = uart_getc(UART_INSTANCE);
    // Check and continue if not 0x04 (End of Transmission)
    if(ch != EOT && charCount < BUFFER_SIZE){
      receiveBuffer[charCount] = ch;
      charCount++;
    } 
  }
  
  // Checking if the message received is a command
  if (charCount == 8){
    if (strncmp(receiveBuffer, "GOTO", 4) == 0){
      if(isdigit(receiveBuffer[5]) && isdigit(receiveBuffer[7])){
        int x = receiveBuffer[5] - '0';
        int y = receiveBuffer[7] - '0';
        
        //Insert mapping command goto here.
        uartprintf("COMMAND RECEIVED: GOTO %d %d\n", x, y);
      }
    }      
  }
  
  charCount = 0;
}
