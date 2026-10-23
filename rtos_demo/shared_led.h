/*
 * shared_led.h
 *
 *  Created on: 24 Sept 2026
 *      Author: Albert Minasyan albert.minasyan@gmail.com
 */

#ifndef SHARED_LED_H
#define SHARED_LED_H
#include <Arduino_FreeRTOS.h>
#include <semphr.h>

#ifdef __cplusplus
extern "C" {
#endif

void shared_led_init(int pin, SemaphoreHandle_t mutex);
void shared_led_pulse(int times);   /* contended resource: multiple tasks may call this */

#ifdef __cplusplus
}
#endif


#endif
