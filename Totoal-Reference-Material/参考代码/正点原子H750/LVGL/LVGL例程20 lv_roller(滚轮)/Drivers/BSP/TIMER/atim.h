/**
 ****************************************************************************************************
 * @file        atim.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.2
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
 * V1.1 20200320
 * 1, 增加atim_timx_cplm_pwm_init函数
 * 2, 增加atim_timx_cplm_pwm_gpio_init函数
 * V1.2 20200321
 * 1, 增加atim_timx_pwmin_chy_process函数
 * 2, 增加atim_timx_pwmin_chy_init函数
 * 3, 增加atim_timx_pwmin_chy_restart等函数
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
#define ATIM_TIMX_COMP_CH1_GPIO_PIN             GPIO_PIN_9
#define ATIM_TIMX_COMP_CH1_GPIO_AF              GPIO_AF1_TIM1                                 /* AF功能选择 */
#define ATIM_TIMX_COMP_CH1_GPIO_CLK_ENABLE()    do{ __HAL_RCC_GPIOE_CLK_ENABLE(); }while(0)   /* PE口时钟使能 */

#define ATIM_TIMX_COMP_CH2_GPIO_PORT            GPIOE
#define ATIM_TIMX_COMP_CH2_GPIO_PIN             GPIO_PIN_11
#define ATIM_TIMX_COMP_CH2_GPIO_AF              GPIO_AF1_TIM1                                 /* AF功能选择 */
#define ATIM_TIMX_COMP_CH2_GPIO_CLK_ENABLE()    do{ __HAL_RCC_GPIOE_CLK_ENABLE(); }while(0)   /* PE口时钟使能 */

#define ATIM_TIMX_COMP_CH3_GPIO_PORT            GPIOE
#define ATIM_TIMX_COMP_CH3_GPIO_PIN             GPIO_PIN_13
#define ATIM_TIMX_COMP_CH3_GPIO_AF              GPIO_AF1_TIM1                                 /* AF功能选择 */
#define ATIM_TIMX_COMP_CH3_GPIO_CLK_ENABLE()    do{ __HAL_RCC_GPIOE_CLK_ENABLE(); }while(0)   /* PE口时钟使能 */

#define ATIM_TIMX_COMP_CH4_GPIO_PORT            GPIOE
#define ATIM_TIMX_COMP_CH4_GPIO_PIN             GPIO_PIN_14
#define ATIM_TIMX_COMP_CH4_GPIO_AF              GPIO_AF1_TIM1                                 /* AF功能选择 */
#define ATIM_TIMX_COMP_CH4_GPIO_CLK_ENABLE()    do{ __HAL_RCC_GPIOE_CLK_ENABLE(); }while(0)   /* PE口时钟使能 */


#define ATIM_TIMX_COMP                          TIM1            
#define ATIM_TIMX_COMP_CH1_CCRX                 TIM_CHANNEL_1        /* 通道1的输出比较寄存器 */
#define ATIM_TIMX_COMP_CH2_CCRX                 TIM_CHANNEL_2        /* 通道2的输出比较寄存器 */
#define ATIM_TIMX_COMP_CH3_CCRX                 TIM_CHANNEL_3        /* 通道3的输出比较寄存器 */
#define ATIM_TIMX_COMP_CH4_CCRX                 TIM_CHANNEL_4        /* 通道4的输出比较寄存器 */
#define ATIM_TIMX_COMP_CLK_ENABLE()             do{ __HAL_RCC_TIM1_CLK_ENABLE(); }while(0)   /* TIM1 时钟使能 */




/* TIMX 互补输出模式 定义 
 * 这里设置互补输出相关硬件配置, CHY即正常输出, CHYN即互补输出
 * 修改CCRx可以修改占空比.
 * 默认是针对TIM1/TIM8
 * 注意: 通过修改这些宏定义,可以支持TIM1~TIM17任意一个定时器,任意一个IO口(前提是该定时器必须有互补输出功能)
 */

/* 输出通道引脚 */
#define ATIM_TIMX_CPLM_CHY_GPIO_PORT            GPIOE
#define ATIM_TIMX_CPLM_CHY_GPIO_PIN             GPIO_PIN_9
#define ATIM_TIMX_CPLM_CHY_GPIO_AF              GPIO_AF1_TIM1                          /* AF功能选择 */
#define ATIM_TIMX_CPLM_CHY_GPIO_CLK_ENABLE()    do{ __HAL_RCC_GPIOE_CLK_ENABLE(); }while(0)   /* PE口时钟使能 */

