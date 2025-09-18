#ifndef __FLASH_EX_H
#define __FLASH_EX_H

#include "main.h"

void norflash_ex_erase_chip(void);              /* NOR FLASH 全片擦除 */
uint16_t norflash_ex_read_id(void);             /* NOR FLASH读取ID */
void norflash_ex_erase_sector(uint32_t addr);   /* NOR FLASH 擦除扇区 */
uint8_t norflash_ex_write(uint8_t *pbuf, uint32_t addr, uint16_t datalen);  /* NOR FLASH写入数据 */
void norflash_ex_read(uint8_t *pbuf, uint32_t addr, uint16_t datalen);      /* NOR FLASH读取数据 */

#endif /* __FLASH_EX_H */
