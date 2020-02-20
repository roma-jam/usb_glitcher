/*
 * stm32_spi.c
 *
 *  Created on: 1 апр. 2017 г.
 *      Author: RomaJam
 */

#include "stm32_spi.h"
#include "stm32_exo_private.h"
<<<<<<< HEAD
#include "../../userspace/stdlib.h"
=======
#include "../kstdlib.h"
#include "../kerror.h"
>>>>>>> d5527e34669581874a0d7b650984fa2e7ed767a9
#include "../../userspace/stdio.h"
#include "../../userspace/spi.h"
#include "../../userspace/irq.h"
#include "../../userspace/stm32/stm32_driver.h"
#include "../kstdlib.h"
#include "../kirq.h"

#include "../../CMSIS/Device/ST/STM32L1xx/Include/stm32l151xc.h"

typedef SPI_TypeDef* SPI_TypeDef_P;
#if (SPI_COUNT > 1)
static const SPI_TypeDef_P __SPI_REGS[] =                               {SPI1, SPI2};
static const uint8_t __SPI_VECTORS[] =                                  {35, 36};
#else
static const SPI_TypeDef_P __SPI_REGS[] =                               {SPI1};
static const uint8_t __SPI_VECTORS[] =                                  {35};
static const uint32_t __SPI_CLOCK[] =                                   {RCC_APB2ENR_SPI1EN};
#endif


void stm32_spi_init(EXO* exo)
{
    int i;
    for (i = 0; i < SPI_COUNT; ++i)
        exo->spi.spis[i] = NULL;
}

<<<<<<< HEAD
static inline void stm32_spi_enable(SPI_TypeDef* SPI, bool enable)
{
    if(enable)
        SPI->CR1 |=  SPI_CR1_SPE;
    else
        SPI->CR1 &= ~SPI_CR1_SPE;
}

static void stm32_spi_on_status_isr(SPI* spi, SPI_PORT port)
{
    if(__SPI_REGS[port]->SR & SPI_SR_OVR)
    {
#if (SPI_DEBUG_ERRORS)
        iprintd("spi %d overrun\n", port);
#endif // SPI_DEBUG_ERRORS
    }

    if(__SPI_REGS[port]->SR & SPI_SR_MODF)
    {
#if (SPI_DEBUG_ERRORS)
        iprintd("spi %d mode fault\n", port);
#endif // SPI_DEBUG_ERRORS
    }

    if(__SPI_REGS[port]->SR & SPI_SR_BSY)
    {
#if (SPI_DEBUG_ERRORS)
        iprintd("spi %d busy\n", port);
#endif // SPI_DEBUG_ERRORS
    }

    if(__SPI_REGS[port]->SR & SPI_SR_UDR)
    {
#if (SPI_DEBUG_ERRORS)
        iprintd("spi %d underrun\n", port);
#endif // SPI_DEBUG_ERRORS
    }

    if(__SPI_REGS[port]->SR & SPI_SR_TXE)
    {
#if (SPI_DEBUG_ERRORS)
        iprintd("spi %d txe\n", port);
#endif // SPI_DEBUG_ERRORS
    }

    if(__SPI_REGS[port]->SR & SPI_SR_RXNE)
    {
#if (SPI_DEBUG_ERRORS)
        iprintd("spi %d rxne\n", port);
#endif // SPI_DEBUG_ERRORS
    }
}

static inline void stm32_spi_on_rx_isr(SPI* spi, SPI_PORT port)
{
    uint8_t byte = __SPI_REGS[port]->DR;

    if(spi->mode != SPI_MODE_RX)
        return;

    if(spi->rx_length == 0)
    {
        __SPI_REGS[port]->CR2 &= ~SPI_CR2_TXEIE;
        __SPI_REGS[port]->CR2 &= ~SPI_CR2_RXNEIE;
        NVIC_DisableIRQ(__SPI_VECTORS[port]);
        iio_complete(spi->process, HAL_IO_CMD(HAL_SPI, IPC_READ), port, spi->io);
        return;
    }

    *(uint8_t*)(io_data(spi->io) + spi->io->data_size++) = byte;
    spi->rx_length--;
}

static inline void stm32_spi_on_tx_isr(SPI* spi, SPI_PORT port)
{
    uint8_t byte;

    if(spi->mode != SPI_MODE_TX)
    {
        __SPI_REGS[port]->DR = 0x00;
        return;
    }

    if(--spi->tx_length == 0)
    {
        __SPI_REGS[port]->CR2 &= ~SPI_CR2_TXEIE;
        __SPI_REGS[port]->CR2 &= ~SPI_CR2_RXNEIE;
        NVIC_DisableIRQ(__SPI_VECTORS[port]);
        iio_complete(spi->process, HAL_IO_CMD(HAL_SPI, IPC_WRITE), port, spi->io);
        return;
    }

    byte = *(uint8_t*)(io_data(spi->io) + (spi->io->data_size - spi->tx_length));
    __SPI_REGS[port]->DR = byte;
}

