//spi.h

#ifndef SPI_H
#define SPI_H

#include <avr/io.h>

void SPI_init(void);
uint8_t SPI_transfer(uint8_t data);

#endif


