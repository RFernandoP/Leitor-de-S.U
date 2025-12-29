#include "ili9341.h"

// Pinos definidos conforme ATmega328P (ajuste conforme necessário)
#define DC_PIN     PD6
#define RST_PIN    PD7
#define CS_PIN     PB2

#define DC_HIGH()  PORTD |= (1 << DC_PIN)
#define DC_LOW()   PORTD &= ~(1 << DC_PIN)
#define RST_HIGH() PORTD |= (1 << RST_PIN)
#define RST_LOW()  PORTD &= ~(1 << RST_PIN)
#define CS_HIGH()  PORTB |= (1 << CS_PIN)
#define CS_LOW()   PORTB &= ~(1 << CS_PIN)

#define WIDTH  240
#define HEIGHT 320

static void spi_init() {
    DDRB |= (1 << PB3) | (1 << PB5) | (1 << CS_PIN); // MOSI, SCK, CS como saída
    SPCR = (1 << SPE) | (1 << MSTR); // SPI habilitado, mestre
    SPSR = (1 << SPI2X); // Fosc/2
}

static void spi_send(uint8_t data) {
    SPDR = data;
    while (!(SPSR & (1 << SPIF)));
}

static void lcd_command(uint8_t cmd) {
    DC_LOW();
    CS_LOW();
    spi_send(cmd);
    CS_HIGH();
}

static void lcd_data(uint8_t data) {
    DC_HIGH();
    CS_LOW();
    spi_send(data);
    CS_HIGH();
}

static void lcd_data16(uint16_t data) {
    lcd_data(data >> 8);
    lcd_data(data & 0xFF);
}

static void ili9341_reset() {
    DDRD |= (1 << DC_PIN) | (1 << RST_PIN);
    PORTD |= (1 << DC_PIN);

    RST_LOW();
    _delay_ms(10);
    RST_HIGH();
    _delay_ms(120);
}

void ili9341_init() {
    spi_init();
    ili9341_reset();

    lcd_command(0x01); _delay_ms(5); // Software reset

    lcd_command(0x28); // Display OFF

    lcd_command(0xCF);
    lcd_data(0x00); lcd_data(0x83); lcd_data(0x30);

    lcd_command(0xED);
    lcd_data(0x64); lcd_data(0x03); lcd_data(0x12); lcd_data(0x81);

    lcd_command(0xE8);
    lcd_data(0x85); lcd_data(0x01); lcd_data(0x79);

    lcd_command(0xCB);
    lcd_data(0x39); lcd_data(0x2C); lcd_data(0x00); lcd_data(0x34); lcd_data(0x02);

    lcd_command(0xF7); lcd_data(0x20);

    lcd_command(0xEA); lcd_data(0x00); lcd_data(0x00);

    lcd_command(0xC0); lcd_data(0x26); // Power control
    lcd_command(0xC1); lcd_data(0x11);
    lcd_command(0xC5); lcd_data(0x35); lcd_data(0x3E);
    lcd_command(0xC7); lcd_data(0xBE);

    lcd_command(0x36); lcd_data(0x48); // Memory access ctrl
    lcd_command(0x3A); lcd_data(0x55); // Pixel format = 16 bits

    lcd_command(0xB1); lcd_data(0x00); lcd_data(0x1B);
    lcd_command(0xF2); lcd_data(0x08);
    lcd_command(0x26); lcd_data(0x01);

    lcd_command(0xE0); // Positive gamma correction
    lcd_data(0x1F); lcd_data(0x1A); lcd_data(0x18); lcd_data(0x0A); lcd_data(0x0F);
    lcd_data(0x06); lcd_data(0x45); lcd_data(0x87); lcd_data(0x32); lcd_data(0x0A);
    lcd_data(0x07); lcd_data(0x02); lcd_data(0x07); lcd_data(0x05); lcd_data(0x00);

    lcd_command(0xE1); // Negative gamma correction
    lcd_data(0x00); lcd_data(0x25); lcd_data(0x27); lcd_data(0x05); lcd_data(0x10);
    lcd_data(0x09); lcd_data(0x3A); lcd_data(0x78); lcd_data(0x4D); lcd_data(0x05);
    lcd_data(0x18); lcd_data(0x0D); lcd_data(0x38); lcd_data(0x3A); lcd_data(0x1F);

    lcd_command(0xB7); lcd_data(0x07);
    lcd_command(0xB6); lcd_data(0x0A); lcd_data(0x82); lcd_data(0x27);

    lcd_command(0x11); _delay_ms(120); // Exit sleep
    lcd_command(0x29); // Display ON
}

static void ili9341_set_address(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    lcd_command(0x2A);
    lcd_data16(x0); lcd_data16(x1);

    lcd_command(0x2B);
    lcd_data16(y0); lcd_data16(y1);

    lcd_command(0x2C);
}

void ili9341_draw_pixel(uint16_t x, uint16_t y, uint16_t color) {
    if (x >= WIDTH || y >= HEIGHT) return;
    ili9341_set_address(x, y, x, y);
    lcd_data16(color);
}

void ili9341_fill_screen(uint16_t color) {
    ili9341_set_address(0, 0, WIDTH - 1, HEIGHT - 1);
    for (uint32_t i = 0; i < (uint32_t)WIDTH * HEIGHT; i++) {
        lcd_data16(color);
    }
}