void stm32_spi_on_isr(int vector, void* param)
{
    SPI_PORT port;
    SPI* spi;
    EXO* exo = param;
    port = SPI_1;
#if (SPI_COUNT > 1)
    if (vector != __SPI_VECTORS[0])
        port = SPI_2;
#endif
    spi = exo->spi.spis[port];

    if(__SPI_REGS[port]->SR & SPI_SR_TXE)
    {
        stm32_spi_on_tx_isr(spi, port);
        return;
    }

    if(__SPI_REGS[port]->SR & SPI_SR_RXNE)
    {
        stm32_spi_on_rx_isr(spi, port);
        return;
    }

    if (__SPI_REGS[port]->SR & (SPI_SR_OVR | SPI_SR_MODF | SPI_SR_BSY))
    {
        stm32_spi_on_status_isr(spi, port);
    }
}

void stm32_spi_open(EXO* exo, SPI_PORT port, unsigned int settings)
=======
void stm32_spi_open(EXO* exo, SPI_PORT port, uint32_t mode, uint32_t cs_pin)
>>>>>>> d5527e34669581874a0d7b650984fa2e7ed767a9
{
    SPI* spi = exo->spi.spis[port];
    if (spi)
    {
        kerror(ERROR_ALREADY_CONFIGURED);
        return;
    }

    spi = kmalloc(sizeof(SPI));
    if (spi == NULL)
    {
        kerror(ERROR_OUT_OF_MEMORY);
        return;
    }
    exo->spi.spis[port] = spi;

    spi->process = INVALID_HANDLE;
    spi->io = NULL;
    spi->tx_length = spi->rx_length = 0;
    spi->mode = SPI_MODE_IDLE;
    stm32_spi_enable(__SPI_REGS[port], false);
    // Enable clocking
    switch(port)
    {
        case SPI_1:
            RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
            break;
        case SPI_2:
            RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
            break;
        default:
        {
            error(ERROR_HARDWARE);
            return;
        }
    }

<<<<<<< HEAD
    __SPI_REGS[port]->SR = SPI_SR_TXE | SPI_SR_RXNE;
    __SPI_REGS[port]->CR1 = settings;
    __SPI_REGS[port]->CR2 = SPI_CR2_ERRIE;
    __SPI_REGS[port]->I2SCFGR &= ~((uint16_t)SPI_I2SCFGR_I2SMOD); // Disable I2S
    stm32_spi_enable(__SPI_REGS[port], true);

    kirq_register(KERNEL_HANDLE, __SPI_VECTORS[port], stm32_spi_on_isr, (void*)exo);
    NVIC_SetPriority(__SPI_VECTORS[port], 13);
=======
    //enable clock
    if (port == SPI_1 )
        RCC->APB2ENR |= 1 << __SPI_POWER_PINS[port];
    else
        RCC->APB1ENR |= 1 << __SPI_POWER_PINS[port];

    spi->cs_pin = cs_pin;
    cr1 = 0;
    if (mode & SPI_LSBFIRST_MSK)
        cr1 |= SPI_CR1_LSBFIRST;
    if (mode & SPI_CPOL_MSK)
        cr1 |= SPI_CR1_CPOL;
    if (mode & SPI_CPHA_MSK)
        cr1 |= SPI_CR1_CPHA;

    __SPI_REGS[port]->CR1 |= ((mode & 0x07) << 3) | SPI_CR1_MSTR;
    __SPI_REGS[port]->CR1 |= cr1;
    __SPI_REGS[port]->CR2 = (mode & 0x0F00) | SPI_CR2_SSOE | SPI_CR2_FRXTH;
    __SPI_REGS[port]->CR1 |= SPI_CR1_SPE;
>>>>>>> d5527e34669581874a0d7b650984fa2e7ed767a9
}

void stm32_spi_close(EXO* exo, SPI_PORT port)
{
    SPI* spi = exo->spi.spis[port];
    if (!spi)
    {
        kerror(ERROR_NOT_CONFIGURED);
        return;
    }
<<<<<<< HEAD
    kirq_unregister(KERNEL_HANDLE, __SPI_VECTORS[port]);
    stm32_spi_enable(__SPI_REGS[port], false);
    // Disable clocking
    switch(port)
    {
        case SPI_1:
            RCC->APB2ENR &= ~RCC_APB2ENR_SPI1EN;
            break;
        case SPI_2:
            RCC->APB1ENR &= ~RCC_APB1ENR_SPI2EN;
            break;
        default:
        {
            error(ERROR_HARDWARE);
            return;
        }
    }
=======
    __SPI_REGS[port]->CR1 &= ~SPI_CR1_SPE;
    if (port == SPI_1)
        RCC->APB2ENR &= ~(1 << __SPI_POWER_PINS[port]);
    else
        RCC->APB1ENR &= ~(1 << __SPI_POWER_PINS[port]);
>>>>>>> d5527e34669581874a0d7b650984fa2e7ed767a9

    spi->io = NULL;
    kfree(spi);
    exo->spi.spis[port] = NULL;
}

