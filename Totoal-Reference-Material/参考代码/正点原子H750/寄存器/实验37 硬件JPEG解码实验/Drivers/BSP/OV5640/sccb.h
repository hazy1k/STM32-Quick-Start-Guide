/**
 ****************************************************************************************************
 * @file        sccb.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-03
 * @brief       SCCB 驱动代码
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

#ifndef __SCCB_H
#define __SCCB_H

#include "./SYSTEM/sys/sys.h"


/******************************************************************************************/
/* 引脚 定义 */

#define SCCB_SCL_GPIO_PORT               GPIOB
#define SCCB_SCL_GPIO_PIN                SYS_GPIO_PIN10
#define SCCB_SCL_GPIO_CLK_ENABLE()       do{ RCC->AHB4ENR |= 1 << 1; }while(0)   /* PB口时钟使能 */

#define SCCB_SDA_GPIO_PORT               GPIOB
#define SCCB_SDA_GPIO_PIN                SYS_GPIO_PIN11
#define SCCB_SDA_GPIO_CLK_ENABLE()       do{ RCC->AHB4ENR |= 1 << 1; }while(0)   /* PB口时钟使能 */

/******************************************************************************************/

/* IO操作函数 */
#define SCCB_SCL(x)         sys_gpio_pin_set(SCCB_SCL_GPIO_PORT, SCCB_SCL_GPIO_PIN, x)  /* SCL */
#define SCCB_SDA(x)         sys_gpio_pin_set(SCCB_SDA_GPIO_PORT, SCCB_SDA_GPIO_PIN, x)  /* SDA */
#define SCCB_READ_SDA       sys_gpio_pin_get(SCCB_SDA_GPIO_PORT, SCCB_SDA_GPIO_PIN)     /* 读取SDA */


/* 对外接口函数 */
void sccb_init(void);
void sccb_stop(void);
void sccb_start(void);

void sccb_nack(void);
uint8_t sccb_read_byte(void);
uint8_t sccb_send_byte(uint8_t data);

#endif













