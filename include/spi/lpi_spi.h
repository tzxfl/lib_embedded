#ifndef LPI_SPI_H
#define LPI_SPI_H

#include "lpi_types.h"
DECLAR_HANDLE(HSPI);

typedef enum
{
    SPI_MODE0,
    SPI_MODE1,
    SPI_MODE2,
    SPI_MODE3
}SpiMode;

typedef struct
{
    char spi_dev[32];
    unsigned char spi_mode;
    unsigned int spi_speed;
    unsigned char spi_bit_per_word;
}SpiParam;

HSPI spi_open(const SpiParam *param);

int spi_write_read(unsigned char *data,int len);

int

#endif // LPI_SPI_H
