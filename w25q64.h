//w25q64.h

#ifndef W25Q64_H
#define W25Q64_H

#include <stdint.h>

#define FLASH_CS_LOW()   (PORTB &= ~(1 << PB2))  // SS = PB2
#define FLASH_CS_HIGH()  (PORTB |= (1 << PB2))



void W25Q64_pageProgram(uint32_t addr, const uint8_t *data, uint16_t len);
uint8_t W25Q64_readStatus(void);
void W25Q64_waitBusy(void);
void W25Q64_eraseSector(uint32_t address);
void W25Q64_init(void);
uint8_t W25Q64_readID(void);
void W25Q64_readData(uint32_t addr, uint8_t *buffer, uint16_t len);
void W25Q64_writeEnable(void);
void W25Q64_writeData(uint32_t addr, const uint8_t *data, uint32_t len);
void W25Q64_dumpHex(uint32_t addr, uint16_t len);


#endif