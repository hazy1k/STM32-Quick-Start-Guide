/**
 ****************************************************************************************************
 * @file        atim.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-03-19
 * @brief       高级定时器 驱动代码
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
 * V1.0 20200319
 * 第一次发布 
 *
 ****************************************************************************************************
 */

#ifndef __ATIM_H
#define __ATIM_H

#include "./SYSTEM/sys/sys.h"

/******************************************************************************************/
/* 高级定时器 定义 */


/* TIMX 输出比较模式 定义 
 * 这里通过TIM1的输出比较模式,控制PE9,PE11,PE13,PE14输出4路PWM,占空比50%,并且每一路PWM之间的相位差为25%
 * 修改CCRx可以修改相位.
 * 默认是针对TIM1/TIM8
 * 注意: 通过修改这些宏定义,可以支持TIM1~TIM17任意一个定时器,任意一个IO口使用输出比较模式,输出PWM
 */
#define ATIM_TIMX_COMP_CH1_GPIO_PORT            GPIOE
#define ATIM_TIMX_COMP_CH1_GPIO_PIN             SYS_GPIO_PIN9
#define ATIM_TIMX_COMP_CH1_GPIO_AF              1                           /* AF功能选择 */
#define ATIM_TIMX_COMP_CH1_GPIO_CLK_ENABLE()    do{ RCC->AHB4ENR |= 1 << 4; }while(0)   /* PE口时钟使能 */

#define ATIM_TIMX_COMP_CH2_GPIO_PORT            GPIOE
#define ATIM_TIMX_COMP_CH2_GPIO_PIN             SYS_GPIO_PIN11
#define ATIM_TIMX_COMP_CH2_GPIO_AF              1                           /* AF功能选择 */
#define ATIM_TIMX_COMP_CH2_GPIO_CLK_ENABLE()    do{ RCC->AHB4ENR |= 1 << 4; }while(0)   /* PE口时钟使能 */

#define ATIM_TIMX_COMP_CH3_GPIO_PORT            GPIOE
#define ATIM_TIMX_COMP_CH3_GPIO_PIN             SYS_GPIO_PIN13
#define ATIM_TIMX_COMP_CH3_GPIO_AF              1                           /* AF功能选择 */
#define ATIM_TIMX_COMP_CH3_GPIO_CLK_ENABLE()    do{ RCC->AHB4ENR |= 1 << 4; }while(0)   /* PE口时钟使能 */

#define ATIM_TIMX_COMP_CH4_GPIO_PORT            GPIOE
#define ATIM_TIMX_COMP_CH4_GPIO_PIN             SYS_GPIO_PIN14
#define ATIM_TIMX_COMP_CH4_GPIO_AF              1                           /* AF功能选择 */
#define ATIM_TIMX_COMP_CH4_GPIO_CLK_ENABLE()    do{ RCC->AHB4ENR |= 1 << 4; }while(0)   /* PE口时钟使能 */


#define ATIM_TIMX_COMP                          TIM1            
#define ATIM_TIMX_COMP_CH1_CCRX                 ATIM_TIMX_COMP->CCR1        /* 通道1的输出比较寄存器 */
#define ATIM_TIMX_COMP_CH2_CCRX                 ATIM_TIMX_COMP->CCR2        /* 通道2的输出比较寄存器 */
#define ATIM_TIMX_COMP_CH3_CCRX                 ATIM_TIMX_COMP->CCR3        /* 通道3的输出比较寄存器 */
#define ATIM_TIMX_COMP_CH4_CCRX                 ATIM_TIMX_COMP->CCR4        /* 通道4的输出比较寄存器 */
#define ATIM_TIMX_COMP_CLK_ENABLE()             do{ RCC->APB2ENR |= 1 << 0; }while(0)   /* TIM1 时钟使能 */
 
/******************************************************************************************/


void atim_timx_comp_pwm_init(uint16_t arr, uint16_t psc);   /* 高级定时器 输出比较模式输出PWM 初始化函数 */

#endif

















