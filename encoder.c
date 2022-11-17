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
int startTime, stopTime1, stopTime2;
double totalSpeed = 0.0f, totalDistance = 0.0f, rotationDistance = 0.0f;

double get_distance()
{
    return totalDistance;
}

double get_speed()
{
    return totalSpeed;
}
