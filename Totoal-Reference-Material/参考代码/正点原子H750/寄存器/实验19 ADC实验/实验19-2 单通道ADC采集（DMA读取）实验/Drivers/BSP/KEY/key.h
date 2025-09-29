/**
 ****************************************************************************************************
 * @file        key.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-03-13
 * @brief       按键输入 驱动代码
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
 * V1.0 20200312
 * 第一次发布
 *
 ****************************************************************************************************
 */

#ifndef __KEY_H
#define __KEY_H

#include "./SYSTEM/sys/sys.h"

/******************************************************************************************/
/* 引脚 定义 */

#define KEY0_GPIO_PORT                  GPIOA
#define KEY0_GPIO_PIN                   SYS_GPIO_PIN1
#define KEY0_GPIO_CLK_ENABLE()          do{ RCC->AHB4ENR |= 1 << 0; }while(0)   /* PA口时钟使能 */

#define KEY1_GPIO_PORT                  GPIOA
#define KEY1_GPIO_PIN                   SYS_GPIO_PIN15
#define KEY1_GPIO_CLK_ENABLE()          do{ RCC->AHB4ENR |= 1 << 0; }while(0)   /* PA口时钟使能 */

#define WKUP_GPIO_PORT                  GPIOA
#define WKUP_GPIO_PIN                   SYS_GPIO_PIN0
#define WKUP_GPIO_CLK_ENABLE()          do{ RCC->AHB4ENR |= 1 << 0; }while(0)   /* PA口时钟使能 */

/******************************************************************************************/

#define KEY0        sys_gpio_pin_get(KEY0_GPIO_PORT, KEY0_GPIO_PIN)      /* 读取KEY0引脚 */
#define KEY1        sys_gpio_pin_get(KEY1_GPIO_PORT, KEY1_GPIO_PIN)      /* 读取KEY1引脚 */
#define WK_UP       sys_gpio_pin_get(WKUP_GPIO_PORT, WKUP_GPIO_PIN)      /* 读取WKUP引脚 */


#define KEY0_PRES    1      /* KEY0按下 */
#define KEY1_PRES    2      /* KEY1按下 */
#define WKUP_PRES    3      /* KEY_UP按下(即WK_UP) */

void key_init(void);        /* 按键初始化函数 */
uint8_t key_scan(uint8_t mode);/* 按键扫描函数 */

#endif


















