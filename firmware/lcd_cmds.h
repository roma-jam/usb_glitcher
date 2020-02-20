/*
 * lcd_cmds.h
 *
 *  Created on: 15 февр. 2018 г.
 *      Author: RLeonov
 */

#ifndef LCD_CMDS_H_
#define LCD_CMDS_H_

// LCD Commands
#define PCD8544_NOP                         0x00
#define PCD8544_FUNCTIONSET                 0x20

#define PCD8544_FUNCTIONSET_NORMAL          (0 << 0)
#define PCD8544_FUNCTIONSET_EXTENDED        (1 << 0)
#define PCD8544_FUNCTIONSET_HORIZONT_ADDR   (0 << 1)
#define PCD8544_FUNCTIONSET_VERTICAL_ADDR   (1 << 1)
#define PCD8544_FUNCTIONSET_ACTIVE          (0 << 2)
#define PCD8544_FUNCTIONSET_POWER_DOWN      (1 << 2)

// H = 0
#define PCD8544_DISPLAYCONTROL              0x08
#define PCD8544_DISPLAYCONTROL_BLANK        ((0 << 2) | (0 << 0))
#define PCD8544_DISPLAYCONTROL_NORMAL       ((1 << 2) | (0 << 0))
#define PCD8544_DISPLAYCONTROL_ALLON        ((0 << 2) | (1 << 0))
#define PCD8544_DISPLAYCONTROL_INV_VIDEO    ((1 << 2) | (1 << 0))

// 0 <= X <= 83
#define PCD8544_SETXADDR                    0x80
// 0 <= Y <= 5
#define PCD8544_SETYADDR                    0x40

// H = 1
#define PCD8544_SETTEMP                     0x04

#define PCD8544_SETTEMP_COEFF_0             (0)
#define PCD8544_SETTEMP_COEFF_1             (1)
#define PCD8544_SETTEMP_COEFF_2             (2)
#define PCD8544_SETTEMP_COEFF_3             (3)

#define PCD8544_SETBIAS                     0x10
#define PCD8544_SETVOP                      0x80

#endif /* LCD_CMDS_H_ */
