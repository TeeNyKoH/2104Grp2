#include "pico/stdlib.h"

#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "hardware/irq.h"
#include "hardware/adc.h"

#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdio.h>

#include "barcode.h"
#include "decode.h"
#include "encoder.h"

static void wheel_encoder(unsigned gpio, unsigned int _)
{
    if (gpio == 17)
    {
        wheelRotation1++; // right wheel
        wheelDistance1 = wheelRotation1 * 0.33 * PI;
    }
    if (gpio == 14)
    {
        wheelRotation2++;                            // left wheel
        wheelDistance2 = wheelRotation2 * 0.33 * PI; // d = 6.6cm, distance = notches * 6.6pi / 20
    }

    if (wheelRotation1 == 1 && wheelRotation2 == 1)
    {
        startTime = time_us_32(); // start timer once wheel start moving
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

bool get_total_average_speed(struct repeating_timer *t)
{
    double temp = get_average_speed(); 
    // get total average speed
    // printf("total average speed %.2f\n", temp);

    return true;
}

int main()
{
    stdio_init_all();
    sleep_ms(1000);

    // pin 14 & 17 for wheel 1 and wheel 2
    gpio_set_irq_enabled_with_callback(17, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &wheel_encoder);
    gpio_set_irq_enabled(14, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);

    struct repeating_timer timer;
    struct repeating_timer timer1;

    add_repeating_timer_ms(1000, get_current_speed, NULL, &timer);
    add_repeating_timer_ms(10001, get_total_average_speed, NULL, &timer1);

    adc_init();
    // Make sure GPIO is high-impedance, no pullups etc
    adc_gpio_init(26);

    // Select ADC input 0 (GPIO26)
    adc_select_input(0);

    adc_fifo_setup(
        true,  // Write each completed conversion to the sample FIFO
        false, // Enable DMA data request (DREQ)
        1,     // DREQ (and IRQ) asserted when at least 1 sample present
        false, // We won't see the ERR bit because of 8 bit reads; disable.
        false  // Shift each sample to 8 bits when pushing to FIFO
    );
    adc_set_clkdiv(0);
    adc_irq_set_enabled(true);

    irq_clear(ADC_IRQ_FIFO);
    irq_set_exclusive_handler(ADC_IRQ_FIFO, barcode_interrupt);
    irq_set_enabled(ADC_IRQ_FIFO, true);

    adc_run(true);

    while (1)
    {
    }
}