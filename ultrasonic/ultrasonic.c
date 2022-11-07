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
    // uint trigPin2 = 14;
    uint echoPin2 = 15;
    // uint trigPin3 = 17;
    uint echoPin3 = 16;
    setupUltrasonicPins(trigPin1, echoPin1); //front
    setupUltrasonicPins(trigPin1, echoPin2); //left
    setupUltrasonicPins(trigPin1, echoPin3); //right


    float front,avgfront = 0,avgleft = 0,avgright = 0,left,right; 

    while(1){
        front = getCm(trigPin1, echoPin1);
        left = getCm(trigPin1, echoPin2);
        right = getCm(trigPin1, echoPin3);
        
        int count = 1;
        int i = 0;
        for (int i = 0; i < 50; ++i)
            {
            avgfront = front + avgfront;
            avgleft = left + avgleft;
            avgright = right + avgright;
        
            
            }
        avgfront = (avgfront/50) + 2.5;
        avgleft = (avgleft/50) + 2.5;
        avgright = (avgright/50) + 2.5;

        

        printf("\n %.2f cm of front",getCm(trigPin1, echoPin1));
        printf("\n %.2f cm of left",getCm(trigPin1, echoPin2));
        printf("\n %.2f cm of right",getCm(trigPin1, echoPin3));
        
        printf("\n %.2f cm front avg",avgfront);
        printf("\n %.2f cm left avg",avgleft);
        printf("\n %.2f cm right avg",avgright);
        if (avgfront < 5.5)
        {
            printf("\n Front Blocked/Detected");
        }
        else
        {
            printf("\n Front Clear");
        }


        if (avgleft < 5.5)
        {
            printf("\n Left Blocked");
            
        }
        else
        {
            printf("\n Left Clear");
        }

        if (avgright < 5.5)
        {
            printf("\n Right Blocked");
        }
        else
        {
            printf("\n Right Clear");
        }

        if (avgleft > avgright)
        {
            printf("\n car slanted left");
        }

        if (avgright > avgleft)
        {
            printf("\n car slanted right");
        }
        sleep_ms(40);
}
}
