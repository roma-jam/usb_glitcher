/*
    RExOS - embedded RTOS
    Copyright (c) 2011-2017, Alexey Kramarenko
    All rights reserved.
*/

#ifndef SPI_H
#define SPI_H

#include <stdint.h>
#include "io.h"

#define SPI_DATA_FIRST_EDGE         (0 << 0)
#define SPI_DATA_SECOND_EDGE        (1 << 0)

#define SPI_DATA_CK_IDLE_LOW        (0 << 1)
#define SPI_DATA_CK_IDLE_HIGH       (1 << 1)

#define SPI_MODE_SLAVE              (0 << 2)
#define SPI_MODE_MASTER             (1 << 2)

#define SPI_BAUDRATE_DIV2           (0 << 3)
#define SPI_BAUDRATE_DIV4           (1 << 3)
#define SPI_BAUDRATE_DIV8           (2 << 3)
#define SPI_BAUDRATE_DIV16          (3 << 3)
#define SPI_BAUDRATE_DIV32          (4 << 3)
#define SPI_BAUDRATE_DIV64          (5 << 3)
#define SPI_BAUDRATE_DIV128         (6 << 3)
#define SPI_BAUDRATE_DIV256         (7 << 3)

#define SPI_DATA_BO_MSB             (0 << 7)
#define SPI_DATA_BO_LSB             (1 << 7)

#define SPI_SSI_OFF                 (0 << 8)
#define SPI_SSI_ON                  (1 << 8)

#define SPI_NSS_HARDWARE            (0 << 9)
#define SPI_NSS_SOFTWARE            (1 << 9)

#define SPI_MODE_FULLDUPLEX         (0 << 10)
#define SPI_MODE_RECIEVE            (1 << 10)

typedef enum {
    SPI_BYTE = IPC_USER,
    SPI_IPC_MAX
} SPI_IPCS;

bool spi_open(int port, unsigned int settings);
void spi_close(int port);

unsigned int spi_byte(int port, uint8_t byte);
unsigned int spi_write_data(int port, IO* io);
unsigned int spi_read_data(int port, IO* io, unsigned int size);

//typedef enum {
//    SPI_MSBFIRST = 0,
//    SPI_LSBFIRST
//} SPI_ORDER;
//
//typedef enum {
//    SPI_DIV_2 = 0,
//    SPI_DIV_4,
//    SPI_DIV_8,
//    SPI_DIV_16,
//    SPI_DIV_32,
//    SPI_DIV_64,
//    SPI_DIV_128,
//    SPI_DIV_256
//} SPI_SPEED;
//
//typedef struct {
//    uint32_t cs_pin;
//    uint8_t cpha;
//    uint8_t cpol;
//    uint8_t size;   // data size from 4 to 16 bit
//    SPI_ORDER order;
//    SPI_SPEED speed;
//}SPI_MODE;
//
//#define SPI_LSBFIRST_MSK          ( 1 << 31)
//#define SPI_CPOL_MSK           ( 1 << 30)
//#define SPI_CPHA_MSK           ( 1 << 29)
//
//bool spi_open(int port, SPI_MODE* mode);
//void spi_close(int port);
//bool spi_send(uint32_t port, uint32_t size, uint16_t* data, uint32_t cs_pin);

#endif // SPI_H
