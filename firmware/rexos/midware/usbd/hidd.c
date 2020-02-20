/*
 * hidd.c
 *
 *  Created on: 14 рту. 2018 у.
 *      Author: RLeonov
 */

#include <string.h>
#include "../../userspace/stdlib.h"
#include "../../userspace/stdio.h"
#include "../../userspace/hid.h"
#include "../../userspace/usb.h"
#include "../../userspace/error.h"
#include "hidd.h"
#include "../../../sys_config.h"

static const uint8_t __HID_CONTROL_REPORT[HID_CONTROL_REPORT_SIZE] =
{
        HID_USAGE_PAGE_GENERIC_DEVICE_CONTROLS,
        WBVAL( DFU_USAGE_PAGE ), // UsagePageVendor
        HID_Usage ( DFU_USAGE ),
        HID_Collection ( HID_COLLECTION_APPLICATION ),
        HID_Usage ( DFU_USAGE_DATA_IN ),
        HID_LogicalMin ( 0 ),
        HID_LogicalMaxS ( 0xff ),
        HID_ReportSize ( 8 ),
        HID_ReportCount ( DFU_REPORT_COUNT ),
        HID_Input ( HID_DATA | HID_ABSOLUTE | HID_VARIABLE ),
        HID_Usage ( DFU_USAGE_DATA_OUT ),
        HID_LogicalMin ( 0 ),
        HID_LogicalMaxS ( 0xff ),
        HID_ReportSize ( 8 ),
        HID_ReportCount ( DFU_REPORT_COUNT ),
        HID_Output ( HID_DATA | HID_ABSOLUTE | HID_VARIABLE ),
        HID_END_COLLECTION
};

typedef struct {
    IO* io;
    uint8_t data_ep, ep_size, iface;
    uint8_t idle;
    uint8_t suspended, boot_protocol;
} HIDD;

static void hidd_destroy(HIDD* hidd)
{
    io_destroy(hidd->io);
    free(hidd);
}

void hidd_class_configured(USBD* usbd, USB_CONFIGURATION_DESCRIPTOR* cfg)
{
    USB_INTERFACE_DESCRIPTOR* iface;
    USB_ENDPOINT_DESCRIPTOR* ep;
    uint8_t in_ep, in_ep_size, hid_u2f_iface;
    in_ep = in_ep_size = hid_u2f_iface = 0;

    //check control/data ep here
    for (iface = usb_get_first_interface(cfg); iface != NULL; iface = usb_get_next_interface(cfg, iface))
    {
        if (iface->bInterfaceClass == HID_INTERFACE_CLASS && iface->bInterfaceSubClass == HID_SUBCLASS_NO_SUBCLASS &&
            iface->bInterfaceProtocol == HID_PROTOCOL_NONE)
        {
            ep = (USB_ENDPOINT_DESCRIPTOR*)usb_interface_get_first_descriptor(cfg, iface, USB_ENDPOINT_DESCRIPTOR_TYPE);
            if (ep != NULL)
            {
                in_ep = USB_EP_NUM(ep->bEndpointAddress);
                in_ep_size = ep->wMaxPacketSize;
                hid_u2f_iface = iface->bInterfaceNumber;
                break;
            }
        }
    }

    //No HID descriptors in interface
    if (in_ep == 0)
    {
        printf("1\n");
        return;
    }
    HIDD* hidd = (HIDD*)malloc(sizeof(HIDD));
    if (hidd == NULL)
    {
#if (USBD_HID_DEBUG_ERRORS)
        printf("USB HIDD: malloc error\n");
#endif // USBD_HID_DEBUG_ERRORS
        return;
    }
    hidd->io = io_create(in_ep_size);

    if (hidd->io == NULL)
    {
#if (USBD_HID_DEBUG_ERRORS)
        printf("USB HIDD: io create error\n");
#endif // USBD_HID_DEBUG_ERRORS
        hidd_destroy(hidd);
        return;
    }
    hidd->iface = hid_u2f_iface;
    hidd->data_ep = in_ep;
    hidd->idle = 0;

#if (USBD_HID_DEBUG_REQUESTS)
    printf("Found USB HID device class, EP%d, iface: %d\n", hidd->data_ep, hidd->iface);
#endif //USBD_HID_DEBUG_REQUESTS

    usbd_register_interface(usbd, hidd->iface, &__HIDD_CLASS, hidd);
    usbd_register_endpoint(usbd, hidd->iface, hidd->data_ep);
    usbd_usb_ep_open(usbd, hidd->data_ep, USB_EP_TYPE_INTERRUPT, in_ep_size);
    usbd_usb_ep_open(usbd, USB_EP_IN | hidd->data_ep, USB_EP_TYPE_INTERRUPT, in_ep_size);
    io_reset(hidd->io);
    usbd_usb_ep_read(usbd, hidd->data_ep, hidd->io, io_get_free(hidd->io));
}

