/*
This is the source code for the car integration for Raspberry Pi Pico

Issues:
Due to a problem with the Raspberry Pi Pico SDK, the UART cannot be changed from 8N1 (8 bits of data, no parity bit, 1 stop bit)
https://github.com/raspberrypi/pico-sdk/issues/548
*/

#ifndef communications_h
#define communications_h

#include <stdarg.h>
#include "pico/stdlib.h"
#define BUFFER_SIZE 128
#define MAX_LENGTH 1024
#define EOT 0x04

void uartprintf(const char* fmt, ...);
void long_printf(const char* fmt, ...);

void readyPrintf();
void stopPrintf();
void receive();
#endif
