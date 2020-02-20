/*
    RExOS - embedded RTOS
    Copyright (c) 2011-2016, Alexey Kramarenko
    All rights reserved.
*/

#ifndef APP_PRIVATE_H
#define APP_PRIVATE_H

#include "app.h"
#include "rexos/userspace/types.h"
#include "rexos/userspace/ipc.h"
#include "device.h"
#include "app_hid.h"
#include "mux.h"

typedef struct _APP {
    DEVICE device;
    HANDLE lcd;
    HANDLE timer;
    HANDLE usbd;
    bool usb_started;
    HID hid;

    // ADC
    HANDLE adc_timer;

    // MUX
    MUX_STATE mux_state;
} APP;

#endif // APP_PRIVATE_H
