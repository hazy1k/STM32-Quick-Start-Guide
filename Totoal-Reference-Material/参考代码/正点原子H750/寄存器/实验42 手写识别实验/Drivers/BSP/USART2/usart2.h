/**
 ****************************************************************************************************
 * @file        USART22.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-03
 * @brief       串口2 驱动代码
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
 * V1.0 20200403
 * 第一次发布
 *
 ****************************************************************************************************
 */

#ifndef __USART22_H
#define __USART22_H

#include "stdio.h"
#include "./SYSTEM/sys/sys.h"


/******************************************************************************************/
/* 串口2 引脚 定义 */
#define USART2_TX_GPIO_PORT                  GPIOA
#define USART2_TX_GPIO_PIN                   SYS_GPIO_PIN2
#define USART2_TX_GPIO_AF                    7                           /* AF功能选择 */
#define USART2_TX_GPIO_CLK_ENABLE()          do{ RCC->AHB4ENR |= 1 << 0; }while(0)   /* PA口时钟使能 */

#define USART2_RX_GPIO_PORT                  GPIOA
#define USART2_RX_GPIO_PIN                   SYS_GPIO_PIN3
#define USART2_RX_GPIO_AF                    7                           /* AF功能选择 */
#define USART2_RX_GPIO_CLK_ENABLE()          do{ RCC->AHB4ENR |= 1 << 0; }while(0)   /* PA口时钟使能 */

/******************************************************************************************/



void usart2_init(uint32_t sclk, uint32_t baudrate); /* 串口2初始化 */

#endif































