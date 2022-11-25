#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "hardware/uart.h"
#include <string.h>
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#include <stdlib.h>
#include <time.h>
#include "hardware/irq.h"
#include "accelorometer.h"

/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* 
   GPIO PICO_DEFAULT_I2C_SDA_PIN (On Pico this is GP4 (pin 6)) -> SDA on MPU6050 board
   GPIO PICO_DEFAULT_I2C_SCL_PIN (On Pico this is GP5 (pin 7)) -> SCL on MPU6050 board
   3.3v (pin 36) -> VCC on MPU6050 board
   GND (pin 38)  -> GND on MPU6050 board
*/


// By default these devices  are on bus address 0x68

static int addr = 0x68;

double accelo_Diff_X = 0, accelo_Diff_Y = 0, accelo_Height = 0, accelo_new_Diff = 0, accelo_Diff = 0, accelo_Avg_Diff = 0, accelo_All_Height_Average = 0, accelo_Total_All_Height_Average = 0;
double accelo_First_Var_X = 0, accelo_First_Var_Y = 0;
int hump_Status = 0;
int count = 0;

#ifdef i2c_default
static void mpu6050_reset() {
    // Two byte reset. First byte register, second byte data
    // There are a load more options to set up the device in different ways that could be added here
    uint8_t buf[] = {0x6B, 0x00};
    i2c_write_blocking(i2c_default, addr, buf, 2, false);
}

static void mpu6050_read_raw(int16_t accel[3], int16_t gyro[3], int16_t *temp) {
    // For this particular device, we send the device the register we want to read
    // first, then subsequently read from the device. The register is auto incrementing
    // so we don't need to keep sending the register we want, just the first.

    uint8_t buffer[6];

    // Start reading acceleration registers from register 0x3B for 6 bytes
    uint8_t val = 0x3B;
    i2c_write_blocking(i2c_default, addr, &val, 1, true); // true to keep master control of bus
    i2c_read_blocking(i2c_default, addr, buffer, 6, false);

    for (int i = 0; i < 3; i++) {
        accel[i] = (buffer[i * 2] << 8 | buffer[(i * 2) + 1]);
    }

    // Now gyro data from reg 0x43 for 6 bytes
    // The register is auto incrementing on each read
    val = 0x43;
    i2c_write_blocking(i2c_default, addr, &val, 1, true);
    i2c_read_blocking(i2c_default, addr, buffer, 6, false);  // False - finished with bus

    for (int i = 0; i < 3; i++) {
        gyro[i] = (buffer[i * 2] << 8 | buffer[(i * 2) + 1]);;
    }

    // Now temperature from reg 0x41 for 2 bytes
    // The register is auto incrementing on each read
    val = 0x41;
    i2c_write_blocking(i2c_default, addr, &val, 1, true);
    i2c_read_blocking(i2c_default, addr, buffer, 2, false);  // False - finished with bus

    *temp = buffer[0] << 8 | buffer[1];
}
#endif

/* Setup for accelorometer detection */
float getAcelloDectection()
{
    // This example will use I2C0 on the default SDA and SCL pins (4, 5 on a Pico)
    int16_t acceleration[3], gyro[3], temp;
    i2c_init(i2c_default, 400 * 1000);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);

    // Make the I2C pins available to picotool
    bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));
    mpu6050_reset();
    mpu6050_read_raw(acceleration, gyro, &temp);
    busy_wait_ms(3000);
    accelo_First_Var_X = accelo_First_Var_X + acceleration[0];
    accelo_First_Var_Y = accelo_First_Var_Y + acceleration[1];
    
}

/* Setup to calculate humps */
float getHump()  
{
    
    int16_t acceleration[3], gyro[3], temp;
    mpu6050_read_raw(acceleration, gyro, &temp);
    accelo_Diff_X = accelo_First_Var_X - acceleration[0];
    accelo_Diff_Y = accelo_First_Var_Y - acceleration[1];
    accelo_Diff = ( accelo_Diff_X + accelo_Diff_Y ) / 2;
    accelo_Avg_Diff = ( accelo_Diff_X + accelo_Diff_Y ) / 2;
    accelo_Height = accelo_Avg_Diff / 1095.545;

    /* Filtering away all negative values */
    if (accelo_Height < 0){           
        accelo_Height = 0;
    }
    // printf("HEIGHT = %f\n", accelo_Height , "cm");

    /* Only count detected height above 0.7cm as a hump */
    if (accelo_Height > 0.7)
    {
        count = count + 1;
        accelo_All_Height_Average = accelo_Height + accelo_All_Height_Average;
        accelo_Total_All_Height_Average = accelo_All_Height_Average / count;

        /* Samples taken is at a minimum of 3 samples to go over a hump*/
        if (count >= 3)
        {
            uartprintf("Hump Detected, oriheight = %f\n", accelo_Height , "cm" );
            uartprintf("Hump Detected, avgheight = %f\n", accelo_Total_All_Height_Average , "cm" );
            // printf("Hump Detected, oriheight = %f\n", accelo_Height , "cm" );
            // hump_Status = 1;
            // printf("Hump Detected, avgheight = %f\n", accelo_Total_All_Height_Average , "cm" );
        }
        
    }
    else
    {
        /* Resetting all variables to 0 for next hump detection*/
        accelo_Total_All_Height_Average = 0;
        count = 0;
        accelo_All_Height_Average = 0;
        hump_Status = 0;
    }
}
