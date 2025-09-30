/**
 ****************************************************************************************************
 * @file        btim.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-03-14
 * @brief       基本定时器 驱动代码
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
 * V1.0 20200314
 * 第一次发布
 *
 ****************************************************************************************************
 */

#ifndef __BTIM_H
#define __BTIM_H

#include "./SYSTEM/sys/sys.h"


/******************************************************************************************/
/* 基本定时器 定义 */

/* TIMX 中断定义 
 * 默认是针对TIM2~TIM5, TIM12~TIM17.
 * 注意: 通过修改这4个宏定义,可以支持TIM1~TIM17任意一个定时器.
 */
 
#define BTIM_TIM3_INT                       TIM3
#define BTIM_TIM3_INT_IRQn                  TIM3_IRQn
#define BTIM_TIM3_INT_IRQHandler            TIM3_IRQHandler
#define BTIM_TIM3_INT_CLK_ENABLE()          do{ __HAL_RCC_TIM3_CLK_ENABLE(); }while(0)  /* TIM3 时钟使能 */

#define BTIM_TIM5_INT                       TIM5
#define BTIM_TIM5_INT_IRQn                  TIM5_IRQn
#define BTIM_TIM5_INT_IRQHandler            TIM5_IRQHandler
#define BTIM_TIM5_INT_CLK_ENABLE()          do{ __HAL_RCC_TIM5_CLK_ENABLE(); }while(0)  /* TIM5 时钟使能 */

/******************************************************************************************/

void btim_tim3_int_init(uint16_t arr, uint16_t psc);
void btim_tim5_int_init(uint16_t arr, uint16_t psc);

#endif

















