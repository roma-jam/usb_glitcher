/*
 * app_usb.h
 *
 *  Created on: 14 рту. 2018 у.
 *      Author: RLeonov
 */

#ifndef APP_USB_H_
#define APP_USB_H_

#include "app.h"
#include "rexos/userspace/ipc.h"

void app_usb_init(APP* app);
void app_usb_deinit(APP* app);
void app_usb_request(APP* app, IPC* ipc);

#endif /* APP_USB_H_ */
