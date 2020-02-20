/*
 * device.c
 *
 *  Created on: 12 рту. 2018 у.
 *      Author: RomaJam
 */


#include "app_private.h"
#include "rexos/userspace/stm32/stm32_driver.h"
#include "rexos/userspace/stdio.h"
#include "rexos/userspace/gpio.h"
#include "rexos/userspace/pin.h"
#include "rexos/userspace/eep.h"
#include "config.h"
#include "lcd.h"
#include "crc.h"

#define DEVICE_POWER_OFF()          { gpio_reset_pin(DEVICE_POWER_PIN); }
#define DEVICE_POWER_ON()           { gpio_set_pin(DEVICE_POWER_PIN);   }

void device_init(APP* app)
{
    gpio_enable_pin(DEVICE_POWER_PIN, GPIO_MODE_OUT);
    DEVICE_POWER_OFF();

    app->device.state = DEVICE_STATE_UNAWARE;

    /* read EEPROM config */
    eep_read(DEVICE_CONFIG_ADDR, (void*)&app->device, sizeof(DEVICE));
    /* no MAGIC - set init values */
    if(app->device.magic != DEVICE_MAGIC)
    {
#if (APP_DEBUG_DEVICE)
        printf("DEVICE: no magic found, init\n");
#endif // APP_DEBUG_DEVICE
        app->device.magic = DEVICE_MAGIC;
        app->device.standalone_flag = false;
        app->device.state = DEVICE_STATE_UNAWARE;
        app->device.delay_ms = DEVICE_DEAFULT_DELAY_MS;
        app->device.timeout_ms = DEVICE_MIN_PERIOD_DELAY_MS;
        app->device.switch_counter = 0;
        device_set_state(app, DEVICE_STATE_OFF);
        app->device.crc32 = crc32_no_table((const unsigned char*)&app->device, sizeof(DEVICE) - sizeof(uint32_t));
        eep_write(DEVICE_CONFIG_ADDR, (const void*)&app->device, sizeof(DEVICE));
        return;
    }

    device_set_state(app, app->device.state);
}

void device_set_config(APP* app, bool standalone_flag, unsigned int delay_ms, unsigned int timeout_ms)
{
    app->device.standalone_flag = standalone_flag;

    if((delay_ms >= 0) && (delay_ms < DEVICE_MIN_DM_DP_DELAY_MS))
        delay_ms = DEVICE_MIN_DM_DP_DELAY_MS;

    if((timeout_ms >= 0) && (timeout_ms < DEVICE_MIN_PERIOD_DELAY_MS))
        timeout_ms = DEVICE_MIN_PERIOD_DELAY_MS;

#if (APP_DEBUG_DEVICE)
    printf("DEVICE: set config\n");
    printf("flag: %X\n", standalone_flag);
    printf("delay_ms: %d ms\n", delay_ms);
    printf("timeout_ms: %d ms\n", timeout_ms);
#endif // APP_DEBUG_DEVICE
    /* update EEPROM config */
    app->device.delay_ms = delay_ms;
    app->device.timeout_ms = timeout_ms;
    app->device.crc32 = crc32_no_table((const unsigned char*)&app->device, sizeof(DEVICE) - sizeof(uint32_t));
    eep_write(DEVICE_CONFIG_ADDR, (const void*)&app->device, sizeof(DEVICE));
}

void device_set_state(APP* app, DEVICE_STATE new_state)
{
    lcd_printf(app, LCD_DEVICE_STATE_ROW, 0, "Device: %s ", (new_state == DEVICE_STATE_OFF)? "OFF" : "ON");

    switch(new_state)
    {
        case DEVICE_STATE_OFF:
        {
            DEVICE_POWER_OFF();
            break;
        }
        case DEVICE_STATE_ON:
        {
            DEVICE_POWER_ON();
            break;
        }
        default:
            return;
    }

    app->device.state = new_state;
    app->device.switch_counter++;
    app->device.crc32 = crc32_no_table((const unsigned char*)&app->device, sizeof(DEVICE) - sizeof(uint32_t));
    eep_write(DEVICE_CONFIG_ADDR, (const void*)&app->device, sizeof(DEVICE));
}
