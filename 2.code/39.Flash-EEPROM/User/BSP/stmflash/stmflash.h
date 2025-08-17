#ifndef __STMFLASH_H
#define __STMFLASH_H

#include "sys.h"

/* FLASH起始地址 */
#define STM32_FLASH_SIZE 0x100000  // STM32 FLASH 总大小
#define STM32_FLASH_BASE 0X08000000 // STM32 FLASH 起始地址
#define FLASH_WATTETIME 5000 // FLASH等待超时时间 

/* FLASH 扇区的起始地址 */
#define ADDR_FLASH_SECTOR_0     ((uint32_t )0x08000000)     /* 扇区0起始地址, 16 Kbytes */
#define ADDR_FLASH_SECTOR_1     ((uint32_t )0x08004000)     /* 扇区1起始地址, 16 Kbytes */
#define ADDR_FLASH_SECTOR_2     ((uint32_t )0x08008000)     /* 扇区2起始地址, 16 Kbytes */
#define ADDR_FLASH_SECTOR_3     ((uint32_t )0x0800C000)     /* 扇区3起始地址, 16 Kbytes */
#define ADDR_FLASH_SECTOR_4     ((uint32_t )0x08010000)     /* 扇区4起始地址, 64 Kbytes */
#define ADDR_FLASH_SECTOR_5     ((uint32_t )0x08020000)     /* 扇区5起始地址, 128 Kbytes */
#define ADDR_FLASH_SECTOR_6     ((uint32_t )0x08040000)     /* 扇区6起始地址, 128 Kbytes */
#define ADDR_FLASH_SECTOR_7     ((uint32_t )0x08060000)     /* 扇区7起始地址, 128 Kbytes */
#define ADDR_FLASH_SECTOR_8     ((uint32_t )0x08080000)     /* 扇区8起始地址, 128 Kbytes */
#define ADDR_FLASH_SECTOR_9     ((uint32_t )0x080A0000)     /* 扇区9起始地址, 128 Kbytes */
#define ADDR_FLASH_SECTOR_10    ((uint32_t )0x080C0000)     /* 扇区10起始地址,128 Kbytes */
#define ADDR_FLASH_SECTOR_11    ((uint32_t )0x080E0000)     /* 扇区11起始地址,128 Kbytes */

uint32_t stmflash_read_word(uint32_t faddr);
uint8_t  stmflash_get_flash_sector(uint32_t addr);
void stmflash_write(uint32_t waddr, uint32_t *pbuf, uint32_t length);
void stmflash_read(uint32_t raddr, uint32_t *pbuf, uint32_t length);
void stmflash_erase_sector(uint8_t sector);

#endif /* __STMFLASH_H__ */
