/*
    RExOS - embedded RTOS
    Copyright (c) 2011-2017, Alexey Kramarenko
    All rights reserved.
*/

#include "spi.h"
#include "gpio.h"
#include "object.h"
#include "sys_config.h"
#include "core/core.h"

#ifdef EXODRIVERS
bool spi_open(int port, unsigned int settings)
{
    return get_handle_exo(HAL_REQ(HAL_SPI, IPC_OPEN), port, settings, 0) != INVALID_HANDLE;
}

void spi_close(int port)
{
    ack_exo(HAL_REQ(HAL_SPI, IPC_CLOSE), port, 0, 0);
}

unsigned int spi_byte(int port, uint8_t byte)
{
    return get_exo(HAL_REQ(HAL_SPI, SPI_BYTE), port, byte, 0);
}

unsigned int spi_write_data(int port, IO* io)
{
    return io_read_sync_exo(HAL_IO_REQ(HAL_SPI, IPC_WRITE), port, (unsigned int)io, io->data_size);
}

unsigned int spi_read_data(int port, IO* io, unsigned int size)
{
    return io_read_sync_exo(HAL_IO_REQ(HAL_SPI, IPC_READ), port, (unsigned int)io, size);
}
#else
bool spi_open(int port, unsigned int settings)
{
    return get_handle(object_get(SYS_OBJ_CORE), HAL_REQ(HAL_SPI, IPC_OPEN), port, settings, 0) != INVALID_HANDLE;
}

void spi_close(int port)
{
    ack(object_get(SYS_OBJ_CORE), HAL_REQ(HAL_SPI, IPC_CLOSE), port, 0, 0);
}

unsigned int spi_byte(int port, uint8_t byte)
{
    return get(object_get(SYS_OBJ_CORE), HAL_REQ(HAL_SPI, SPI_BYTE), port, byte, 0);
}

unsigned int spi_send_data(int port, uint8_t* data, unsigned int data_size)
{
    return io_read_sync(object_get(SYS_OBJ_CORE), HAL_REQ(HAL_SPI, SPI_SEND_DATA), port, data, data_size);
}

unsigned int spi_get_data(int port, uint8_t* data)
{
    return io_read_sync(object_get(SYS_OBJ_CORE), HAL_REQ(HAL_SPI, SPI_GET_DATA), port, data, 0);
}
#endif

//#ifdef EXODRIVERS
//bool spi_open(int port,  SPI_MODE* mode)
//{
//    uint32_t prom = (mode->speed) | ((mode->size-1) << 8);
//    if(mode->order == SPI_LSBFIRST) prom |= SPI_LSBFIRST_MSK;
//    if(mode->cpol) prom |= SPI_CPOL_MSK;
//    if(mode->cpha) prom |= SPI_CPHA_MSK;
//    return get_handle_exo(HAL_REQ(HAL_SPI, IPC_OPEN), port, prom, mode->cs_pin) != INVALID_HANDLE;
//}
//
//void spi_close(int port)
//{
//    ipc_post_exo(HAL_CMD(HAL_SPI, IPC_CLOSE), port, 0, 0);
//}
//
//bool spi_send(uint32_t port, uint32_t size, uint16_t* data, uint32_t cs_pin)
//{
//    IPC ipc;
//    ipc.process = KERNEL_HANDLE;
//    ipc.cmd = HAL_REQ(HAL_SPI, IPC_WRITE);
//    if(size>3) return false;
//    ipc.param1 = (data[2] << 16) | (size << 8) | port;
//    ipc.param2 = data[1];
//    ipc.param3 = data[0];
//    gpio_reset_pin(cs_pin);
//    call(&ipc);
//    gpio_set_pin(cs_pin);
//    data[0] = ipc.param3;
//    if(size > 1)
//        data[1] = ipc.param2;
//    if(size > 2)
//        data[2] = ipc.param1 & 0xFFFF;
//    return true;
//}
//#endif //EXODRIVERS
