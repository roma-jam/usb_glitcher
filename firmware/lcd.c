/*
 * lcd.c
 *
 *  Created on: 15 февр. 2018 г.
 *      Author: RLeonov
 */

#include <string.h>
#include <stdarg.h>
#include "app_private.h"
#include "rexos/userspace/stm32/stm32_driver.h"
#include "rexos/userspace/gpio.h"
#include "rexos/userspace/pin.h"
#include "rexos/userspace/sys.h"
#include "rexos/userspace/stdio.h"
#include "rexos/userspace/stdlib.h"
#include "lcd.h"
#include "lcd_cmds.h"
#include "lcd_font.h"
#include "lcd_format.h"
#include "config.h"
#include "crc.h"

void lcd_task();


#define DATA_MODE()             gpio_set_pin(LCD_DC_PIN)
#define CMD_MODE()              gpio_reset_pin(LCD_DC_PIN)
#define RESET_HI()              gpio_set_pin(LCD_RESET_PIN)
#define RESET_LO()              gpio_reset_pin(LCD_RESET_PIN)
#define CS_HI()                 gpio_set_pin(LCD_CE_PIN)
#define CS_LO()                 gpio_reset_pin(LCD_CE_PIN)
#define SCLK_HI()               gpio_set_pin(LCD_SCLK_PIN)
#define SCLK_LO()               gpio_reset_pin(LCD_SCLK_PIN)
#define DIN_HI()                gpio_set_pin(LCD_DIN_PIN)
#define DIN_LO()                gpio_reset_pin(LCD_DIN_PIN)

#define BCKLT_ON()              gpio_reset_pin(LCD_BCKLT_PIN)
#define BCKLT_OFF()             gpio_set_pin(LCD_BCKLT_PIN)

const REX __LCD = {
    //name
    "LCD",
    //size
    1024,
    //priority
    202,
    //flags
    PROCESS_FLAGS_ACTIVE | REX_FLAG_PERSISTENT_NAME,
    //function
    lcd_task
};


static inline void lcd_send_cmd(uint8_t cmd)
{
    CS_LO();
    CMD_MODE();
#if (LCD_BITBYTE)
    for(uint8_t i = 0; i < 8; i++)
    {
        SCLK_LO();
        if(cmd & 0x80)
            DIN_HI();
        else
            DIN_LO();
        cmd <<= 1;
        SCLK_HI();
    }
#endif
    CS_HI();
}

static inline void lcd_send_data(uint8_t byte)
{
    CS_LO();
    DATA_MODE();
#if (LCD_BITBYTE)
    for(uint8_t i = 0; i < 8; i++)
    {
        SCLK_LO();
        if(byte & 0x01)
            DIN_HI();
        else
            DIN_LO();
        byte >>= 1;
        SCLK_HI();
    }
#endif
    CS_HI();
}

static inline void lcd_device_draw_block(LCD* lcd, unsigned int index, uint8_t data, uint8_t mask)
{
    uint8_t *w = lcd->data_buf + index;
    uint8_t data2 = data;
    uint8_t mask2 = mask;

    switch (lcd->mode)
    {
        case LCD_MODE_DRAW:                 *w |= data2;                            break;
        case LCD_MODE_CLEAR:                *w &= ~data2;                           break;
        case LCD_MODE_OVERWRITE:            *w = (*w & ~mask2) | data2;             break;
        case LCD_MODE_OVERWRITE_INVERTED:   *w = (*w & ~mask2) | (data2 ^ mask2);   break;
        case LCD_MODE_INVERT:               *w ^= data2;                            break;
        default: break;
    }
}

static inline void lcd_device_draw_char(LCD* lcd, unsigned int* index, unsigned char data)
{
    for(uint8_t i = 0; i < LCD_FONT_WIDTH; i++)
    {
#if (LCD_FONT_WIDTH == 4)
        lcd_device_draw_block(lcd, (*index)++, Font_4x8_mirror_Data[data][i], 0xFF);
#endif
#if (LCD_FONT_WIDTH == 5)
        lcd_device_draw_block(lcd, (*index)++, Font_5x8_mirror_Data[data][i], 0xFF);
#endif //
        if ((*index) >= lcd->data_buf_size)
            (*index) = 0;
    }
}

