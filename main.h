#ifndef main_h
#define main_h

extern int trigPin;
extern int echoPin1, echoPin2, echoPin3;

#define UART_INSTANCE (uart0)
#define UART_TX 12
#define UART_RX 13
#define UART_SPEED 115200
#define DATA_BIT 8
#define STOP_BIT 1
#define PARITY_BIT (UART_PARITY_NONE)

#endif
