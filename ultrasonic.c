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
#include "hardware/irq.h"
#include "ultrasonic.h"

/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* 
   GPIO PICO_DEFAULT_I2C_SDA_PIN (On Pico this is GP4 (pin 6)) -> SDA on MPU6050 board
   GPIO PICO_DEFAULT_I2C_SCL_PIN (On Pico this is GP5 (pin 7)) -> SCL on MPU6050 board
   3.3v (pin 36) -> VCC on MPU6050 board
   GND (pin 38)  -> GND on MPU6050 board
*/


float ultrasonic_Front = 0, ultrasonic_Avg_Front = 0, ultrasonic_Avg_Left = 0, ultrasonic_Avg_Right = 0, ultrasonic_Left = 0, ultrasonic_Right = 0; 
int j = 0;
int left_block = 0,front_block = 0,right_block = 0,centerCar = 0,HumpStatus = 0;
/* Set up for ultrasonic Pins */
void setupUltrasonicPins(int trigPin, int echoPin)
{
    gpio_init(trigPin);
    gpio_init(echoPin);
    
    gpio_set_dir(trigPin, GPIO_OUT);
    gpio_set_dir(echoPin, GPIO_IN);

    gpio_pull_up(echoPin);
}

/* Setup for ultrasonic Pulse */

int getPulse(int trigPin, int echoPin)
{
    int loop1 = 0;
    int loop2 = 0;
    
    busy_wait_us(2);
    gpio_put(trigPin, 1);
    busy_wait_us(10);
    gpio_put(trigPin, 0);
    busy_wait_us(2);

    while (gpio_get(echoPin) == 0 ) 
    {
        loop1++;
        busy_wait_us(1);
        if(loop1 > 5000){
            break;
        }
    }
    
    while (gpio_get(echoPin) != 0 ) 
    {
        loop2++;
        busy_wait_us(1);
        if(loop2 > 3000){
            return 0;
        }
    }
    return loop2;
}

/* Converting pulse into cm */
float getCm(int trigPin, int echoPin)
{
    int pulseLength = getPulse(trigPin, echoPin);
    return (float)pulseLength/2 * 0.0343;
}

/* Get values for the ultrasonic detection calculations for varaibles to be sent */
float getUSDectection(int trigPin, int echoPin1, int echoPin2, int echoPin3)
{
    /* Getting the cm for each sensor */  
    ultrasonic_Front = getCm(trigPin, echoPin1);
    ultrasonic_Left = getCm(trigPin, echoPin2);
    ultrasonic_Right = getCm(trigPin, echoPin3);

    /* Get the average of the directions for smoothing  */   
    for (int j = 0; j < 50; ++j)
        {
        ultrasonic_Avg_Front = ultrasonic_Front + ultrasonic_Avg_Front;
        ultrasonic_Avg_Left = ultrasonic_Left + ultrasonic_Avg_Left;
        ultrasonic_Avg_Right = ultrasonic_Right + ultrasonic_Avg_Right;
        // printf("%.2dcount\n",j);
        if(j == 50)
        {
            j = 0;
        }
        }

    /* Hardcoded addition as sensors give a set wrong value */
    ultrasonic_Avg_Front = ( ultrasonic_Avg_Front / 50 );
    ultrasonic_Avg_Left = ( ultrasonic_Avg_Left / 50 );
    ultrasonic_Avg_Right = ( ultrasonic_Avg_Right / 50 );

    //to uncommment
    // printf("%.2f cm of front\n",getCm(trigPin, echoPin1));
    // printf("%.2f cm of left\n",getCm(trigPin, echoPin2));
    // printf("%.2f cm of right\n",getCm(trigPin, echoPin3));
    
    // printf("%.2f cm front avg\n",ultrasonic_Avg_Front);
    // printf("%.2f cm left avg\n",ultrasonic_Avg_Left);
    // printf("%.2f cm right avg\n",ultrasonic_Avg_Right);

    
    
    // /* Variables to be given to mapping */
    // if (ultrasonic_Front < 5.5)
    // {printf("Front Blocked/Detected\n");
    //     front_block = 1;
    // }
    // else
    // {printf("Front Clear\n");
    // front_block = 0;}

    // if (ultrasonic_Left < 5.5)
    // {printf("Left Blocked\n");
    //     left_block = 1;
    // }
    // else
    // {printf("Left Clear\n");
    // left_block = 0;}

    // if (ultrasonic_Right < 5.5)
    // {printf("Right Blocked\n");
    // right_block = 1;
    // }
    // else
    // {printf("Right Clear\n");
    // right_block = 0;}

    // if (ultrasonic_Left > ultrasonic_Right)
    // {printf("car slanted left\n");
    // centerCar = 1;}

    // if (ultrasonic_Right > ultrasonic_Left)
    // {printf("car slanted right\n");
    // centerCar = 1;}

    // uncomment end
}

