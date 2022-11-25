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

int leftNotch;
int rightNotch;
int leftCount;
int rightCount;

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
    pwm_set_wrap(slice_num, 255);
    // Set channel A output high for one cycle before dropping
    pwm_set_chan_level(slice_num, PWM_CHAN_A, 128);
    // Set initial B output high for three cycles before dropping
    pwm_set_chan_level(slice_num, PWM_CHAN_B, 128);
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
        if(rightNotch == 8)
        {
            stopRight();
            rightNotch = 0;
        }
        if(leftNotch == 9)
        {
            stopLeft();
            leftNotch = 0;
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
        if(rightNotch == 18)
        {
            stopRight();
            rightNotch = 0;
        }
        if(leftNotch == 20)
        {
            stopLeft();
            leftNotch = 0;
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
        if(rightNotch == 8)
        {
            stopRight();
            rightNotch = 0;
        }
        if(leftNotch == 9)
        {
            stopLeft();
            leftNotch = 0;
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
        if(rightNotch >=8 && leftNotch >=8)
         {
             stopRight();
             stopLeft();
             rightNotch = 0;
             leftNotch = 0;
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
        if(rightNotch == 2)
        {
            stopRight();
            rightNotch = 0;
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
        if(leftNotch == 2)
        {
            stopLeft();
            leftNotch = 0;
        }
    }
}

void motorPID()
{
    volatile static float pidLeft = 1;
    volatile static float pidRight = 1;
    static const float KP = 0.03;
    static const float KI = 0.006;
    static const float KD = 0.015;
    // static const float KP = 0.025;
    // static const float KD = 0.025;
    // static const float KI = 0.009;
    volatile static int16_t prevErrorLeft = 0;
    volatile static int16_t prevErrorRight = 0;
    volatile static int16_t sumErrorLeft = 0;
    volatile static int16_t sumErrorRight = 0;
    volatile static int16_t target = 8;

    int16_t errorLeft = target - leftCount;
    int16_t errorRight = target - rightCount;

    pidLeft += (errorLeft * KP) + (prevErrorLeft * KD) + (sumErrorLeft * KI);
    pidRight += (errorRight * KP) + (prevErrorRight * KD) + (sumErrorRight * KI);

    if(pidLeft >= 1)
    {
        pidLeft = 1;
    }
    else if(pidLeft <= 0)
    {
        pidLeft = 0;
    }
    else
    {
        pidLeft = pidLeft;
    }

    if(pidRight >= 1)
    {
        pidRight = 1;
    }
    else if(pidRight <= 0)
    {
        pidRight = 0;
    }
    else
    {
        pidRight = pidRight;
    }

    uint16_t left_duty_cycle = 128 * pidLeft;
    uint16_t right_duty_cycle = 128 * pidRight;

    uint slice_num = pwm_gpio_to_slice_num(8);

    pwm_set_chan_level(slice_num, PWM_CHAN_A, left_duty_cycle);
    pwm_set_chan_level(slice_num, PWM_CHAN_B, right_duty_cycle);
    pwm_set_enabled(slice_num, true);

    leftCount = 0;
    rightCount = 0;

    prevErrorLeft = errorLeft;
    prevErrorRight = errorRight;

    sumErrorLeft += errorLeft;
    sumErrorRight += errorRight;
}

void turnLeft45()
{
    gpio_put(6, 1);
    gpio_put(7, 0);
    gpio_put(10, 1);    
    gpio_put(11, 0);

    while(1)
    {
        if(rightNotch == 4)
        {
            stopRight();
            rightNotch = 0;
        }
        if(leftNotch == 5)
        {
            stopLeft();
            leftNotch = 0;
        }
    }
}

void turnRight45()
{
    gpio_put(6, 0);
    gpio_put(7, 1);
    gpio_put(10, 0);    
    gpio_put(11, 1);

    while(1)
    {
        if(rightNotch == 4)
        {
            stopRight();
            rightNotch = 0;
        }
        if(leftNotch == 5)
        {
            stopLeft();
            leftNotch = 0;
        }
    }
}

void adjustSpeed()
{
    if (leftNotch > rightNotch)
    {
        slowLeft();
    }
    else if (rightNotch > leftNotch)
    {
        slowRight();
    }
}

void slowLeft()
{
    uint16_t leftDC = 128 * 0.95;
    uint16_t rightDC = 128;

    uint slice_num = pwm_gpio_to_slice_num(8);
    pwm_set_chan_level(slice_num, PWM_CHAN_A, leftDC);
    pwm_set_chan_level(slice_num, PWM_CHAN_B, rightDC);
    pwm_set_enabled(slice_num, true);

}

void slowRight()
{
    uint16_t leftDC = 128;
    uint16_t rightDC = 128 * 0.95;

    uint slice_num = pwm_gpio_to_slice_num(8);

    pwm_set_chan_level(slice_num, PWM_CHAN_A, leftDC);
    pwm_set_chan_level(slice_num, PWM_CHAN_B, rightDC);
    pwm_set_enabled(slice_num, true);

}



