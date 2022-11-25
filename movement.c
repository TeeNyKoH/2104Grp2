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

void MSPmoveCarForward(unsigned int pin0, unsigned int pin1, unsigned int pin2)
{
    gpio_put(pin0, 0);
    gpio_put(pin1, 0);
    gpio_put(pin2, 1);
    // printf("front\n");
    // if(is_center() == 1){
    //     slightTurnRight(pin0,pin1,pin2);

    // }
    // else{
    //     slightTurnLeft(pin0,pin1,pin2);

    // }
    // reset(pin0,pin1,pin2);
}

void MSPstopCar(unsigned int pin0, unsigned int pin1, unsigned int pin2)
{
    gpio_put(pin0, 0);
    gpio_put(pin1, 1);
    gpio_put(pin2, 0);
    printf("stop\n");
}

void MSPturnLeft90(unsigned int pin0, unsigned int pin1, unsigned int pin2)
{
    gpio_put(pin0, 0);
    gpio_put(pin1, 1);
    gpio_put(pin2, 1);
    printf("left\n");

}

void MSPturnRight90(unsigned int pin0, unsigned int pin1, unsigned int pin2)
{
    gpio_put(pin0, 1);
    gpio_put(pin1, 0);
    gpio_put(pin2, 0);
    printf("right\n");

}

void MSPreverseCar(unsigned int pin0, unsigned int pin1, unsigned int pin2)
{
    gpio_put(pin0, 1);
    gpio_put(pin1, 0);
    gpio_put(pin2, 1);
    printf("reverse\n");

}

void MSPslightTurnLeft(unsigned int pin0, unsigned int pin1, unsigned int pin2)
{
    gpio_put(pin0, 1);
    gpio_put(pin1, 1);
    gpio_put(pin2, 0);
    printf("slight left\n");
}

void MSPslightTurnRight(unsigned int pin0, unsigned int pin1, unsigned int pin2)
{
    gpio_put(pin0, 1);
    gpio_put(pin1, 1);
    gpio_put(pin2, 1);
    printf("slight right\n");
}

void MSPreset(unsigned int pin0, unsigned int pin1, unsigned int pin2){
    gpio_put(pin0, 0);
    gpio_put(pin1, 0);
    gpio_put(pin2, 0);
    printf("reset\n");
}