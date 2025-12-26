//w25q64.c

#include "w25q64.h"
#include "spi.h"
#include "uart.h"
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdio.h>

#define CMD_READ_ID       0x90
#define CMD_READ_DATA     0x03
#define CMD_WRITE_ENABLE  0x06

#define CMD_PAGE_PROGRAM    0x02
#define CMD_READ_STATUS     0x05
#define CMD_SECTOR_ERASE_4K 0X20
#define CMD_BLCK_ERASE_32K  0X52
#define CMD_BLCK_ERASE_64K  0XD8
#define CMD_CHIP_ERASE      0xC7  // também pode usar 0x60

/*************************************************************************************/
void W25Q64_init(void) {
    SPI_init();
    FLASH_CS_HIGH();
}
/*************************************************************************************/
void W25Q64_writeEnable(void) {
    FLASH_CS_LOW();
    SPI_transfer(CMD_WRITE_ENABLE);
    FLASH_CS_HIGH();
    _delay_ms(1);
}
/*************************************************************************************/
uint8_t W25Q64_readID(void) {
    uint8_t id = 0;

    FLASH_CS_LOW();
    SPI_transfer(CMD_READ_ID);
    SPI_transfer(0x00);  // dummy
    SPI_transfer(0x00);
    SPI_transfer(0x00);
    id = SPI_transfer(0xFF);
    FLASH_CS_HIGH();

    return id;
}
/*************************************************************************************/
void W25Q64_readData(uint32_t addr, uint8_t *buffer, uint16_t len) {
    FLASH_CS_LOW();
    SPI_transfer(CMD_READ_DATA);
    SPI_transfer((addr >> 16) & 0xFF);
    SPI_transfer((addr >> 8) & 0xFF);
    SPI_transfer(addr & 0xFF);
    for (uint16_t i = 0; i < len; i++) {
        buffer[i] = SPI_transfer(0xFF);
    }
    FLASH_CS_HIGH();
}
/*************************************************************************************/
uint8_t W25Q64_readStatus(void) {
    uint8_t status;
    FLASH_CS_LOW();
    SPI_transfer(CMD_READ_STATUS);
    status = SPI_transfer(0xFF);
    FLASH_CS_HIGH();
    return status;
}
/*************************************************************************************/
void W25Q64_waitBusy(void) {
    while (W25Q64_readStatus() & 0x01);  // Bit 0: Write-In-Progress
}
/*************************************************************************************/
void W25Q64_pageProgram(uint32_t addr, const uint8_t *data, uint16_t len) {
    if (len > 256) len = 256; // máximo por página

    W25Q64_writeEnable();
    FLASH_CS_LOW();

    SPI_transfer(CMD_PAGE_PROGRAM);
    SPI_transfer((addr >> 16) & 0xFF);
    SPI_transfer((addr >> 8) & 0xFF);
    SPI_transfer(addr & 0xFF);

    for (uint16_t i = 0; i < len; i++) {
        SPI_transfer(data[i]);
    }

    FLASH_CS_HIGH();
    W25Q64_waitBusy(); // Espera até terminar
}
/*************************************************************************************/
void W25Q64_writeData(uint32_t addr, const uint8_t *data, uint32_t len) {
    uint16_t bytesToWrite;

    while (len > 0) {
        // Calcula o quanto pode escrever até o fim da página atual
        bytesToWrite = 256 - (addr % 256);
        if (bytesToWrite > len) {
            bytesToWrite = len;
        }

        W25Q64_pageProgram(addr, data, bytesToWrite);

        addr += bytesToWrite;
        data += bytesToWrite;
        len  -= bytesToWrite;
    }
}
/*************************************************************************************/
void W25Q64_dumpHex(uint32_t addr, uint16_t len) {
    uint8_t buffer[16];
    char linha[128];

    for (uint16_t i = 0; i < len; i += 16) {
        uint8_t linha_len = (len - i) >= 16 ? 16 : (len - i);

        W25Q64_readData(addr + i, buffer, linha_len);

        // Imprime o endereço base da linha
        sprintf(linha, "%06lX: ", addr + i);

        // Adiciona os bytes em hexadecimal
        for (uint8_t j = 0; j < linha_len; j++) {
            char temp[4];
            sprintf(temp, "%02X ", buffer[j]);
            strcat(linha, temp);
        }

        // Espaçamento se for linha incompleta
        for (uint8_t j = linha_len; j < 16; j++) {
            strcat(linha, "   ");
        }

        //strcat(linha, " | ");

        // Adiciona os caracteres imprimíveis (ou ponto)
        for (uint8_t j = 0; j < linha_len; j++) {
            char ch = buffer[j];
            strcat(linha, (ch >= 32 && ch <= 126) ? (char[]){ch, 0} : ".");
        }

        //strcat(linha, "\r\n");

        uart_write_str(linha);
    }
}
/*************************************************************************************/
void W25Q64_eraseSector(uint32_t address) {
    // Endereço deve estar alinhado em 4 KB (0x1000)
    address &= 0xFFFFF000;  

    W25Q64_writeEnable();

    FLASH_CS_LOW();
    SPI_transfer(CMD_SECTOR_ERASE_4K);
    SPI_transfer((address >> 16) & 0xFF);
    SPI_transfer((address >> 8) & 0xFF);
    SPI_transfer(address & 0xFF);
    FLASH_CS_HIGH();

    W25Q64_waitBusy(); // aguarda até terminar
}
/*************************************************************************************/
void W25Q64_eraseBlock32K(uint32_t address) {
    // Endereço deve estar alinhado em 32 KB (0x8000)
    address &= 0xFFFF8000;

    W25Q64_writeEnable();

    FLASH_CS_LOW();
    SPI_transfer(CMD_BLCK_ERASE_32K);
    SPI_transfer((address >> 16) & 0xFF);
    SPI_transfer((address >> 8) & 0xFF);
    SPI_transfer(address & 0xFF);
    FLASH_CS_HIGH();

    W25Q64_waitBusy(); // aguarda até terminar
}
/*************************************************************************************/
void W25Q64_eraseBlock64K(uint32_t address) {
    // Endereço deve estar alinhado em 64 KB (0x10000)
    address &= 0xFFFE0000;

    W25Q64_writeEnable();

    FLASH_CS_LOW();
    SPI_transfer(CMD_BLCK_ERASE_64K);
    SPI_transfer((address >> 16) & 0xFF);
    SPI_transfer((address >> 8) & 0xFF);
    SPI_transfer(address & 0xFF);
    FLASH_CS_HIGH();

    W25Q64_waitBusy(); // aguarda até terminar
}
/*************************************************************************************/
void W25Q64_chipErase(void) {
    W25Q64_writeEnable();

    FLASH_CS_LOW();
    SPI_transfer(CMD_CHIP_ERASE);
    FLASH_CS_HIGH();

    W25Q64_waitBusy(); // espera até apagar tudo (~20 a 100s)
}
