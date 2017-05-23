#ifndef __SPI_H__
#define __SPI_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

#define SPI_CLOCK_DIV1   1
#define SPI_CLOCK_DIV2   2
#define SPI_CLOCK_DIV4   4
#define SPI_CLOCK_DIV8   8
#define SPI_CLOCK_DIV16  16
#define SPI_CLOCK_DIV32  32
#define SPI_CLOCK_DIV64  64
#define SPI_CLOCK_DIV128 128

void spi_initialize(void);
void spi_disable(void);
uint8_t spi_send(const uint8_t);
void spi_set_bitorder(const uint8_t);
void spi_set_datamode(const uint8_t);
void spi_set_divisor(const uint16_t clkdivider);

#ifdef __cplusplus
}
#endif

#endif //__SPI_H__
