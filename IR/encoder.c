/*  Left = Gnd
**  Middle = Vcc
**  Right = output
*/

#include "hardware/irq.h"
#include "hardware/timer.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include "encoder.h"

int wheelRotation1, wheelRotation2, wheelTime1, wheelTime2;
float wheelDistance1, wheelDistance2;
int done1 = 0, done2 = 0;
int startTime, stopTime1, stopTime2, currentTime;
double avgSpeed = 0.0f, totalDistance = 0.0f, tempDistance = 0.0f;
double currentDistance,currentSpeed = 0.0f;


double get_total_distance()
{
    return totalDistance;
}

double get_average_speed()
{
    stopTime1 = time_us_32();
    wheelTime1 = (stopTime1 - startTime) / 1000000; // time for 1 rotation in seconds
    avgSpeed = (totalDistance / wheelTime1);

    return avgSpeed;
}

static void wheel_encoder(unsigned gpio, long unsigned int _)
{
    if (gpio == 17)
    {
        wheelRotation1++;                               // right wheel
        wheelDistance1 = wheelRotation1 * 0.165 * PI;
    }
    if (gpio == 14)
    {
        wheelRotation2++;                               // left wheel
        wheelDistance2 = wheelRotation2 * 0.165 * PI;   // d = 6.6cm, distance = notches * 6.6pi / 40
    }

    if (wheelRotation1 == 1 && wheelRotation2 == 1)
    {
        startTime = time_us_32();                       // start timer once wheel start moving
    }

    totalDistance = ((wheelDistance2 + wheelDistance1) / 2.0);

    // printf("distance 1: %.2f | distance 2: %.2f | total distance: %.f cm\n", wheelDistance1, wheelDistance2, totalDistance);
    // printf("time1: %d | time2: %d | total speed: %.2f cm/s \n", wheelTime1, wheelTime2, totalSpeed);
    irq_clear(PIO0_IRQ_0);
}

bool get_current_speed(struct repeating_timer *t)
{
    // printf("total %.2f\n", totalDistance);
    // printf("temp %.2f\n", tempDistance);
    // printf("current %.2f\n", currentDistance);

    if (tempDistance == 0.0)
    {
        currentDistance = totalDistance;
        tempDistance = totalDistance;
    }
    else
    {
        if (tempDistance == totalDistance)
        {
            currentDistance = 0.0;
        }
        else
        {
            currentDistance = totalDistance - tempDistance;
            tempDistance = totalDistance;
        }
    }
    // get distance every 1 s
    // printf("speed every 1s %.2f\n", currentDistance);
    return true;
}

void init_encoder(){
    printf("ENCODER INIT\n");
    // pin 14 & 17 for wheel 1 and wheel 2
    gpio_set_irq_enabled_with_callback(17, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, wheel_encoder);
    gpio_set_irq_enabled(14, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);

    struct repeating_timer timer;

    add_repeating_timer_ms(1000, get_current_speed, NULL, &timer);

}