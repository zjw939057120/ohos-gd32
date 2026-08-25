

#ifndef _BSP_W25Q64_H__
#define _BSP_W25Q64_H__

#include "gd32f4xx.h"

void w25q64_init_config(void);
uint16_t W25Q64_readID(void);
void W25Q64_write(uint8_t* buffer, uint32_t addr, uint16_t numbyte);
void W25Q64_read(uint8_t* buffer,uint32_t read_addr,uint16_t read_length) ;
#endif
