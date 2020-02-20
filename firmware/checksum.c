/*
 * checksum.c
 *
 *  Created on: 15 рту. 2018 у.
 *      Author: RLeonov
 */
#include "checksum.h"
#include "app_private.h"
#include "config.h"
#include "rexos/userspace/stdio.h"
#include "rexos/userspace/stm32/stm32.h"
#include "crc.h"

void checksum_init(APP* app)
{
    if (crc32_no_table((void*)(FLASH_BASE), (unsigned int)&__ETEXT - FLASH_BASE) != __ETEXT[0])
    {
#if (APP_DEBUG_ERRORS)
        printf("Flash checksum error\n");
#endif
    }
}
