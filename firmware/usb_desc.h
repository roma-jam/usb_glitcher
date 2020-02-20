/*
 * usb_desc.h
 *
 *  Created on: 23 мая 2017 г.
 *      Author: RLeonov
 */

#ifndef USB_DESC_H_
#define USB_DESC_H_


#include "rexos/userspace/usb.h"
#include "rexos/userspace/hid.h"
#include "rexos/userspace/ccid.h"
#include "config.h"
#include "sys_config.h"

#pragma pack(push, 1)

const USB_DEVICE_DESCRIPTOR __DEVICE_DESCRIPTOR = {
    sizeof(USB_DEVICE_DESCRIPTOR),                                            /*bLength */
    USB_DEVICE_DESCRIPTOR_TYPE,                                               /*bDescriptorType*/
    0x0110,                                                                   /*bcdUSB */
    0x00,                                                                     /*bDeviceClass*/
    0x00,                                                                     /*bDeviceSubClass*/
    0x00,                                                                     /*bDeviceProtocol*/
    64,                                                                       /*bMaxPacketSize*/
    0x24DC,                                                                   /*idVendor: temporary*/
    0x0FFE,                                                                   /*idProduct*/
    0x0100,                                                                   /*bcdDevice release*/
    0,                                                                        /*Index of manufacturer string*/
    2,                                                                        /*Index of product string*/
    0,                                                                        /*Index of serial number string*/
    1                                                                         /*bNumConfigurations*/
};

typedef struct {
    USB_CONFIGURATION_DESCRIPTOR configuration;
    //HID
    USB_INTERFACE_DESCRIPTOR hid_interface;
    HID_DESCRIPTOR hid_descriptor;
    USB_ENDPOINT_DESCRIPTOR hid_endpoint[2];
} CONFIGURATION;

const CONFIGURATION __CONFIGURATION_DESCRIPTOR = {
    //CONFIGURATION descriptor
    {
        sizeof(USB_CONFIGURATION_DESCRIPTOR),                               /*bLength*/
        USB_CONFIGURATION_DESCRIPTOR_TYPE,                                  /*bDescriptorType*/
        sizeof(CONFIGURATION),                                              /*wTotalLength*/
        1,                                                                  /*bNumInterfaces*/
        1,                                                                  /*bConfigurationValue*/
        0,                                                                  /*iConfiguration*/
        0x80,                                                               /*bmAttributes*/
        50                                                                  /*bMaxPower*/
    },
    //HID INTERFACE descriptor
    {
        sizeof(USB_INTERFACE_DESCRIPTOR),                                      /*bLength*/
        USB_INTERFACE_DESCRIPTOR_TYPE,                                     /*bDescriptorType*/
        0,                                                                  /*bInterfaceNumber*/
        0,                                                                  /*bAlternateSetting*/
        2,                                                                  /*bNumEndpoints*/
        HID_INTERFACE_CLASS,                                                /*bInterfaceClass*/
        HID_SUBCLASS_NO_SUBCLASS,                                        /*bInterfaceSubClass*/
        HID_PROTOCOL_NONE,                                              /*bInterfaceProtocol*/
        0x00                                                                   /*iInterface*/
    },
    //U2F descriptor
    {
        sizeof(HID_DESCRIPTOR),                                             /* Size of this descriptor in bytes */
        HID_DESCRIPTOR_TYPE,                                                /* HID descriptor type (assigned by USB) */
        0x0100,                                                              /* HID Class Specification release number */
        HID_COUNTRY_NOT_SUPPORTED,                                          /* Hardware target country */
        0x00,                                                               /* Number of HID class descriptors to follow */
        HID_REPORT_DESCRIPTOR_TYPE,                                         /* Report descriptor type */
        HID_CONTROL_REPORT_SIZE                                               /* Total length of Report descriptor */
    },
    //U2F endpoint
    {
        { // IN
            sizeof(USB_ENDPOINT_DESCRIPTOR),                                       /*bLength*/
            USB_ENDPOINT_DESCRIPTOR_TYPE,                                      /*bDescriptorType*/
            0x80 | 0x01,                                                         /*bEndpointAddress*/
            USB_EP_BM_ATTRIBUTES_INTERRUPT,                                     /*bmAttributes*/
            64,                                                                 /*wMaxPacketSize*/
            0x01                                                                /*bInterval*/
        },
        { // OUT
            sizeof(USB_ENDPOINT_DESCRIPTOR),                                       /*bLength*/
            USB_ENDPOINT_DESCRIPTOR_TYPE,                                      /*bDescriptorType*/
            0x00 | 0x01,                                                         /*bEndpointAddress*/
            USB_EP_BM_ATTRIBUTES_INTERRUPT,                                     /*bmAttributes*/
            64,                                                                 /*wMaxPacketSize*/
            0x01                                                                 /*bInterval*/
        }
    },
};

const char __STRING_WLANGS[] = {
    1 * 2 + 2,                                                              /*bLength*/
    USB_STRING_DESCRIPTOR_TYPE,                                             /*bDescriptorType*/
    0x09, 0x04                                                              // 0x409 - English
};

const char __STRING_MANUFACTURER[] = {
    10 * 2 + 2,                                                             /*bLength*/
    USB_STRING_DESCRIPTOR_TYPE,                                             /*bDescriptorType*/
    'A', 0,
    'l', 0,
    'a', 0,
    'd', 0,
    'd', 0,
    'i', 0,
    'n', 0,
    ' ', 0,
    'R', 0,
    'D', 0,
};

const char __STRING_PRODUCT[] = {
    10 * 2 + 2,                                                              /*bLength*/
    USB_STRING_DESCRIPTOR_TYPE,                                             /*bDescriptorType*/
    'U', 0,
    'S', 0,
    'B', 0,
    ' ', 0,
    'G', 0,
    'l', 0,
    'i', 0,
    't', 0,
    'c', 0,
    'h', 0
};
#pragma pack(pop)


#endif /* USB_DESC_H_ */
