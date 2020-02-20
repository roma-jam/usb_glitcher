/*
 * app_hid.c
 *
 *  Created on: 15 рту. 2018 у.
 *      Author: RLeonov
 */

#include <string.h>
#include "app_private.h"
#include "rexos/userspace/stdio.h"
#include "rexos/userspace/types.h"
#include "rexos/userspace/error.h"
#include "rexos/userspace/sys.h"
#include "rexos/userspace/usb.h"
#include "rexos/userspace/hid.h"
#include "rexos/userspace/io.h"
#include "checksum.h"
#include "app_usb.h"
#include "app_hid.h"
#include "config.h"
#include "device.h"
#include "led.h"
#include "crc.h"
#include "app_adc.h"
#include "lcd.h"

static inline bool app_hid_start_update_fw(APP* app, uint8_t* data)
{
    /* verify possibility */
    if((unsigned int)&__ETEXT > FLASH_FW_UPDATE_BANK)
    {
#if (APP_DEBUG_HID)
        printf("APP HID: Update impossible. Size of bin %d Bytes\n", ((unsigned int)&__ETEXT - FLASH_BASE));
#endif // APP_DEBUG_HID
        return false;
    }

    /* prepare values */
    app->hid.fw_page_addr = FLASH_FW_UPDATE_BANK;
    app->hid.fw_current_size = 0;
    app->hid.fw_collected_size = 0;
    /* extract total fw size */
    app->hid.fw_total_size = (data[3] << 24) | (data[2] << 16) | (data[1] << 8) | data[0];
    app->hid.fw_state = HID_FW_UPDATE_STATE_IN_PROCESS;

#if (APP_DEBUG_HID)
    printf("APP HID: Start update\n");
    printf("total size: %d\n", app->hid.fw_total_size);
#endif // APP_DEBUG_HID
    return true;
}

static inline bool app_hid_collect_data(APP* app, uint8_t* data, unsigned int data_size)
{
    uint8_t aligned_page[FLASH_PAGE_SIZE] = { 0 };
#if (APP_DEBUG_HID)
    printf("APP HID: collect data, size %d\n", data_size);
#endif // APP_DEBUG_HID
    unsigned int in_size = data_size;

    if(app->hid.fw_current_size + in_size < FLASH_PAGE_SIZE)
    {
        memcpy(app->hid.fw_page + app->hid.fw_current_size, data, in_size);
        app->hid.fw_current_size += in_size;
        return true;
    }
    else
    {
        memcpy(app->hid.fw_page + app->hid.fw_current_size, data, FLASH_PAGE_SIZE - app->hid.fw_current_size);
        in_size -= (FLASH_PAGE_SIZE - app->hid.fw_current_size);
        app->hid.fw_current_size = FLASH_PAGE_SIZE;
    }

    if(app->hid.fw_current_size == FLASH_PAGE_SIZE)
    {
#if (APP_DEBUG_HID)
        printf("program addr 0x%08X, from %#X\n", app->hid.fw_page_addr, aligned_page);
#endif // APP_DEBUG_HID
        memcpy(aligned_page, app->hid.fw_page, FLASH_PAGE_SIZE);
        led_off();
        __disable_irq();
        flash_upd_sram(app->hid.fw_ram_foo,
                        app->hid.fw_page_addr,
                        (unsigned int)aligned_page,
                        FLASH_PAGE_SIZE,
                        false);
        __enable_irq();
        led_on();
        app->hid.fw_page_addr += FLASH_PAGE_SIZE;
        app->hid.fw_collected_size += FLASH_PAGE_SIZE;
        app->hid.fw_current_size = 0;
        memset(app->hid.fw_page, 0x00, FLASH_PAGE_SIZE);
    }

    if(in_size != 0)
    {
        memcpy(app->hid.fw_page + app->hid.fw_current_size, &data[data_size - in_size], in_size);
        app->hid.fw_current_size += in_size;
    }

    return true;
}

static inline void app_hid_update_fw(APP* app, uint8_t* data)
{
    unsigned int offset = 0;
    unsigned int size = HID_MEMORY_WRITE_CHUNK_SZ;
    /* collect and write to flash */
    if(HID_FW_UPDATE_STATE_IN_PROCESS != app->hid.fw_state)
    {
        /* TODO: set error code */
        return;
    }
    offset = (data[3] << 24) | (data[2] << 16) | (data[1] << 8) | data[0];
    /* verify overflow */
    if(FLASH_FW_UPDATE_BANK + offset > FLASH_BASE + FLASH_SIZE)
    {
        /* TODO: set error code */
        return;
    }

    if(!app_hid_collect_data(app, (uint8_t*)(data + sizeof(uint32_t)), size))
    {
        /* TODO: set error code */
        return;
    }
}

