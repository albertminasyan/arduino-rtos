/*
 * shared_led.c
 *
 *  Created on: 24 Sept 2026
 *      Author: Albert Minasyan albert.minasyan@gmail.com
 */

#include <Arduino.h>
#include "shared_led.h"

static int led_pin;
static SemaphoreHandle_t led_mutex;

void shared_led_init(int pin, SemaphoreHandle_t mutex)
{
    led_pin = pin;
    led_mutex = mutex;
    pinMode(led_pin, OUTPUT);
}

void shared_led_pulse(int times)
{
    /* mutex, not a plain semaphore: this is ownership of a resource,
       and FreeRTOS mutexes support priority inheritance to avoid
       priority inversion if a low-priority task holds it */
    if (xSemaphoreTake(led_mutex, pdMS_TO_TICKS(100)) == pdTRUE)
    {
        for (int i = 0; i < times; i++)
        {
            digitalWrite(led_pin, HIGH);
            vTaskDelay(pdMS_TO_TICKS(80));
            digitalWrite(led_pin, LOW);
            vTaskDelay(pdMS_TO_TICKS(80));
        }
        xSemaphoreGive(led_mutex);
    }
    /* else: could not acquire within timeout — real code would log/count this */
}
