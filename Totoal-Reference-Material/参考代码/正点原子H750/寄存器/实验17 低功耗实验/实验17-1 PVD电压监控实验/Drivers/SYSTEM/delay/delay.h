/**
 ****************************************************************************************************
 * @file        delay.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.1
 * @date        2023-02-27
 * @brief       使用SysTick的普通计数模式对延迟进行管理(支持OS)
 *              提供delay_init初始化函数， delay_us和delay_ms等延时函数
 * @license     Copyright (c) 2022-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 STM32开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.0 20221222
 * 第一次发布
 * V1.1 20230227
 * 修改SYS_SUPPORT_OS部分代码, 默认仅支持UCOSII 2.93.01版本, 其他OS请参考实现
 * 修改delay_init不再使用8分频,全部统一使用MCU时钟
 * 修改delay_us使用时钟摘取法延时, 兼容OS
 * 修改delay_ms直接使用delay_us延时实现.
 *
 ****************************************************************************************************
 */

#ifndef __DELAY_H
#define __DELAY_H

#include "./SYSTEM/sys/sys.h"


void delay_init(uint16_t sysclk);   /* 初始化延迟函数 */
void delay_ms(uint16_t nms);        /* 延时nms */
void delay_us(uint32_t nus);        /* 延时nus */

#endif





























