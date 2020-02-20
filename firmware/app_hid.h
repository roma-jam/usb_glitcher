/*
 * app_hid.h
 *
 *  Created on: 15 рту. 2018 у.
 *      Author: RLeonov
 */

#ifndef APP_HID_H_
#define APP_HID_H_

#include "app.h"
#include "rexos/userspace/ipc.h"
#include "flash_update_STM32L052K8.h"

#define FLASH_PAGE_SIZE                 128
#define HID_MEMORY_WRITE_CHUNK_SZ       55
#define HID_UPDATE_TIMEOUT_MS           1000
#define FLASH_FW_UPDATE_BANK            0x08008000

typedef enum {
    HID_CMD_RESERVED = 0x00,
    HID_CMD_GET_INFO,
    HID_CMD_SET_STATE,
    HID_CMD_SET_CONFIG,
    HID_CMD_START_UPDATE_FIRMWARE,
    HID_CMD_UPDATE_FIRMWARE,
    HID_CMD_LAUNCH_FIRMWARE,
    HID_CMD_GET_FW_BUILD,
    HID_CMD_MAX
} HID_CMD;

typedef enum {
    HID_FW_UPDATE_STATE_IDLE = 0,
    HID_FW_UPDATE_STATE_IN_PROCESS
} HID_FW_UPDATE_STATE;

#pragma pack(push, 1)
typedef struct {
    uint8_t cmd_id;
    uint8_t param1;
} HID_DATA_HEADER;

typedef struct {
    bool flag;
    uint32_t delay_ms;
    uint32_t timeout_ms;
} HID_CONFIG_DATA;

typedef struct {
    uint8_t* fw_ram_foo;
    uint8_t fw_page[FLASH_PAGE_SIZE];
    unsigned int fw_page_addr;
    unsigned int fw_total_size;
    unsigned int fw_current_size;
    unsigned int fw_collected_size;
    HID_FW_UPDATE_STATE fw_state;
    HANDLE timer;
} HID;
#pragma pack(pop)

void app_hid_init(APP* app);
void app_hid_request(APP* app, IPC* ipc);


#endif /* APP_HID_H_ */
