/*
    RExOS - embedded RTOS
    Copyright (c) 2011-2019, RExOS team
    All rights reserved.

    author: RJ (jam_roma@yahoo.com)
*/

#ifndef APP_ADC_H_
#define APP_ADC_H_

#include "app.h"
#include "rexos/userspace/types.h"
#include "rexos/userspace/pin.h"
#include "rexos/userspace/adc.h"

typedef enum {
    ADC_VOLTAGE_VALUE_NORMAL = 1,
    ADC_VOLTAGE_VALUE_GLITCH,
} ADC_VOLTAGE_VALUE;


void adc_init(APP* app);
void adc_deinit(APP* app);
unsigned int adc_get_mV(APP* app, ADC_VOLTAGE_VALUE avv);
void app_adc_request(APP* app, IPC* ipc);


#endif /* APP_ADC_H_ */
