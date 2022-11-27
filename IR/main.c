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

int main()
{
    stdio_init_all();
    sleep_ms(1000);

    // printf("BARCODE INIT\n");
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
    irq_set_exclusive_handler(ADC_IRQ_FIFO, barcode_interrupt_raw);
    // irq_set_exclusive_handler(ADC_IRQ_FIFO, barcode_interrupt_exponential_weighted_filter);
    irq_set_enabled(ADC_IRQ_FIFO, true);

    adc_run(true);

    // init_encoder();
    
    while (1)
    {
    }
}