static inline void lcd_device_open(LCD* lcd)
{
#if (APP_DEBUG_LCD)
    printf("LCD: open\n");
#endif // APP_DEBUG_LCD
    /* Setup GPIOs */
    gpio_enable_pin(LCD_BCKLT_PIN, GPIO_MODE_OUT);
    gpio_enable_pin(LCD_RESET_PIN, GPIO_MODE_OUT);
    gpio_enable_pin(LCD_CE_PIN, GPIO_MODE_OUT);
    gpio_enable_pin(LCD_SCLK_PIN, GPIO_MODE_OUT);
    gpio_enable_pin(LCD_DC_PIN, GPIO_MODE_OUT);
    gpio_enable_pin(LCD_DIN_PIN, GPIO_MODE_OUT);

    RESET_HI();
    SCLK_HI();
    CS_HI();
    BCKLT_ON();
}

static inline void lcd_device_init(LCD* lcd, IPC* ipc)
{
#if (APP_DEBUG_LCD)
    printf("LCD: init, w:%d, h:%d\n", ipc->param1, ipc->param2);
#endif // APP_DEBUG_LCD

    lcd->timer = timer_create(0, HAL_LCD);

    lcd->data_buf_size = (ipc->param1 * ipc->param2) >> 3;
    lcd->data_buf = malloc(lcd->data_buf_size);

    if(lcd->timer == INVALID_HANDLE || lcd->data_buf == NULL)
    {
#if (APP_DEBUG_ERRORS)
        printf("LCD: init device failure\n");
#endif // APP_DEBUG_ERRORS
        return;
    }

    timer_start_ms(lcd->timer, LCD_UPDATE_MS);
    // display ON
    RESET_LO();
    sleep_ms(99);
    RESET_HI();

    lcd_send_cmd(PCD8544_FUNCTIONSET | PCD8544_FUNCTIONSET_EXTENDED);
    lcd_send_cmd(PCD8544_SETBIAS | 0x03);
    lcd_send_cmd(PCD8544_SETVOP | 0x3D);
    lcd_send_cmd(PCD8544_FUNCTIONSET | PCD8544_FUNCTIONSET_NORMAL);
    lcd_send_cmd(PCD8544_DISPLAYCONTROL | PCD8544_DISPLAYCONTROL_NORMAL);

    lcd_send_cmd(PCD8544_SETXADDR | 0);
    lcd_send_cmd(PCD8544_SETYADDR | 0);

    lcd->mode = LCD_MODE_OVERWRITE;
    memset(lcd->data_buf, 0x00, lcd->data_buf_size);
    lcd->need_update = true;
}


static inline void lcd_device_clear(LCD* lcd)
{
    memset(lcd->data_buf, 0x00, lcd->data_buf_size);
    lcd_send_cmd(PCD8544_DISPLAYCONTROL | PCD8544_DISPLAYCONTROL_NORMAL);
}

static inline void lcd_device_update(LCD* lcd)
{
    if(!lcd->need_update)
        return;
    lcd_send_cmd(PCD8544_SETXADDR | 0);
    lcd_send_cmd(PCD8544_SETYADDR | 0);

    for (uint16_t i = 0; i < lcd->data_buf_size; i++)
        lcd_send_data(lcd->data_buf[i]);

    lcd->need_update = false;
}

void lcd_device_timeout(LCD* lcd)
{
    if(lcd->timer == INVALID_HANDLE)
        return;

    lcd_device_update(lcd);
    timer_start_ms(lcd->timer, LCD_UPDATE_MS);
}

static inline void lcd_device_printf(LCD* lcd, IO* io)
{
    unsigned int index = 0;
    unsigned int i;
    LCD_STACK* stack = io_stack(io);

    index = stack->column * LCD_FONT_WIDTH + stack->row * LCD_WIDTH;
    for(i = 0; i < io->data_size; i++)
        lcd_device_draw_char(lcd, &index, *(uint8_t*)(io_data(io) + i));

    lcd->need_update = true;
}

