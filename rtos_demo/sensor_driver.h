/*
 * sensor_driver.h
 *
 *  Created on: 24 Sept 2026
 *      Author: Albert Minasyan albert.minasyan@gmail.com
 */

#ifndef SENSOR_DRIVER_H
#define SENSOR_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

void sensor_driver_init(int pin);
int  sensor_driver_read(void);   /* blocking-ish HAL call, as a real peripheral read would be */

#ifdef __cplusplus
}
#endif



#endif
