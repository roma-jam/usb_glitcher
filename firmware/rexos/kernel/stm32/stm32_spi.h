/*
 * stm32_spi.h
 *
 *  Created on: 1 апр. 2017 г.
 *      Author: RomaJam
 */

#ifndef STM32_SPI_H
#define STM32_SPI_H

#include "stm32_exo.h"
#include "../../userspace/io.h"
#include "../../userspace/i2c.h"
#include "../../userspace/process.h"
#include <stdbool.h>

typedef enum {
    SPI_MODE_IDLE = 0,
    SPI_MODE_TX,
    SPI_MODE_RX
} SPI_MODE;

typedef struct  {
    IO* io;
    HANDLE process;
    SPI_MODE mode;
    int tx_length;
    int rx_length;
} SPI;

typedef struct  {
    SPI* spis[SPI_COUNT];
} SPI_DRV;

void stm32_spi_init(EXO* exo);
void stm32_spi_request(EXO* exo, IPC* ipc);


#endif /* STM32_SPI_H */
