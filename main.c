/*
 * -------------------------------------------
 *    MSP432 DriverLib - v3_21_00_05
 * -------------------------------------------
 *
 * --COPYRIGHT--,BSD,BSD
 * Copyright (c) 2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
/*******************************************************************************
 * MSP432 ADC14 - Single Channel Sample Repeat
 *
 * Description: This code example will demonstrate the basic functionality of
 * of the DriverLib ADC APIs with sampling a single channel repeatedly. Each
 * time the ADC conversion occurs, the result is stored into a local variable.
 * The sample timer is used to continuously grab a new value from the ADC
 * module using a manual iteration that is performed in the ADC ISR. A
 * normalized ADC value with respect to the 3.3v Avcc is also calculated using
 * the FPU.
 *
 *                MSP432P401
 *             ------------------
 *         /|\|                  |
 *          | |                  |
 *          --|RST         P5.5  |<--- A0 (Analog Input)
 *            |                  |
 *            |                  |
 *            |                  |
 *            |                  |
 *            |                  |
 *
 * Author: Timothy Logan
 ******************************************************************************/
/* DriverLib Includes */
#include "driverlib.h"

/* Standard Includes */
#include <stdint.h>

#include <stdbool.h>

/* Statics */
static volatile uint16_t curADCResult;
static volatile uint16_t whiteLimit;
static volatile uint16_t blackLimit;
static volatile uint16_t median;

static volatile uint16_t WMA = 0;
static uint16_t i;
const uint16_t n = 10;
static volatile uint16_t counter = 0;
static volatile uint16_t thicknessCounter = 0;
static volatile uint16_t thinCounter = 0;
static volatile uint16_t thickCounter = 0;
uint16_t arr[n];
static uint16_t thickDone = 0;
static uint16_t thinDone = 0;
static uint16_t isBlack = 1;

static volatile float normalizedADCRes;

void test(void);

int main(void)
{
    /* Halting the Watchdog  */
    MAP_WDT_A_holdTimer();

    /* Initializing Variables */
    curADCResult = 0;

    /* Setting Flash wait state */
    MAP_FlashCtl_setWaitState(FLASH_BANK0, 2);
    MAP_FlashCtl_setWaitState(FLASH_BANK1, 2);

    /* Setting DCO to 48MHz  */
    MAP_PCM_setPowerState(PCM_AM_LDO_VCORE1);
    MAP_CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_48);

    /* Enabling the FPU for floating point operation */
    MAP_FPU_enableModule();
    MAP_FPU_enableLazyStacking();

    /* Initializing ADC (MCLK/1/4) */
    MAP_ADC14_enableModule();
    MAP_ADC14_initModule(ADC_CLOCKSOURCE_SMCLK, ADC_PREDIVIDER_4, ADC_DIVIDER_3, 0);

    /* Configuring P1.0 as output */
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);

    /* Configuring GPIOs (5.5 A0) */
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P5, GPIO_PIN5, GPIO_TERTIARY_MODULE_FUNCTION);

    /* Configuring ADC Memory */
    MAP_ADC14_configureSingleSampleMode(ADC_MEM0, true);
    MAP_ADC14_configureConversionMemory(ADC_MEM0, ADC_VREFPOS_AVCC_VREFNEG_VSS,
                                        ADC_INPUT_A0, false);

    /* Configuring Sample Timer */
    MAP_ADC14_enableSampleTimer(ADC_MANUAL_ITERATION);

    /* Enabling/Toggling Conversion */
    MAP_ADC14_enableConversion();
    MAP_ADC14_toggleConversionTrigger();

    /* Enabling interrupts */
    MAP_ADC14_enableInterrupt(ADC_INT0);
    MAP_Interrupt_enableInterrupt(INT_ADC14);
    MAP_Interrupt_enableMaster();

    /* Michael's Code */
    for (i = 0; i < n; i++)
    {
        arr[i] = 0;
    }

    while (1)
    {
        MAP_PCM_gotoLPM0();
        //
        //        printf("%d\n", counter);
        //        if(counter == 9){
        //            test();
        //        }
    }
}

// void test(void){
//         for (i = 0; i < n; i++ ) {
//                WMA = WMA + (arr[i] * i);
//                WMA = WMA / ((i*(i+1))/2);
//             }
//
//             // set black bg threshold
//             if (WMA >= blackLimit){
//                 blackLimit = WMA;
//                 // init of white threshold
//                 if(whiteLimit == 0){
//                     whiteLimit = blackLimit;
//                 }
//             }
//
//             // set white bg threshold
//             if (WMA <= whiteLimit){
//                 whiteLimit = WMA;
//             }
//             printf("%d\n",WMA);
//             printf("%d\n", WMA);
//  if WMA goes near threshold, consider 1 or 0
//    if (WMA < whiteLimit * 1.2){
//        printf("0\n");
//    }
//
//    if (WMA > blackLimit * 0.8){
//        printf("1\n");
//    }

//        printf("%d\n",blackLimit);
//        printf("%d\n\n",whiteLimit);
//        printf("%d\n",WMA);

//}

/* ADC Interrupt Handler. This handler is called whenever there is a conversion that is finished for ADC_MEM0. */
void ADC14_IRQHandler(void)
{
    uint64_t status = MAP_ADC14_getEnabledInterruptStatus();
    MAP_ADC14_clearInterruptFlag(status);

    if (ADC_INT0 & status)
    {
        curADCResult = MAP_ADC14_getResult(ADC_MEM0);

        arr[counter] = curADCResult;
        counter = counter + 1;

        /* If white is detected, turn on P1.0 */
        if (curADCResult < 1000)
        {
            GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
        }
        else
        {
            GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
        }

        if (counter == n)
        {
            for (i = 0; i < n; i++)
            {
                WMA = WMA + (arr[i] * i);
                WMA = WMA / ((i * (i + 1)) / 2);
            }

            // set black bg threshold
            if (WMA >= blackLimit)
            {
                blackLimit = WMA;
                // init of white threshold
                if (whiteLimit == 0)
                {
                    whiteLimit = blackLimit;
                }
            }

            // set white bg threshold
            if (WMA <= whiteLimit)
            {
                whiteLimit = WMA;
            }

            if (whiteLimit != blackLimit)
            {
                thicknessCounter++;

                if (WMA > blackLimit * 0.8)
                {

                    if (thicknessCounter == thickCounter & !isBlack)
                    {
                        // output 1 into black string
                        thicknessCounter = 0;
                    }
                    else if (thicknessCounter == thinCounter & !isBlack)
                    {
                        // output 0 into black string
                        thicknessCounter = 0;
                    }

                    if (!thinDone)
                    {
                        thinCounter++;
                    }
                    else
                    {
                        thickDone = 1;
                    }
                }

                if (WMA < whiteLimit * 1.2)
                {

                    if (thicknessCounter == thickCounter & isBlack)
                    {
                        // output 1 into white string
                        thicknessCounter = 0;
                    }
                    else if (thicknessCounter == thinCounter !isBlack)
                    {
                        // output 0 into white string
                        thicknessCounter = 0;
                    }

                    if (!thickDone)
                    {
                        thickCounter++;
                    }
                    else
                    {
                        thinDone = 1;
                    }
                }
            }
        }
        normalizedADCRes = (curADCResult * 3.3) / 16384;
        //        printf("%d\n",curADCResult);

        MAP_ADC14_toggleConversionTrigger();
    }
}
