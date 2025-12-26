//spi.c

#include "spi.h"

void SPI_init(void) {
    // SCK (PB5), MOSI (PB3), SS (PB2) como saída
    DDRB |= (1 << PB5) | (1 << PB3) | (1 << PB2);
    // MISO (PB4) como entrada
    DDRB &= ~(1 << PB4);

    // Habilita SPI, modo master, clock = fosc/16
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
}

uint8_t SPI_transfer(uint8_t data) {
    SPDR = data;                         // Envia dado
    while (!(SPSR & (1 << SPIF)));       // Aguarda envio completo
    return SPDR;                         // Retorna byte recebido
}