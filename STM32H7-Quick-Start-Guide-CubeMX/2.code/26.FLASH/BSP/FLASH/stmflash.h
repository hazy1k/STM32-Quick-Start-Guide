#ifndef __STMFLASH_H
#define __STMFLASH_H

#include "main.h"

/* FLASH起始地址 */
#define STM32_FLASH_BASE        0x08000000      /* STM32 FLASH的起始地址 */
#define STM32_FLASH_SIZE        0x20000         /* STM32 FLASH总大小 */
#define BOOT_FLASH_SIZE         0x4000          /* 前16K FLASH用于保存BootLoader */
#define FLASH_WAITETIME         50000           /* FLASH等待超时时间 */

/* FLASH 扇区的起始地址,H750xx只有BANK1的扇区0有效,共128KB */
#define BANK1_FLASH_SECTOR_0    ((uint32_t)0x08000000)  /* Bank1扇区0起始地址, 128 Kbytes  */

uint32_t stmflash_read_word(uint32_t faddr);    /* 读取一个字(4字节) */
void stmflash_write(uint32_t waddr, uint32_t *pbuf, uint32_t length);   /* 指定地址开始写入指定长度的数据 */
void stmflash_read(uint32_t raddr, uint32_t *pbuf, uint32_t length);    /* 从指定地址开始读出指定长度的数据 */

/* 测试写入 */
void test_write(uint32_t waddr, uint32_t wdata);

#endif //__STMFLASH_H