void hidd_class_reset(USBD* usbd, void* param)
{
#if (USBD_HID_DEBUG_REQUESTS)
    printf("HIDD: reset\n");
#endif // USBD_HID_DEBUG_REQUESTS
    HIDD* hidd = (HIDD*)param;
    usbd_usb_ep_close(usbd, hidd->data_ep);
    usbd_usb_ep_close(usbd, USB_EP_IN | hidd->data_ep);
    usbd_unregister_endpoint(usbd, hidd->iface, hidd->data_ep);
    usbd_unregister_interface(usbd, hidd->iface, &__HIDD_CLASS);
    hidd_destroy(hidd);
}

void hidd_class_suspend(USBD* usbd, void* param)
{
#if (USBD_HID_DEBUG_REQUESTS)
    printf("HIDD: suspend\n");
#endif // USBD_HID_DEBUG_REQUESTS
//    HIDD* hidd = (HIDD*)param;
//    usbd_usb_ep_flush(usbd, hidd->data_ep);
//    usbd_usb_ep_flush(usbd, USB_EP_IN | hidd->data_ep);
//    hidd->suspended = true;
}

void hidd_class_resume(USBD* usbd, void* param)
{
#if (USBD_HID_DEBUG_REQUESTS)
    printf("HIDD: resume\n");
#endif // USBD_HID_DEBUG_REQUESTS
//    HIDD* hidd = (HIDD*)param;
//    hidd->suspended = false;
//    io_reset(hidd->io);
//    usbd_usb_ep_read(usbd, hidd->data_ep, hidd->io, io_get_free(hidd->io));
}

static inline int hidd_get_descriptor(HIDD* hidd, unsigned int value, unsigned int index, IO* io)
{
    int res = -1;
    switch (value)
    {
    case HID_REPORT_DESCRIPTOR_TYPE:
#if (USBD_HID_DEBUG_REQUESTS)
        printf("HIDD: get REPORT DESCRIPTOR\n");
#endif // USBD_HID_DEBUG_REQUESTS
        io_data_write(io, &__HID_CONTROL_REPORT, HID_CONTROL_REPORT_SIZE);
        res = HID_CONTROL_REPORT_SIZE;
        break;
    }
    return res;
}

static inline int hidd_get_report(USBD* usbd, HIDD* hidd, unsigned int type, IO* io)
{
    int res = -1;
    uint8_t* report = io_data(io);

    switch (type)
    {
    case HID_REPORT_TYPE_INPUT:
#if (USBD_HID_DEBUG_REQUESTS)
        printf("HIDD: get INPUT report\n");
#endif // USBD_HID_DEBUG_REQUESTS
        memset(report, 0x00, 64);
        res = 64;
        break;
    case HID_REPORT_TYPE_OUTPUT:
#if (USBD_HID_DEBUG_REQUESTS)
        printf("HIDD: get OUTPUT report\n");
#endif // USBD_HID_DEBUG_REQUESTS
        res = 64;
        break;
    }
    return res;
}

static inline int hidd_set_report(USBD* usbd, HIDD* hidd, IO* io, unsigned int length)
{
#if (USBD_HID_DEBUG_REQUESTS)
    printf("HIDD: set report,  len=%u\n", length);
#endif // USBD_HID_DEBUG_REQUESTS
    return 0;
}

static inline int hidd_get_idle(HIDD* hidd, IO* io)
{
#if (USBD_HID_DEBUG_REQUESTS)
    printf("HIDD U2F: get idle\n");
#endif // USBD_HID_DEBUG_REQUESTS

    uint8_t* report = io_data(io);
    report[0] = hidd->idle;
    return 1;
}

