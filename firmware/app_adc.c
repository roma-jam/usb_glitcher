/*
    RExOS - embedded RTOS
    Copyright (c) 2011-2019, RExOS team
    All rights reserved.

    author: RJ (jam_roma@yahoo.com)
*/


#include "app_adc.h"
#include "app_private.h"
#include "lcd.h"
#include "rexos/userspace/stdio.h"
#include "rexos/userspace/stm32/stm32_driver.h"
#include "config.h"

#define MV_1V                          1000

static inline void adc_get_measures(APP* app)
{
    unsigned int normal_mV = adc_get_mV(app, ADC_VOLTAGE_VALUE_NORMAL);
    unsigned int glitch_mV = adc_get_mV(app, ADC_VOLTAGE_VALUE_GLITCH);

    /* UART */
//    printf("Normal voltage: %d mV\n", normal_mV);
//    printf("Glitch voltage: %d mV\n", glitch_mV);

    /* LCD */
    lcd_printf(app, LCD_ADC_NORMAL_MEAS_ROW, 0, "Vnorm:     ");
    lcd_printf(app, LCD_ADC_GLITCH_MEAS_ROW, 0, "Vglch:     ");

    /* NORMAL */
    if(normal_mV > MV_1V)
    {
        /* V */
        lcd_printf(app, LCD_ADC_NORMAL_MEAS_ROW, LCD_ADC_VALUE_COLUMN, "%d.%d",
                normal_mV / MV_1V,
                (normal_mV % MV_1V) / 10);
        lcd_printf(app, LCD_ADC_NORMAL_MEAS_ROW, LCD_ADC_MEAS_SCALE_COLUMN, " V");
    }
    else
    {
        /* mV */
        lcd_printf(app, LCD_ADC_NORMAL_MEAS_ROW, LCD_ADC_VALUE_COLUMN, "%d", normal_mV);
        lcd_printf(app, LCD_ADC_NORMAL_MEAS_ROW, LCD_ADC_MEAS_SCALE_COLUMN, "mV");
    }

    /* GLITCH */
    if(glitch_mV > MV_1V)
    {
        /* V */
        lcd_printf(app, LCD_ADC_GLITCH_MEAS_ROW, LCD_ADC_VALUE_COLUMN, "%d.%d",
                glitch_mV / MV_1V,
                (glitch_mV % MV_1V) / 10);
        lcd_printf(app, LCD_ADC_GLITCH_MEAS_ROW, LCD_ADC_MEAS_SCALE_COLUMN, " V");
    }
    else
    {
        /* mV */
        lcd_printf(app, LCD_ADC_GLITCH_MEAS_ROW, LCD_ADC_VALUE_COLUMN, "%d", glitch_mV);
        lcd_printf(app, LCD_ADC_GLITCH_MEAS_ROW, LCD_ADC_MEAS_SCALE_COLUMN, "mV");
    }
}

void adc_init(APP* app)
{
    /* init adc */
    adc_open();
    /* enable pin as ADC input */
    pin_enable(ADC_VOLTAGE_NORMAL_PIN, STM32_GPIO_MODE_ANALOG, 0);
    pin_enable(ADC_VOLTAGE_GLITCH_PIN, STM32_GPIO_MODE_ANALOG, 0);
    /* create timer */
    app->adc_timer = timer_create(0, HAL_ADC);
    /* start measurment */
    timer_start_ms(app->adc_timer, ADC_MEASURE_TIMEOUT_MS);

}

void adc_deinit(APP* app)
{
    timer_stop(app->adc_timer, ADC_MEASURE_TIMEOUT_MS, HAL_ADC);
    /* adc close */
}

unsigned int adc_get_mV(APP* app, ADC_VOLTAGE_VALUE avv)
{
    /* get voltage */
    return (ADC_RESISTOR_DIVIDER * ADC2mV(adc_get(avv, ADC_SAMPLE_RATE_160_5_CLK), ADC_REFERENCE_mV, ADC_RESOLUTION_BIT));
}

static inline void adc_timeout(APP* app)
{
    /* measure values */
    adc_get_measures(app);
    /* start timer again */
    timer_start_ms(app->adc_timer, ADC_MEASURE_TIMEOUT_MS);
}

void app_adc_request(APP* app, IPC* ipc)
{
    switch (HAL_ITEM(ipc->cmd))
    {
        case IPC_OPEN:
            /* get first measure */
            adc_get_measures(app);
            break;
        case IPC_TIMEOUT:
            adc_timeout(app);
            break;
        default:
            printf("APP ADC unsupport item %08X\n", HAL_ITEM(ipc->cmd));
            break;
    }
}

