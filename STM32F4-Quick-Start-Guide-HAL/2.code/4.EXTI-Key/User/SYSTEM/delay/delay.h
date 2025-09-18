/**
 ****************************************************************************************************
 * @file        delay.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2021-10-14
 * @brief       使用SysTick的普通计数模式对延迟进行管理(支持ucosii)
 *              提供delay_init初始化函数， delay_us和delay_ms等延时函数
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台：正点原子 探索者 F407开发板
 * 在线视频：www.yuanzige.com
 * 技术论坛：http://www.openedv.com/forum.php
 * 公司网址：www.alientek.com
 * 购买地址：zhengdianyuanzi.tmall.com
 *
 * 修改说明
 * V1.0 20211014
 * 第一次发布
 *
 ****************************************************************************************************
 */
 
#ifndef __DELAY_H
#define __DELAY_H

#include "./SYSTEM/sys/sys.h"


void delay_init(uint16_t sysclk);           /* 初始化延迟函数 */
void delay_ms(uint16_t nms);                /* 延时nms */
void delay_us(uint32_t nus);                /* 延时nus */

#if (!SYS_SUPPORT_OS)                       /* 没有使用Systick中断 */
    void HAL_Delay(uint32_t Delay);         /* HAL库的延时函数，SDIO等需要用到 */
#endif

#endif

