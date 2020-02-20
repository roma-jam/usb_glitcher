/*
 * lcd.h
 *
 *  Created on: 15 февр. 2018 г.
 *      Author: RLeonov
 */

#ifndef LCD_H_
#define LCD_H_

#include "app.h"
#include "rexos/userspace/types.h"
#include "rexos/userspace/io.h"

typedef enum {
    LCD_INIT = IPC_USER,
    LCD_PRINTF,
    LCD_CLEAR,
    LCD_MODE_SET,
    LCD_MODE_GET
} LCD_IPCS;

typedef enum {
    LCD_MODE_OFF = 0,
    LCD_MODE_DRAW, // zero has no effect
    LCD_MODE_CLEAR, // zero has no effect, one clears
    LCD_MODE_OVERWRITE, // write both zeros and ones
    LCD_MODE_OVERWRITE_INVERTED, // write both with negated meaning
    LCD_MODE_INVERT // zero has no effect, one inverts
} LCD_MODE;

#pragma pack(push, 1)
typedef struct {
    uint8_t row;
    uint8_t column;
} LCD_STACK;
#pragma pack(pop)

typedef struct {
    HANDLE timer;
    uint8_t* data_buf;
    unsigned int data_buf_size;
    bool need_update;
    LCD_MODE mode;
} LCD;

extern const REX __LCD;

void lcd_init(APP* app);
void lcd_deinit(APP* app);
void lcd_clear(APP* app);
void lcd_set_mode(APP* app, LCD_MODE mode);
LCD_MODE lcd_get_mode(APP* app);
void lcd_printf(APP* app, uint8_t row, uint8_t column, const char *format, ...);
//void lcd_printf_mode(APP* app, uint8_t row, uint8_t column, LCD_MODE mode, const char *format, ...);

#endif /* LCD_H_ */
