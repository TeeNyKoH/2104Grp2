#include "pico/stdlib.h"
#include "pico/binary_info.h"

#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "hardware/uart.h"
#include "hardware/i2c.h"
#include "hardware/irq.h"
#include "hardware/adc.h"

#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdio.h>

#include "main.h"
#include "ultrasonic.h"
#include "accelorometer.h"
#include "barcode.h"
#include "decode.h"
#include "encoder.h"


int trigPin = 0;
int echoPin1 = 1;
int echoPin2 = 15;
int echoPin3 = 16;

static void wheel_encoder(unsigned gpio, unsigned int _) {
    if (gpio == 17) {
        wheelRotation1++; // right wheel
        wheelDistance1 = wheelRotation1 * 3.3 * PI;

    }
    if (gpio == 14) {
        wheelRotation2++; // left wheel
        wheelDistance2 = wheelRotation2 * 3.3 * PI; // d = 6.6cm, distance = notches * 6.6pi / 20
    }

    if (wheelRotation1 == 1 && wheelRotation2 == 1) {
        startTime = time_us_32();               // start timer once wheel start moving 
    }

    if (wheelRotation1 % 20 == 0) {
        stopTime1 = time_us_32();
        wheelTime1 = (stopTime1 - startTime) / 1000000;     // time for 1 rotation in seconds

        done1 = 1;
    }
    if (wheelRotation2 % 20 == 0) {
        stopTime2 = time_us_32();
        wheelTime2 = (stopTime2 - startTime) / 1000000;     // time for 1 rotation in seconds

        done2 = 1;
    }

    if (done1 && done2) {

        totalDistance = ((wheelDistance2 + wheelDistance1) / 2.0);
        rotationDistance = ((wheelDistance2 + wheelDistance1) / 2.0);
        
        totalSpeed = (rotationDistance / ((wheelTime1 + wheelTime2) / 2.0));

        // printf("distance 1: %.2f | distance 2: %.2f | total distance: %.f cm\n", wheelDistance1, wheelDistance2, totalDistance);
        // printf("time1: %d | time2: %d | total speed: %.2f cm/s \n", wheelTime1, wheelTime2, totalSpeed);
        rotationDistance = 0.0f;
        done1 = 0;
        done2 = 0;
    }
    irq_clear(PIO0_IRQ_0);
}

bool repeating_timer_callback(struct repeating_timer *t)
{
    getUSDectection(trigPin, echoPin1, echoPin2, echoPin3);
    getHump();
    return true;
}

int main(){
    stdio_init_all();
    sleep_ms(1000);

    gpio_set_irq_enabled_with_callback(17, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &wheel_encoder);
    gpio_set_irq_enabled(14, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);    

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

    // need to check if i2c is defined
#if !defined(i2c_default) || !defined(PICO_DEFAULT_I2C_SDA_PIN) || !defined(PICO_DEFAULT_I2C_SCL_PIN)
    #warning i2c/mpu6050_i2c example requires a board with I2C pins
    puts("Default I2C pins were not defined");
#else
    sleep_ms(1000);
    setupUltrasonicPins(trigPin, echoPin1); //front
    setupUltrasonicPins(trigPin, echoPin2); //left
    setupUltrasonicPins(trigPin, echoPin3); //right
    struct repeating_timer timer;
    getAcelloDectection();
    
    add_repeating_timer_ms(50, repeating_timer_callback, NULL, &timer);
    while (1) {
        //tight_loop_contents();
        sleep_ms(40);
    }
     
    #endif
}