static inline void lcd_device_close(LCD* lcd)
{
#if (APP_DEBUG_LCD)
    printf("LCD: close\n");
#endif // APP_DEBUG_LCD

    BCKLT_OFF();

    /* stop timer */
    timer_stop(lcd->timer, LCD_UPDATE_MS, HAL_LCD);
    /* clear lcd */
    lcd_device_clear(lcd);
    lcd_device_update(lcd);
}

static void lcd_cmd(LCD* lcd, IPC* ipc)
{
    switch(HAL_ITEM(ipc->cmd))
    {
        case IPC_OPEN:
            lcd_device_open(lcd);
            break;
        case IPC_CLOSE:
            lcd_device_close(lcd);
            break;
        case IPC_TIMEOUT:
            lcd_device_timeout(lcd);
            break;
        case IPC_WRITE:
            lcd_device_printf(lcd, (IO*)ipc->param2);
            break;
        case LCD_INIT:
            lcd_device_init(lcd, ipc);
            break;
        case LCD_CLEAR:
            lcd_device_clear(lcd);
            break;
        case LCD_MODE_SET:
            lcd->mode = (LCD_MODE)ipc->param1;
            break;
        case LCD_MODE_GET:
            ipc->param2 = lcd->mode;
            break;
        default:
#if (APP_DEBUG_ERRORS)
            printf("LCD: Unsupported cmd item: %#X\n", HAL_ITEM(ipc->cmd));
#endif // APP_DEBUG_ERRORS
            break;
    }
}

void lcd_init(APP* app)
{
    // process create
    app->lcd = process_create(&__LCD);

    // init
    ack(app->lcd, HAL_REQ(HAL_LCD, IPC_OPEN), 0, 0, 0);
    ack(app->lcd, HAL_REQ(HAL_LCD, LCD_INIT), LCD_WIDTH, LCD_HEIGHT, 0);

    lcd_printf(app, 0, 2, "USB Glitcher");
}

void lcd_deinit(APP* app)
{
    ack(app->lcd, HAL_REQ(HAL_LCD, IPC_CLOSE), 0, 0, 0);
}

void lcd_clear(APP* app)
{
    ack(app->lcd, HAL_REQ(HAL_LCD, LCD_CLEAR), 0, 0, 0);
}

void lcd_set_mode(APP* app, LCD_MODE mode)
{
    ack(app->lcd, HAL_REQ(HAL_LCD, LCD_MODE_SET), mode, 0, 0);
}

LCD_MODE lcd_get_mode(APP* app)
{
    return get(app->lcd, HAL_REQ(HAL_LCD, LCD_MODE_GET), 0, 0, 0);
}

void lcd_printf(APP* app, uint8_t row, uint8_t column, const char *format, ...)
{
    IO* io = io_create(sizeof(LCD_STACK) + (LCD_STR_HEIGHT * LCD_STR_WIDTH + 1));

    if(io == NULL)
        return;

    LCD_STACK* stack = io_push(io, sizeof(LCD_STACK));
    stack->row = row;
    stack->column = column;

    va_list args;
    va_start(args, format);
    io->data_size = __vsprintf(io_data(io), format, args);
    va_end(args);

    io_write_sync(app->lcd, HAL_REQ(HAL_LCD, IPC_WRITE), 0, io);
    io_pop(io, sizeof(LCD_STACK));
    io_destroy(io);
}

void lcd_task()
{
    LCD lcd;
    IPC ipc;

    lcd.timer = INVALID_HANDLE;
    lcd.data_buf = NULL;
    lcd.data_buf_size = 0;
    lcd.mode = LCD_MODE_OFF;
    lcd.need_update = false;

    open_stdout();

    for (;;)
    {
        ipc_read(&ipc);
        switch (HAL_GROUP(ipc.cmd))
        {
            case HAL_LCD:
                lcd_cmd(&lcd, &ipc);
                break;

            default:
                printf("LCD: Unsupported cmd group\n");
                error(ERROR_NOT_SUPPORTED);
        }
        ipc_write(&ipc);
    }
}
