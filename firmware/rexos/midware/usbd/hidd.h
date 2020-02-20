/*
 * hidd.h
 *
 *  Created on: 14 рту. 2018 у.
 *      Author: RLeonov
 */

#ifndef _HIDD_H_
#define _HIDD_H_

#include "rexos/midware/usbd/usbd.h"
#include "rexos/userspace/hid.h"

#define DFU_REPORT_COUNT                         64

#define WBVAL(x)                               ((x) & 0xFF),(((x) >> 8) & 0xFF)
#define B3VAL(x)                               ((x) & 0xFF),(((x) >> 8) & 0xFF),(((x) >> 16) & 0xFF)
#define HID_Input(x)                            0x81,x
#define HID_Output(x)                           0x91,x
#define HID_Feature(x)                          0xB1,x
#define HID_Collection(x)                       0xA1,x
#define HID_ReportSize(x)                       0x75,x
#define HID_LogicalMaxS(x)                      0x26,(x&0xFF),((x>>8)&0xFF)
#define HID_LogicalMin(x)                       0x15,x
#define HID_ReportCount(x)                      0x95,x
#define HID_Usage(x)                            0x09,x

#define DFU_USAGE_PAGE                          0xFA1D  //!< Aladdin R.D. usage page
#define DFU_USAGE                               0x02    //!< Control usage for top-level collection
#define DFU_USAGE_DATA_IN                       0x20    //!< Raw IN data report
#define DFU_USAGE_DATA_OUT                      0x21    //!< Raw OUT data report

extern const USBD_CLASS __HIDD_CLASS;
#endif /* _HIDD_H_ */
