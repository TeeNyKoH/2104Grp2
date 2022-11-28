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

static void wheel_encoder(unsigned gpio,long unsigned int _)
{
    if (gpio == 17)
    {
        wheelRotation1++; // right wheel
        wheelDistance1 = wheelRotation1 * 0.165 * PI;
    }
    if (gpio == 14)
    {
        wheelRotation2++;                            // left wheel
        wheelDistance2 = wheelRotation2 * 0.165 * PI; // d = 6.6cm, distance = notches * 6.6pi / 40
    }

    totalDistance = ((wheelDistance2 + wheelDistance1) / 2.0);

    // printf("distance 1: %.2f | distance 2: %.2f | total distance: %.f cm\n", wheelDistance1, wheelDistance2, totalDistance);
    // printf("time1: %d | time2: %d | total speed: %.2f cm/s \n", wheelTime1, wheelTime2, totalSpeed);
    irq_clear(PIO0_IRQ_0);
}

bool get_current_speed(struct repeating_timer *t)
{

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
    printf("total distance: %.f cm\n", totalDistance);
    printf("speed per second: %.2fcm/s \n", currentDistance);

    return true;
}

int main()
{
    stdio_init_all();
    sleep_ms(1000);

    printf("BARCODE INIT\n");
    adc_init();
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
    // irq_set_exclusive_handler(ADC_IRQ_FIFO, barcode_interrupt_moving_average);
    // irq_set_exclusive_handler(ADC_IRQ_FIFO, barcode_interrupt_raw);
    irq_set_exclusive_handler(ADC_IRQ_FIFO, barcode_interrupt_exponential_weighted_filter);
    irq_set_enabled(ADC_IRQ_FIFO, true);

    adc_run(true);

    printf("ENCODER INIT\n");
    // pin 14 & 17 for wheel 1 and wheel 2
    gpio_set_irq_enabled_with_callback(17, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, wheel_encoder);
    gpio_set_irq_enabled(14, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);

    struct repeating_timer timer;
    add_repeating_timer_ms(1000, get_current_speed, NULL, &timer);
    
    while (1)
    {
    }
}

// Code is done by
// Gan Jia Xin Mandy       2101882
// Michael Ronny Chandiary 2101491
// Tan Ai Xin Hazel        2102468
