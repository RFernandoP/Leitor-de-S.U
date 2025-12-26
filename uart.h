#ifndef UART_H
#define UART_H

#include <avr/io.h>

void uart_init(uint16_t ubrr);
void uart_write(uint8_t data);
void uart_write_str(const char *str);

#endif