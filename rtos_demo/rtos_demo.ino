#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <semphr.h>
#include "button_driver.h"
#include "sensor_driver.h"
#include "shared_led.h"

typedef struct {
    const char *source;
    int value;
} log_msg_t;

static QueueHandle_t   log_queue;
static SemaphoreHandle_t button_sem;
static SemaphoreHandle_t led_mutex;

/* High priority: reacts to the "urgent hardware event" */
void button_task(void *pv) {
//	Serial.print(__FILE__);
//	Serial.print(":");
//	Serial.print(__LINE__);
//	Serial.print(" ");
//	Serial.println(__FUNCTION__);

    log_msg_t msg;
    for (;;) {
        if (xSemaphoreTake(button_sem, portMAX_DELAY) == pdTRUE) {
            msg.source = "BUTTON";
            msg.value  = 1;
            xQueueSend(log_queue, &msg, 0);
            shared_led_pulse(3);   /* fast triple-pulse for urgent event */
        }
    }
}

/* Medium priority: periodic "peripheral" polling */
void sensor_task(void *pv) {

//	Serial.print(__FILE__);
//	Serial.print(":");
//	Serial.print(__LINE__);
//	Serial.print(" ");
//	Serial.println(__FUNCTION__);

    log_msg_t msg;
    for (;;) {
        msg.source = "SENSOR";
        msg.value  = sensor_driver_read();
        xQueueSend(log_queue, &msg, pdMS_TO_TICKS(50));
        shared_led_pulse(1);   /* single pulse per sample */
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

/* Low priority: drains the queue, simulates a shared bus/UART consumer */
void logger_task(void *pv) {

//	Serial.print(__FILE__);
//	Serial.print(":");
//	Serial.print(__LINE__);
//	Serial.print(" ");
//	Serial.println(__FUNCTION__);

    log_msg_t msg;
    for (;;) {
        if (xQueueReceive(log_queue, &msg, portMAX_DELAY) == pdTRUE) {
            Serial.print(msg.source);
            Serial.print(": ");
            Serial.println(msg.value);
        }
    }
}

void setup() {
    Serial.begin(9600);

//	Serial.print(__FILE__);
//	Serial.print(":");
//	Serial.print(__LINE__);
//	Serial.print(" ");
//	Serial.println(__FUNCTION__);

    log_queue  = xQueueCreate(8, sizeof(log_msg_t));
    button_sem = xSemaphoreCreateBinary();
    led_mutex  = xSemaphoreCreateMutex();

    button_driver_init(2, button_sem);
    sensor_driver_init(A0);
    shared_led_init(8, led_mutex);

//	Serial.print(__FILE__);
//	Serial.print(":");
//	Serial.print(__LINE__);
//	Serial.print(" ");
//	Serial.println(__FUNCTION__);

    /* priorities: button (urgent ISR-driven) > sensor (periodic) > logger (bulk/background) */
    xTaskCreate(button_task, "btn", 128, NULL, 3, NULL);
    xTaskCreate(sensor_task, "sns", 128, NULL, 2, NULL);
    xTaskCreate(logger_task, "log", 128, NULL, 1, NULL);

//    Serial.print(__FILE__);
//	Serial.print(":");
//	Serial.print(__LINE__);
//	Serial.print(" ");
//	Serial.println(__FUNCTION__);
}

void loop() { /* unused — scheduler owns execution */ }