/* 互补输出通道引脚 */
#define ATIM_TIMX_CPLM_CHYN_GPIO_PORT           GPIOE
#define ATIM_TIMX_CPLM_CHYN_GPIO_PIN            GPIO_PIN_8
#define ATIM_TIMX_CPLM_CHYN_GPIO_AF             GPIO_AF1_TIM1                           /* AF功能选择 */
#define ATIM_TIMX_CPLM_CHYN_GPIO_CLK_ENABLE()   do{ __HAL_RCC_GPIOE_CLK_ENABLE(); }while(0)   /* PE口时钟使能 */

/* 刹车输入引脚 */
#define ATIM_TIMX_CPLM_BKIN_GPIO_PORT           GPIOE
#define ATIM_TIMX_CPLM_BKIN_GPIO_PIN            GPIO_PIN_15
#define ATIM_TIMX_CPLM_BKIN_GPIO_AF             GPIO_AF1_TIM1                           /* AF功能选择 */
#define ATIM_TIMX_CPLM_BKIN_GPIO_CLK_ENABLE()   do{ __HAL_RCC_GPIOE_CLK_ENABLE(); }while(0)   /* PE口时钟使能 */

/* 互补输出使用的定时器 */
#define ATIM_TIMX_CPLM                          TIM1            
#define ATIM_TIMX_CPLM_CHY                      GPIO_AF1_TIM1           
#define ATIM_TIMX_CPLM_CHY_CCRY                 TIM_CHANNEL_1        
#define ATIM_TIMX_CPLM_CLK_ENABLE()             do{ __HAL_RCC_TIM1_CLK_ENABLE(); }while(0)   /* TIM1 时钟使能 */




/* TIMX PWM输入模式 定义 
 * 这里的输入捕获使用定时器TIM1_CH1,捕获WK_UP按键的输入
 * 默认是针对TIM1/TIM8等高级定时器 
 * 注意: 通过修改这9个宏定义,可以支持TIM2~TIM5,TIM12/TIM15任意一个定时器的通道1/通道2
 *       特别要注意:如果改其他IO, 通道选择, AF功能等也得改!
 */
#define ATIM_TIMX_PWMIN_CHY_GPIO_PORT           GPIOE
#define ATIM_TIMX_PWMIN_CHY_GPIO_PIN            GPIO_PIN_9
#define ATIM_TIMX_PWMIN_CHY_GPIO_AF             GPIO_AF1_TIM1                           /* AF功能选择 */
#define ATIM_TIMX_PWMIN_CHY_GPIO_CLK_ENABLE()   do{ __HAL_RCC_GPIOE_CLK_ENABLE(); }while(0)   /* PE口时钟使能 */

#define ATIM_TIMX_PWMIN                         TIM1                       
#define ATIM_TIMX_PWMIN_IRQn                    TIM1_UP_IRQn
#define ATIM_TIMX_PWMIN_IRQHandler              TIM1_UP_IRQHandler
#define ATIM_TIMX_PWMIN_CHY                     TIM_CHANNEL_1                           /* 通道Y,  1<= Y <=2*/ 
#define ATIM_TIMX_PWMIN_CHY_CLK_ENABLE()        do{ __HAL_RCC_TIM1_CLK_ENABLE(); }while(0)   /* TIMX 时钟使能 */

 /* TIM1 / TIM8 有独立的捕获中断服务函数,需要单独定义,对于TIM2~5/TIM12/TIM15等,则不需要以下定义 */
#define ATIM_TIMX_PWMIN_CC_IRQn                 TIM1_CC_IRQn
#define ATIM_TIMX_PWMIN_CC_IRQHandler           TIM1_CC_IRQHandler

/******************************************************************************************/


void atim_timx_comp_pwm_init(uint16_t arr, uint16_t psc);   /* 高级定时器 输出比较模式输出PWM 初始化函数 */
void atim_timx_comp_pwm_gpio_init(void);                    /* 定时器输出比较模式通道引脚初始化函数 */

void atim_timx_cplm_pwm_init(uint16_t arr, uint16_t psc);   /* 高级定时器 互补输出 初始化函数 */
void atim_timx_cplm_pwm_gpio_init(void);                    /* 定时器互补输出模式通道引脚初始化函数 */

void atim_timx_pwmin_chy_init(void);                        /* 高级定时器 PWM输入模式初始化 */
void atim_timx_pwmin_chy_restart(void);                     /* 高级定时器 重启PWM输入模式检测 */
void atim_timx_pwmin_chy_gpio_init(void);                   /* 定时器PWM输入模式通道引脚初始化函数 */

#endif

















