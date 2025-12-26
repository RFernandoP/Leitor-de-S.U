/*
 * =============================================================================
 * Nome do Arquivo  : main.c
 * Projeto          : Leitor de memória FLASH W25Q64
 * Descrição        : Breve descrição da finalidade do código.
 *
 * Autor            : Seu Nome
 * Data de Criação  : AAAA-MM-DD
 * Última Modificação: AAAA-MM-DD
 * Versão           : v1.0.0
 *
 * Placa/MCU        : ATmega328P
 * Clock            : 16 MHz
 * Compiler         : avr-gcc 13.4.0
 *
 * Histórico de Versões:
 *   v1.0.0 - AAAA-MM-DD - [Seu Nome] - Primeira versão funcional.
 *   v1.1.0 - AAAA-MM-DD - [Seu Nome] - Pequenas melhorias e correções.
 *
 * Observações:
 *   - Dependências: uart.h, spi.h, w25q64.h
 *   - Este código é parte do projeto XXXXXXX.
 *
 * Licença: MIT / GPL / Proprietária (especificar conforme necessário)
 * =============================================================================
 */


#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdio.h>
#include "uart.h"
#include "spi.h"
#include "w25q64.h"

#define BAUD 57600
#define MY_UBRR (F_CPU / 16 / BAUD - 1)

#define SECTOR_SIZE 4096
#define CHUNK_SIZE 1024

// Posso ler um setor de 4 KB (4096 bytes)
// Micro tem apenas 2KB de SRAM, o que impossibilita a leitura instantânea dos 4 KB. Fazer por pedaços (chunks), 512 BYTES.
// Evitar declarar arrays grandes na stack, usar static porque aloca na RAM do micro ao invés da pilha.


// SECTOR_SIZE 4096 
// CHUNK_SIZE 512

int main(void) {

 static uint8_t buffer[256];

    uart_init(MY_UBRR);
    W25Q64_init();
    _delay_ms(100);

    //uart_write_str("W25Q64 ID: ");
    //uint8_t id = W25Q64_readID();

    char buf[8];  // segurança extra
    //snprintf(buf, sizeof(buf), "%02X\r\n", id);
    //uart_write_str(buf);

    // Opção 1 — usar sua função dumpHex:
    W25Q64_dumpHex(0x000000, 0X8000);


    uint32_t address = 0x000000;
    for (uint16_t offset = 0; offset < SECTOR_SIZE; offset += CHUNK_SIZE) {
        
         W25Q64_readData(address + offset, buffer, CHUNK_SIZE);
        // processa ou envia buffer por UART, grava em SD, etc.
    }
    
    while (1) {
        // loop infinito
    }

}
//____________________________________________________________________________
/*    
W25Q64_init();
W25Q64_writeEnable();
W25Q64_eraseSector(0x0000);

}
*/