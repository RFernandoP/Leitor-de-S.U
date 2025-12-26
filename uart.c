#include <stdio.h>
#include "uart.h"

void uart_init(uint16_t ubrr) {
    // Define baud rate
    UBRR0H = (ubrr >> 8);
    UBRR0L = ubrr;

    // Habilita transmissor
    UCSR0B = (1 << TXEN0);

    // Modo assíncrono, 8 bits, 1 stop bit, sem paridade
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void uart_write(uint8_t data) {
    // Espera até o buffer estar vazio
    while (!(UCSR0A & (1 << UDRE0)));
    // Envia o dado
    UDR0 = data;
}

void uart_write_str(const char *str) {
    while (*str) {
        uart_write(*str++);
    }
}

