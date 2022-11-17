#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "hardware/uart.h"
#include <string.h>
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#include <stdlib.h>
#include <time.h>

void moveCarForward(unsigned int pin0, unsigned int pin1, unsigned int pin2)
{
    gpio_pull_down(pin0);
    gpio_pull_down(pin1);
    gpio_pull_up(pin2);

    printf("hi\n");
}

void stopCar(unsigned int pin0, unsigned int pin1, unsigned int pin2)
{
    gpio_pull_down(pin0);
    gpio_pull_up(pin1);
    gpio_pull_down(pin2);
}

void turnLeft90(unsigned int pin0, unsigned int pin1, unsigned int pin2)
{
    gpio_pull_down(pin0);
    gpio_pull_up(pin1);
    gpio_pull_up(pin2);
}

void turnRight90(unsigned int pin0, unsigned int pin1, unsigned int pin2)
{
    gpio_pull_up(pin0);
    gpio_pull_down(pin1);
    gpio_pull_down(pin2);
}

void reverseCar(unsigned int pin0, unsigned int pin1, unsigned int pin2)
{
    gpio_pull_up(pin0);
    gpio_pull_down(pin1);
    gpio_pull_up(pin2);
}

void slightTurnLeft(unsigned int pin0, unsigned int pin1, unsigned int pin2)
{
    gpio_pull_up(pin0);
    gpio_pull_up(pin1);
    gpio_pull_down(pin2);
}

void slightTurnRight(unsigned int pin0, unsigned int pin1, unsigned int pin2)
{
    gpio_pull_up(pin0);
    gpio_pull_up(pin1);
    gpio_pull_up(pin2);
}