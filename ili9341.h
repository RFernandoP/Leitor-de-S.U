#ifndef ILI9341_H
#define ILI9341_H

#include <avr/io.h>
#include <util/delay.h>

void ili9341_init(void);
void ili9341_fill_screen(uint16_t color);
void ili9341_draw_pixel(uint16_t x, uint16_t y, uint16_t color);

#endif