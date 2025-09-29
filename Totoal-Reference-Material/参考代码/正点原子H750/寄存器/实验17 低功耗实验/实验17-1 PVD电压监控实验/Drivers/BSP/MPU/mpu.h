/**
 ****************************************************************************************************
 * @file        mpu.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-03-22
 * @brief       MPU内存保护 驱动代码
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
 * V1.0 20200322
 * 第一次发布
 *
 ****************************************************************************************************
 */

#ifndef __MPU_H
#define __MPU_H

#include "./SYSTEM/sys/sys.h"


/* MPU的详细设置关系，请看：《STM32H7编程手册.pdf》
 * 这个文档的6.6节,Table 91.
 * MPU保护区域许可属性定义（拷贝自stm32h7xx_hal_cortex.h）
 * 定义MPU->RASR寄存器AP[26:24]位的设置值
 */ 
#define  MPU_REGION_NO_ACCESS       ((uint8_t)0x00U)        /* 无访问（特权&用户都不可访问） */
#define  MPU_REGION_PRIV_RW         ((uint8_t)0x01U)        /* 仅支持特权读写访问 */
#define  MPU_REGION_PRIV_RW_URO     ((uint8_t)0x02U)        /* 禁止用户写访问（特权可读写访问） */
#define  MPU_REGION_FULL_ACCESS     ((uint8_t)0x03U)        /* 全访问（特权&用户都可访问） */
#define  MPU_REGION_PRIV_RO         ((uint8_t)0x05U)        /* 仅支持特权读访问 */
#define  MPU_REGION_PRIV_RO_URO     ((uint8_t)0x06U)        /* 只读（特权&用户都不可以写） */

static uint8_t mpu_convert_bytes_to_pot(uint32_t nbytes);   /* 将nbytes转换为2为第的指数 */

void mpu_disable(void); /* 禁止MPU */
void mpu_enable(void);  /* 使能MPU */
void mpu_memory_protection(void);   /* MPU保护 */
uint8_t mpu_set_protection(uint32_t baseaddr, uint32_t size, uint32_t rnum, uint8_t de, uint8_t ap, uint8_t sen, uint8_t cen, uint8_t ben);

#endif

















