/*
 * sensor_driver.c
 *
 *  Created on: 24 Sept 2026
 *      Author: Albert Minasyan albert.minasyan@gmail.com
 */

#include <Arduino.h>
#include "sensor_driver.h"

static int sensor_pin;

void sensor_driver_init(int pin)
{
    sensor_pin = pin;
}

int sensor_driver_read(void)
{
    return analogRead(sensor_pin);   /* stand-in for a real I2C/SPI peripheral transaction */
}