void app_hid_launch_firmware_update(APP *app)
{
    uint32_t crc32 = 0;
    uint32_t incoming_crc32 = 0;
    uint8_t aligned_page[FLASH_PAGE_SIZE] = { 0 };
#if (APP_DEBUG_HID)
    printf("APP HID: Launch\n");
#endif // APP_DEBUG_HID

    if(HID_FW_UPDATE_STATE_IN_PROCESS != app->hid.fw_state)
    {
        /* TODO: set error code */
        return;
    }

    if(app->hid.fw_current_size != 0)
    {
#if (APP_DEBUG_HID)
        printf("last program addr 0x%08X\n", app->hid.fw_page_addr);
#endif // APP_DEBUG_HID
        memcpy(aligned_page, app->hid.fw_page, FLASH_PAGE_SIZE);
        led_off();
        __disable_irq();
        flash_upd_sram(app->hid.fw_ram_foo,
                        app->hid.fw_page_addr,
                        (unsigned int)aligned_page,
                        FLASH_PAGE_SIZE,
                        false);
        __enable_irq();
        led_on();
    }

    app->hid.fw_collected_size += app->hid.fw_current_size;
    app->hid.fw_state = HID_FW_UPDATE_STATE_IDLE;

    /* verufy crc32 */
    crc32 = crc32_no_table((const unsigned char*)FLASH_FW_UPDATE_BANK, app->hid.fw_total_size - sizeof(uint32_t));
    incoming_crc32 = *((uint32_t*)(FLASH_FW_UPDATE_BANK + app->hid.fw_total_size - sizeof(uint32_t)));

    if(incoming_crc32 != crc32)
    {
        /* TODO */
        // error
        return;
    }

    /* LED OFF */
    led_off();
    /* disable ADC */
    adc_deinit(app);
    /* disable LCD */
    lcd_deinit(app);

    timer_start_ms(app->hid.timer, HID_UPDATE_TIMEOUT_MS);
}

void app_hid_init(APP* app)
{
    app->hid.fw_state = HID_FW_UPDATE_STATE_IDLE;
    app->hid.fw_ram_foo = malloc(FLASH_UPD_SIZE);

    if(app->hid.fw_ram_foo == NULL)
        return;

    memcpy(app->hid.fw_ram_foo, __FLASH_UPD, FLASH_UPD_SIZE);
    app->hid.timer = timer_create(0, HAL_USBD_IFACE);

#if (APP_DEBUG_HID)
    printf("APP HID: Init\n");
    printf("flash memcpy %#X\n", app->hid.fw_ram_foo);
#endif // APP_DEBUG_HID
}

static inline void app_hid_process_data(APP* app, IPC* ipc)
{
    IO* io = (IO*)ipc->param2;
    HID_DATA_HEADER* data_header = (HID_DATA_HEADER*)io_data(io);
    HID_CONFIG_DATA* config_data = io_data(io) + sizeof(HID_DATA_HEADER);

    switch (data_header->cmd_id)
    {
        case HID_CMD_GET_INFO:
            data_header->param1 = sizeof(DEVICE);
            memcpy((uint8_t*)(io_data(io) + sizeof(HID_DATA_HEADER)),
                    (uint8_t*)&app->device,
                    sizeof(DEVICE));
            break;
        case HID_CMD_SET_STATE:
            if(data_header->param1)
                device_set_state(app, DEVICE_STATE_ON);
            else
                device_set_state(app, DEVICE_STATE_OFF);
            break;
        case HID_CMD_SET_CONFIG:
            device_set_config(app, config_data->flag, config_data->delay_ms, config_data->timeout_ms);
            break;
        case HID_CMD_START_UPDATE_FIRMWARE:
            if(!app_hid_start_update_fw(app, (uint8_t*)config_data))
            {
                /* TODO: */
            }
            break;
        case HID_CMD_UPDATE_FIRMWARE:
            if(HID_MEMORY_WRITE_CHUNK_SZ == data_header->param1)
                app_hid_update_fw(app, (uint8_t*)config_data);
            /* TODO: */
            break;
        case HID_CMD_LAUNCH_FIRMWARE:
            app_hid_launch_firmware_update(app);
            break;
        case HID_CMD_GET_FW_BUILD:
            data_header->param1 = strlen(__BUILD_TIME);
            strcpy((char*)(io_data(io) + sizeof(HID_DATA_HEADER)), __BUILD_TIME);
            break;
        default:
            break;
    }

    ipc_post_inline(app->usbd,
            HAL_CMD(HAL_USBD_IFACE, USB_HID_IN),
            USBD_IFACE(USB_HID_INTERFACE, 0),
            0,
            0);
}

void app_hid_request(APP* app, IPC* ipc)
{
    switch (HAL_ITEM(ipc->cmd))
    {
        case IPC_TIMEOUT:
            app_usb_deinit(app);
            sleep_ms(250);
            __disable_irq();
            flash_upd_sram(app->hid.fw_ram_foo,
                    FLASH_BASE,
                    FLASH_FW_UPDATE_BANK,
                    app->hid.fw_total_size,
                    true);
            for( ;; );
            break;
        case USB_HID_OUT:
            app_hid_process_data(app, ipc);
            break;
        case USB_HID_IN:
            ipc_post_inline(app->usbd, HAL_CMD(HAL_USBD_IFACE, USB_HID_OUT), USBD_IFACE(USB_HID_INTERFACE, 0), 0, 0);
            break;
        default:
            error(ERROR_NOT_SUPPORTED);
            break;
    }
}
