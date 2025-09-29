/**
 ****************************************************************************************************
 * @file        pwr.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.3
 * @date        2020-03-26
 * @brief       低功耗模式 驱动代码
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
 * V1.0 20200325
 * 第一次发布
 * V1.1 20200325
 * 支持进入睡眠模式及唤醒
 * 新增pwr_wkup_key_init和pwr_enter_sleep函数
 * V1.2 20200325
 * 支持进入停止模式及唤醒
 * 新增pwr_enter_stop函数
 * V1.3 20200326
 * 支持进入待机模式及唤醒
 * 新增pwr_enter_standby函数
 *
 ****************************************************************************************************
 */

#ifndef __PWR_H
#define __PWR_H

#include "./SYSTEM/sys/sys.h"



/******************************************************************************************/
/* PWR WKUP 按键 引脚和中断 定义 
 * 我们通过WK_UP按键唤醒 MCU,  因此必须定义这个按键及其对应的中断服务函数 
 */

#define PWR_WKUP_GPIO_PORT              GPIOA
#define PWR_WKUP_GPIO_PIN               SYS_GPIO_PIN0
#define PWR_WKUP_GPIO_CLK_ENABLE()      do{ RCC->AHB4ENR |= 1 << 0; }while(0)   /* PA口时钟使能 */
  
#define PWR_WKUP_INT_IRQn               EXTI0_IRQn
#define PWR_WKUP_INT_IRQHandler         EXTI0_IRQHandler

/******************************************************************************************/




void pwr_pvd_init(uint8_t pls);     /* PVD电压检测初始化函数 */
void pwr_wkup_key_init(void);       /* 唤醒按键初始化 */
void pwr_enter_sleep(void);         /* 进入睡眠模式 */
void pwr_enter_stop(void);          /* 进入停止模式 */
void pwr_enter_standby(void);       /* 进入待机模式 */

#endif




