static inline int hidd_set_idle(HIDD* hidd, unsigned int value)
{
#if (USBD_HID_DEBUG_REQUESTS)
    printf("HIDD: set idle %dms\n", value << 2);
#endif // USBD_HID_DEBUG_REQUESTS
    //no IDLE report is supported
    return 0;
}

static inline int hidd_get_protocol(HIDD* hidd, IO* io)
{
#if (USBD_HID_DEBUG_REQUESTS)
    printf("HIDD: get protocol\n");
#endif // USBD_HID_DEBUG_REQUESTS
    //no protocol is supported
    return 1;
}

static inline int hidd_set_protocol(HIDD* hidd, unsigned int protocol, IO* io)
{
#if (USBD_HID_DEBUG_REQUESTS)
    printf("HIDD U2F: set protocol: %d\n", protocol & 1);
#endif // USBD_HID_DEBUG_REQUESTS
    return 0;
}

int hidd_class_setup(USBD* usbd, void* param, SETUP* setup, IO* io)
{
    HIDD* hidd = (HIDD*)param;
    unsigned int res = -1;
    switch (setup->bRequest)
    {
    case USB_REQUEST_GET_DESCRIPTOR:
        res = hidd_get_descriptor(hidd, setup->wValue >> 8, setup->wValue & 0xff, io);
        break;
    case HID_GET_REPORT:
        res = hidd_get_report(usbd, hidd, setup->wValue >> 8, io);
        break;
    case HID_GET_IDLE:
        res = hidd_get_idle(hidd, io);
        break;
    case HID_SET_REPORT:
        res = hidd_set_report(usbd, hidd, io, setup->wLength);
        break;
    case HID_SET_IDLE:
        res = hidd_set_idle(hidd, setup->wValue >> 8);
        break;
    case HID_GET_PROTOCOL:
        res = hidd_get_protocol(hidd, io);
        break;
    case HID_SET_PROTOCOL:
        res = hidd_set_protocol(hidd, setup->wValue, io);
        break;
    }
    return res;
}

static inline bool hidd_driver_event(USBD* usbd, HIDD* hidd, IPC* ipc)
{
    bool need_post = false;
    IO* io = (IO*)ipc->param2;
    switch (HAL_ITEM(ipc->cmd))
    {
    case IPC_WRITE:
        usbd_post_user(usbd, hidd->iface, 0, HAL_CMD(HAL_USBD_IFACE, USB_HID_IN), 0, 0);
        break;
    case IPC_READ:
        usbd_post_user(usbd, hidd->iface, 0, HAL_CMD(HAL_USBD_IFACE, USB_HID_OUT), (unsigned int)io, io->data_size);
        break;
    default:
        error(ERROR_NOT_SUPPORTED);
        need_post = true;
    }
    return need_post;
}

static inline void hidd_send_report(USBD* usbd, HIDD* hidd)
{
#if (USBD_HID_DEBUG_REQUESTS)
    printf("HIDD: send report %u\n", hidd->io->data_size);
#endif // USBD_HID_DEBUG_REQUESTS
    usbd_usb_ep_flush(usbd, hidd->data_ep);
    usbd_usb_ep_write(usbd, hidd->data_ep, hidd->io);
}

void hidd_class_request(USBD* usbd, void* param, IPC* ipc)
{
    HIDD* hidd = (HIDD*)param;

    if (HAL_GROUP(ipc->cmd) == HAL_USB)
       hidd_driver_event(usbd, hidd, ipc);
    else
        switch (HAL_ITEM(ipc->cmd))
        {
            case USB_HID_IN:
                hidd_send_report(usbd, hidd);
                break;

            case USB_HID_OUT:
                io_reset(hidd->io);
                usbd_usb_ep_read(usbd, hidd->data_ep, hidd->io, io_get_free(hidd->io));
                break;

            default:
                error(ERROR_NOT_SUPPORTED);
        }
}

const USBD_CLASS __HIDD_CLASS = {
    hidd_class_configured,
    hidd_class_reset,
    hidd_class_suspend,
    hidd_class_resume,
    hidd_class_setup,
    hidd_class_request,
};

