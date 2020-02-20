/*
 * app_usb.c
 *
 *  Created on: 14 рту. 2018 у.
 *      Author: RLeonov
 */

#include "rexos/userspace/stdio.h"
#include "rexos/userspace/types.h"
#include "rexos/userspace/error.h"
#include "rexos/userspace/usb.h"
#include "app_private.h"
#include "app_usb.h"
#include "config.h"
#include "usb_desc.h"
#include "led.h"

void app_usb_init(APP* app)
{
#if (APP_DEBUG_USB)
    printf("APP USB: init\n");
#endif // APP_DEBUG_USB
    app->usb_started = false;
    //setup usbd
    app->usbd = usbd_create(USB_PORT_NUM, USBD_PROCESS_SIZE, USBD_PROCESS_PRIORITY);

    if (app->usbd == INVALID_HANDLE)
    {
#if (APP_DEBUG_ERRORS)
        printf("USB init failure\n");
#endif
        return;
    }

    ack(app->usbd, HAL_REQ(HAL_USBD, USBD_REGISTER_HANDLER), 0, 0, 0);

    usbd_register_const_descriptor(app->usbd, &__DEVICE_DESCRIPTOR, 0, 0);

    usbd_register_const_descriptor(app->usbd, &__CONFIGURATION_DESCRIPTOR, 0, 0);
    usbd_register_const_descriptor(app->usbd, &__STRING_WLANGS, 0, 0);
    usbd_register_const_descriptor(app->usbd, &__STRING_WLANGS, 0, 0x409);
    usbd_register_const_descriptor(app->usbd, &__STRING_PRODUCT, 2, 0x0409);

    //turn USB on
    ack(app->usbd, HAL_REQ(HAL_USBD, IPC_OPEN), USB_PORT_NUM, 0, 0);
}

void app_usb_deinit(APP* app)
{
    //turn USB off
    ack(app->usbd, HAL_REQ(HAL_USBD, IPC_CLOSE), USB_PORT_NUM, 0, 0);
    app->usb_started = false;
}

static inline void app_usb_enable(APP* app)
{
#if (APP_DEBUG_USB)
    printf("USB start\n");
#endif
    led_on();
}

static inline void app_usb_disable(APP* app)
{
#if (APP_DEBUG_USB)
    printf("USB stop\n");
#endif
    led_off();
}

void app_usb_request(APP* app, IPC* ipc)
{
    switch (HAL_ITEM(ipc->cmd))
    {
    case USBD_ALERT:
        switch (ipc->param1)
        {
        case USBD_ALERT_CONFIGURED:
            app_usb_enable(app);
            break;
        case USBD_ALERT_RESET:
            app_usb_disable(app);
            break;
        }
        break;
    default:
        error(ERROR_NOT_SUPPORTED);
    }
}
