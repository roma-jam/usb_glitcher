/*
    RExOS - embedded RTOS
    Copyright (c) 2011-2019, RExOS team
    All rights reserved.

    author: RJ (jam_roma@yahoo.com)
*/

#include "app_private.h"
#include "rexos/userspace/gpio.h"
#include "rexos/userspace/stdio.h"
#include "rexos/userspace/stm32/stm32_driver.h"
#include "config.h"
#include "lcd.h"
#include "mux.h"


static inline void mux_enable_glitch()
{
    gpio_reset_pin(MULTIPLEXOR_TARGET_ENABLE_PIN);
    gpio_set_pin(MULTIPLEXOR_TARGET_SWITCH_PIN);
}

static inline void mux_enable()
{
    gpio_reset_pin(MULTIPLEXOR_TARGET_ENABLE_PIN);
    gpio_reset_pin(MULTIPLEXOR_TARGET_SWITCH_PIN);

}

static inline void mux_disable()
{
    gpio_set_pin(MULTIPLEXOR_TARGET_ENABLE_PIN);
    gpio_reset_pin(MULTIPLEXOR_TARGET_SWITCH_PIN);

}

void mux_init(APP* app)
{
    lcd_printf(app, LCD_DEVICE_SWITCH_MODE_ROW, 0, "MUX:");
    /* enable gpio */
    gpio_enable_pin(MULTIPLEXOR_TARGET_ENABLE_PIN, GPIO_MODE_OUT);
    gpio_enable_pin(MULTIPLEXOR_TARGET_SWITCH_PIN, GPIO_MODE_OUT);
    /* disable mux */
    app->mux_state = MUX_STATE_UNDEFINED;
    mux_set_state(app, MUX_STATE_OFF);

}

void mux_set_state(APP* app, MUX_STATE state)
{
    if((app->mux_state == state) || (state >= MUX_STATE_MAX))
        return;

    app->mux_state = state;

    switch(state)
    {
        case MUX_STATE_OFF:
            lcd_printf(app, LCD_DEVICE_SWITCH_MODE_ROW, LCD_DEVICE_SWITCH_MODE_COLUMN, "Off   ");
            mux_disable();
            break;
        case MUX_STATE_ON:
            lcd_printf(app, LCD_DEVICE_SWITCH_MODE_ROW, LCD_DEVICE_SWITCH_MODE_COLUMN, "Normal");
            mux_enable();
            break;
        case MUX_STATE_GLITCH:
            lcd_printf(app, LCD_DEVICE_SWITCH_MODE_ROW, LCD_DEVICE_SWITCH_MODE_COLUMN, "Glitch");
            mux_enable_glitch();
            break;
        default:
            break;
    }
}