void stm32_spi_byte(EXO* exo, IPC* ipc)
{
<<<<<<< HEAD
    SPI_PORT port = (SPI_PORT)ipc->param1;
    uint8_t byte = (uint8_t)ipc->param2;

    SPI* spi = exo->spi.spis[port];
    if (spi == NULL)
    {
        error(ERROR_NOT_CONFIGURED);
        return;
    }

    __SPI_REGS[port]->DR = byte;
    while(!(__SPI_REGS[port]->SR & SPI_SR_RXNE));
    byte = __SPI_REGS[port]->DR;
    ipc->param2 = byte;
=======
//    *(uint8_t *)&__SPI_REGS[port]->DR = (uint8_t)data;
    __SPI_REGS[port]->DR = data;
    while ((__SPI_REGS[port]->SR & SPI_SR_RXNE) == 0);
    return __SPI_REGS[port]->DR;
>>>>>>> d5527e34669581874a0d7b650984fa2e7ed767a9
}

static void stm32_spi_data_io(EXO* exo, IPC* ipc, bool read)
{
<<<<<<< HEAD
    SPI_PORT port = (SPI_PORT)ipc->param1;
=======
// TODO: 8bit access if data len  <=8 bit
    uint32_t size = (ipc->param1 >> 8) & 0xFF;
>>>>>>> d5527e34669581874a0d7b650984fa2e7ed767a9
    SPI* spi = exo->spi.spis[port];
    uint32_t size = ipc->param3;

    if (spi == NULL)
    {
        kerror(ERROR_NOT_CONFIGURED);
        return;
    }
<<<<<<< HEAD
=======
    ipc->param3 = spi_write(port, ipc->param3);
    if (size < 2)
        return;
    ipc->param2 = spi_write(port, ipc->param2);
    if (size < 3)
        return;
    ipc->param1 = spi_write(port, ipc->param1 >> 16);
>>>>>>> d5527e34669581874a0d7b650984fa2e7ed767a9

    spi->process = ipc->process;
    spi->io = (IO*)ipc->param2;
    spi->rx_length = spi->tx_length = 0;
    if(read)
    {
        spi->mode = SPI_MODE_RX;
        spi->io->data_size = 0;
        spi->rx_length = size;
    }
    else
    {
        spi->mode = SPI_MODE_TX;
        spi->tx_length = spi->io->data_size;
    }

//    if(spi->tx_length > max_size)
//        spi->tx_length = max_size;

    if(read)
        __SPI_REGS[port]->DR = 0x00;
    else
        __SPI_REGS[port]->DR = *(uint8_t*)io_data(spi->io);


    __SPI_REGS[port]->CR2 |= SPI_CR2_TXEIE;
    if(read)
        __SPI_REGS[port]->CR2 |= SPI_CR2_RXNEIE;
    //all rest in isr
    NVIC_EnableIRQ(__SPI_VECTORS[port]);
    error(ERROR_SYNC);
}

void stm32_spi_request(EXO* exo, IPC* ipc)
{
    SPI_PORT port = (SPI_PORT)ipc->param1;
    if (port >= SPI_COUNT)
    {
        kerror(ERROR_INVALID_PARAMS);
        return;
    }

    switch (HAL_ITEM(ipc->cmd))
    {
<<<<<<< HEAD
        case IPC_OPEN:
            stm32_spi_open(exo, port, ipc->param2);
            break;
        case IPC_CLOSE:
            stm32_spi_close(exo, port);
            break;
        case IPC_WRITE:
            stm32_spi_data_io(exo, ipc, false);
            break;
        case IPC_READ:
            stm32_spi_data_io(exo, ipc, true);
            break;
        case SPI_BYTE:
            stm32_spi_byte(exo, ipc);
            break;
        default:
            error(ERROR_NOT_SUPPORTED);
            break;
=======
    case IPC_OPEN:
        stm32_spi_open(exo, port, ipc->param2, ipc->param3);
        break;
    case IPC_CLOSE:
        stm32_spi_close(exo, port);
    case IPC_WRITE:
        stm32_spi_write(exo, ipc, port);
        break;
    default:
        kerror(ERROR_NOT_SUPPORTED);
        break;
>>>>>>> d5527e34669581874a0d7b650984fa2e7ed767a9
    }
}
