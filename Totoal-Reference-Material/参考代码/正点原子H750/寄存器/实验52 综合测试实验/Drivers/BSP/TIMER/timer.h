/**
 ****************************************************************************************************
 * @file        timer.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-05-30
 * @brief       定时器 驱动代码
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
 * V1.0 20220530
 * 第一次发布
 *
 ****************************************************************************************************
 */

#ifndef __BTIM_H
#define __BTIM_H

#include "./SYSTEM/sys/sys.h"


/* RGBLED PWM设置 */
#define RLED_PWM_VAL        TIM3->CCR1
#define GLED_PWM_VAL        TIM15->CCR2
#define BLED_PWM_VAL        TIM15->CCR1

/* LCD PWM背光设置  */
#define LCD_BLPWM_VAL       TIM3->CCR2 



void tim6_int_init(uint16_t arr, uint16_t psc);             /* 定时器6 中断初始化函数 */
void tim7_int_init(uint16_t arr, uint16_t psc);             /* 定时器7 中断初始化函数 */
void tim8_int_init(uint16_t arr, uint16_t psc);             /* 定时器8 中断初始化函数 */

void tim3_ch12_pwm_init(uint16_t arr, uint16_t psc);        /* 定时器3 通道1&2 PWM初始化函数 */
void tim15_ch12_pwm_init(uint16_t arr, uint16_t psc);       /* 定时器15 通道1&2 PWM初始化函数 */

#endif

















