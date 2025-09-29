/**
 ****************************************************************************************************
 * @file        norflash_ex.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-03-29
 * @brief       QSPI 驱动代码
 *              因为STM32H7不支持QSPI接口读时写,因此该代码用于实现QSPI FLASH的数据写入,原理是：
 *              qspi.c、norflash.c和norflash_ex.c等3部分代码全部存储在H7的内部FLASH,保证操作
 *              QSPI FLASH的时候,不需要访问QSPI FLASH的代码. 从而实现QSPI FLASH数据写入.
 *
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 STM32H750开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.0 20200329
 * 第一次发布
 *
 ****************************************************************************************************
 */
 
#ifndef __NORFLASH_EX_H
#define __NORFLASH_EX_H

#include "./SYSTEM/sys/sys.h"


void norflash_ex_erase_chip(void);              /* NOR FLASH 全片擦除 */
uint16_t norflash_ex_read_id(void);             /* NOR FLASH读取ID */
void norflash_ex_erase_sector(uint32_t addr);   /* NOR FLASH 擦除扇区 */
uint8_t norflash_ex_write(uint8_t *pbuf, uint32_t addr, uint16_t datalen);  /* NOR FLASH写入数据 */
void norflash_ex_read(uint8_t *pbuf, uint32_t addr, uint16_t datalen);      /* NOR FLASH读取数据 */

#endif















