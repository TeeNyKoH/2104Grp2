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
