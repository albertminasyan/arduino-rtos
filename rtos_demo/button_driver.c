/*
 * button_driver.c
 *
 *  Created on: 24 Sept 2026
 *      Author: student
 */

#include <Arduino.h>
#include "button_driver.h"

static SemaphoreHandle_t button_sem;
static int button_pin;

static void button_isr(void) {
    BaseType_t woken = pdFALSE;
    /* ISR does the minimum possible: just signal the waiting task */
    xSemaphoreGiveFromISR(button_sem, &woken);
//    portYIELD_FROM_ISR(woken);
    if (woken) {
        portYIELD_FROM_ISR();
    }
}

void button_driver_init(int pin, SemaphoreHandle_t sem) {
    button_pin = pin;
    button_sem = sem;
    pinMode(button_pin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(button_pin), button_isr, FALLING);
}
