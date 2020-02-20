/*
 * device.h
 *
 *  Created on: 12 рту. 2018 у.
 *      Author: RomaJam
 */

#ifndef DEVICE_H_
#define DEVICE_H_

#include "app.h"

#define DEVICE_MAGIC            0x2DF49332
#define DEVICE_CONFIG_ADDR      0

typedef enum {
    DEVICE_STATE_UNAWARE = 0,
    DEVICE_STATE_OFF,
    DEVICE_STATE_ON
} DEVICE_STATE;

#pragma pack(push, 1)
typedef struct {
    uint32_t magic;
    bool standalone_flag;
    DEVICE_STATE state;
    uint32_t delay_ms;
    uint32_t timeout_ms;
    uint32_t switch_counter;
    uint32_t crc32;
} DEVICE;
#pragma pack(pop)

void device_init(APP* app);
void device_set_config(APP* app, bool standalone_flag, unsigned int delay_ms, unsigned int timeout);
void device_set_state(APP* app, DEVICE_STATE new_state);


#endif /* DEVICE_H_ */
