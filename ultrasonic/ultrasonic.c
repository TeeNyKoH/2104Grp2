#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "hardware/uart.h"
void setupUltrasonicPins(uint trigPin, uint echoPin)
{
    gpio_init(trigPin);
    gpio_init(echoPin);
    
    gpio_set_dir(trigPin, GPIO_OUT);
    gpio_set_dir(echoPin, GPIO_IN);

    gpio_pull_up(echoPin);
}



int getPulse(uint trigPin, uint echoPin)
{
    int loop1 = 0;
    int loop2 = 0;
    
    sleep_us(2);
    gpio_put(trigPin, 1);
    sleep_us(20);
    gpio_put(trigPin, 0);
    sleep_us(2);
    
    while (gpio_get(echoPin) == 0)
    {
        loop1++;
        sleep_us(1);
        if(loop1 > 5000){
            break;
        }
    }

    while (gpio_get(echoPin) != 0 ) 
    {
        loop2++;
        sleep_us(1);
        if(loop2 > 3000){
            return 0;
        }
    }
   
    return loop2;
    
}

float getCm(uint trigPin, uint echoPin)
{
    int pulseLength = getPulse(trigPin, echoPin);
    return (float)pulseLength/2  * 0.0343;
}



int main() {
   stdio_init_all();

    uint trigPin1 = 0;
    uint echoPin1 = 1;
    uint trigPin2 = 2;
    uint echoPin2 = 3;
    uint trigPin3 = 4;
    uint echoPin3 = 5;
    setupUltrasonicPins(trigPin1, echoPin1); //front
    setupUltrasonicPins(trigPin2, echoPin2); //left
    setupUltrasonicPins(trigPin3, echoPin3); //right


    float front,avgfront,avgleft,avgright,diff,left,right; 

    while(1){
        front = getCm(trigPin1, echoPin1);
        left = getCm(trigPin2, echoPin2);
        right = getCm(trigPin3, echoPin3);
        
        for (int i = 1; i < 50; ++i)
            {
            avgfront = avgfront + avgfront;
            avgleft = avgleft + avgleft;
            avgright = avgright + avgright;
        
            
            }
        avgfront = avgfront/49;
        avgleft = avgleft/49;
        avgright = avgright/49;

        printf("\n %.2f cm of front",getCm(trigPin1, echoPin1));
        printf("\n %.2f cm of left",getCm(trigPin2, echoPin2));
        printf("\n %.2f cm of right",getCm(trigPin3, echoPin3));
        sleep_ms(40);
        printf("\n %.2f cm avg",avgfront);
        printf("\n %.2f cm avg",avgleft);
        printf("\n %.2f cm avg",avgright);
        if (avgfront < 9.5)
        {
            printf("\n Front Blocked/Detected");
        }
        else
        {
            printf("\n Front Clear");
        }


        if (avgleft < 3)
        {
            printf("\n Left Blocked/Detected");
        }
        else
        {
            printf("\n Left Clear");
        }

        if (avgright < 3)
        {
            printf("\n Right Blocked/Detected");
        }
        else
        {
            printf("\n Right Clear");
        }
}
}
