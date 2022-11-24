#include "pico/stdlib.h"
#include "pico/binary_info.h"

#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "hardware/pwm.h"
#include "hardware/uart.h"
#include "hardware/i2c.h"
#include "hardware/irq.h"
#include "hardware/adc.h"

#include "motor.h"


#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdio.h>

int leftnotch;
int rightnotch;

void initializeMotor()
{
    printf("init\n");
    gpio_init(6);
    gpio_init(7);
    gpio_init(10);
    gpio_init(11);

    gpio_set_dir(6, GPIO_OUT);
    gpio_set_dir(7, GPIO_OUT);
    gpio_set_dir(10, GPIO_OUT);
    gpio_set_dir(11, GPIO_OUT);

    gpio_put(6, 0);
    gpio_put(7, 0);
    gpio_put(10, 0);    
    gpio_put(11, 0);

    // Tell GPIO 0 and 1 they are allocated to the PWM
    gpio_set_function(8, GPIO_FUNC_PWM);
    gpio_set_function(9, GPIO_FUNC_PWM);

    // Find out which PWM slice is connected to GPIO 0 (it's slice 0)
    uint slice_num = pwm_gpio_to_slice_num(8);

    // Set period of 4 cycles (0 to 3 inclusive)
    pwm_set_wrap(slice_num, 3);
    // Set channel A output high for one cycle before dropping
    pwm_set_chan_level(slice_num, PWM_CHAN_A, 2);
    // Set initial B output high for three cycles before dropping
    pwm_set_chan_level(slice_num, PWM_CHAN_B, 2);
    // Set the PWM running
    pwm_set_enabled(slice_num, true);
    /// \end::setup_pwm[]
}

void forwardCar()
{
    gpio_put(6, 1);
    gpio_put(7, 0);
    gpio_put(10, 0);    
    gpio_put(11, 1);
}

void stopCar()
{
    gpio_put(6, 0);
    gpio_put(7, 0);
    gpio_put(10, 0);    
    gpio_put(11, 0);
}

void stopLeft()
{
    gpio_put(6, 0);
    gpio_put(7, 0);
}

void stopRight()
{
    gpio_put(10, 0);    
    gpio_put(11, 0);
}

void turnLeft90()
{
    gpio_put(6, 1);
    gpio_put(7, 0);
    gpio_put(10, 1);    
    gpio_put(11, 0);
    while(1)
    {
        if(rightnotch == 8)
        {
            stopRight();
            rightnotch = 0;
        }
        if(leftnotch == 9)
        {
            stopLeft();
            leftnotch = 0;
        }
    }
}

void turnLeft180()
{
    gpio_put(6, 1);
    gpio_put(7, 0);
    gpio_put(10, 1);    
    gpio_put(11, 0);
    while(1)
    {
        if(rightnotch == 18)
        {
            stopRight();
            rightnotch = 0;
        }
        if(leftnotch == 20)
        {
            stopLeft();
            leftnotch = 0;
        }
    }
}
void turnRight90()
{
    gpio_put(6, 0);
    gpio_put(7, 1);
    gpio_put(10, 0);    
    gpio_put(11, 1);
    while(1)
    {
        if(rightnotch == 8)
        {
            stopRight();
            rightnotch = 0;
        }
        if(leftnotch == 9)
        {
            stopLeft();
            leftnotch = 0;
        }
    }
}

void reverseCar()
{
    gpio_put(6, 0);
    gpio_put(7, 1);
    gpio_put(10, 1);    
    gpio_put(11, 0);

    while(1)
    {
        if(rightnotch >=8 && leftnotch >=8)
         {
             stopRight();
             stopLeft();
             rightnotch = 0;
             leftnotch = 0;
             turnLeft180();
         }
    }
}

void slightTurnLeft()
{
    gpio_put(6, 1);
    gpio_put(7, 0);
    gpio_put(10, 0);    
    gpio_put(11, 0);

    while(1)
    {
        if(rightnotch == 2)
        {
            stopRight();
            rightnotch = 0;
        }
    }
}

void slightTurnRight()
{
    gpio_put(6, 0);
    gpio_put(7, 0);
    gpio_put(10, 0);    
    gpio_put(11, 1);
    while(1)
    {
        if(leftnotch == 2)
        {
            stopLeft();
            leftnotch = 0;
        }
    }
}
