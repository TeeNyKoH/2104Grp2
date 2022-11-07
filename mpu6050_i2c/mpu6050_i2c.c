/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#include <stdlib.h>
#include <time.h>

/* Example code to talk to a MPU6050 MEMS accelerometer and gyroscope
   This is taking to simple approach of simply reading registers. It's perfectly
   possible to link up an interrupt line and set things up to read from the
   inbuilt FIFO to make it more useful.
   NOTE: Ensure the device is capable of being driven at 3.3v NOT 5v. The Pico
   GPIO (and therefor I2C) cannot be used at 5v.
   You will need to use a level shifter on the I2C lines if you want to run the
   board at 5v.
   Connections on Raspberry Pi Pico board, other boards may vary.
   GPIO PICO_DEFAULT_I2C_SDA_PIN (On Pico this is GP4 (pin 6)) -> SDA on MPU6050 board
   GPIO PICO_DEFAULT_I2C_SCL_PIN (On Pico this is GP5 (pin 7)) -> SCL on MPU6050 board
   3.3v (pin 36) -> VCC on MPU6050 board
   GND (pin 38)  -> GND on MPU6050 board
*/

// By default these devices  are on bus address 0x68
static int addr = 0x68;

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

int main() {
    stdio_init_all();
#if !defined(i2c_default) || !defined(PICO_DEFAULT_I2C_SDA_PIN) || !defined(PICO_DEFAULT_I2C_SCL_PIN)
    #warning i2c/mpu6050_i2c example requires a board with I2C pins
    puts("Default I2C pins were not defined");
#else
    printf("Hello, MPU6050! Reading raw data from registers...\n");

    // This example will use I2C0 on the default SDA and SCL pins (4, 5 on a Pico)
    i2c_init(i2c_default, 400 * 1000);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
    // Make the I2C pins available to picotool
    bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));

    mpu6050_reset();

    int16_t acceleration[3], gyro[3], temp;
    double up,avg,diffx,diffy,height,newdiff,hypo,currx,curry,orihypo,diff,avgdiff,ahh = 0,totalahh = 0;
    int count = 1;
    int count1 = 0;

    

    sleep_ms(3000);
    double firstvarx = 0,firstvary = 0;
    int i =1;

    for (i = 0; i < 1; ++i)
    {
    mpu6050_read_raw(acceleration, gyro, &temp);
    sleep_ms(3000);
    firstvarx = firstvarx + acceleration[0];
    firstvary = firstvary + acceleration[1];


    }
    while (1) {
        mpu6050_read_raw(acceleration, gyro, &temp);
        // int i ;
        
        // for (i = 1; i < 200; ++i)
        //     {
        //     mpu6050_read_raw(acceleration, gyro, &temp);
        //     avg = avg + acceleration[0];
            
        //     }
        // avg = avg/199;
        // These are the raw numbers from the chip, so will need tweaking to be really useful.
        // See the datasheet for more information
         //printf("Acc. X = %d, Y = %d, Z = %d\n", acceleration[0], acceleration[1], acceleration[2]);
       
        

        currx = acceleration[0];
        curry = acceleration[1];
        diffx = firstvarx - acceleration[0];
        diffy = firstvary - acceleration[1];
        // hypo = sqrt(diffx*diffx + diffy*diffy);
        diff = (diffx+diffy)/2;

        avgdiff = (diffx+diffy)/2;

        // orihypo = sqrt(currx*currx + curry*curry);
        height = avgdiff / 931.63;
        // printf("avg X = %f\n", firstvarx);
        // printf("avg y = %f\n", firstvary);
        // printf("diffx = %f\n", diffx);
        // printf("diffy = %f\n", diffy);
        // printf("avgdiff = %f\n", avgdiff);
        // printf("hypo = %f\n", hypo);
        // printf("orihypo = %f\n", orihypo);
        // printf("DIFF = %f\n", diff);
        
        
        if (height <0){            //to convert height from neg to positive
            // height = height * -1;
            height = 0;
        }
        
        printf("HEIGHT = %f\n", height , "cm");
        
        if (height > 0.7)
        {
            count1 = count1 + 1;
            
            ahh = height + ahh; ///ahh = all height average
            totalahh = ahh/count1;
            printf("%d",count1);
            if (count1>15)
            {
                printf("Hump Detected, oriheight = %f\n", height , "cm" );
                printf("Hump Detected, avgheight = %f\n", totalahh , "cm" );
            }
            
        }
        else
        {
            
            totalahh = 0;
            count1=0;
            ahh=0;
        }
        
        //  printf("Gyro. X = %d, Y = %d, Z = %d\n", gyro[0], gyro[1], gyro[2]);
        // Temperature is simple so use the datasheet calculation to get deg C.
        // Note this is chip temperature.
        // printf("Temp. = %f\n", (temp / 340.0) + 36.53);

        sleep_ms(10);
    }

#endif
    return 0;
}