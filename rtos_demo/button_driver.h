/*
 * button_driver.h
 *
 *  Created on: 24 Sept 2026
 *      Author: Albert Minasyan albert.minasyan@gmail.com
 */

#ifndef BUTTON_DRIVER_H
#define BUTTON_DRIVER_H
#include <Arduino_FreeRTOS.h>
#include <semphr.h>



#ifdef __cplusplus
extern "C" {
#endif

void button_driver_init(int pin, SemaphoreHandle_t sem);

#ifdef __cplusplus
}
#endif



#endif
