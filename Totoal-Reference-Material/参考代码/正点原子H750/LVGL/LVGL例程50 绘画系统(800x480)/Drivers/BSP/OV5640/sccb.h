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
#define SCCB_SCL_GPIO_PIN                GPIO_PIN_10
#define SCCB_SCL_GPIO_CLK_ENABLE()       do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)   /* PB口时钟使能 */

#define SCCB_SDA_GPIO_PORT               GPIOB
#define SCCB_SDA_GPIO_PIN                GPIO_PIN_11
#define SCCB_SDA_GPIO_CLK_ENABLE()       do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)   /* PB口时钟使能 */

/******************************************************************************************/

/* IO操作函数 */
#define SCCB_SCL(x)       do{ x ? \
                              HAL_GPIO_WritePin(SCCB_SCL_GPIO_PORT, SCCB_SCL_GPIO_PIN, GPIO_PIN_SET) : \
                              HAL_GPIO_WritePin(SCCB_SCL_GPIO_PORT, SCCB_SCL_GPIO_PIN, GPIO_PIN_RESET); \
                          }while(0)       /* SCL */

#define SCCB_SDA(x)       do{ x ? \
                              HAL_GPIO_WritePin(SCCB_SDA_GPIO_PORT, SCCB_SDA_GPIO_PIN, GPIO_PIN_SET) : \
                              HAL_GPIO_WritePin(SCCB_SDA_GPIO_PORT, SCCB_SDA_GPIO_PIN, GPIO_PIN_RESET); \
                          }while(0)       /* SDA */

#define SCCB_READ_SDA     HAL_GPIO_ReadPin(SCCB_SDA_GPIO_PORT, SCCB_SDA_GPIO_PIN) /* 读取SDA */


/* 对外接口函数 */
void sccb_init(void);
void sccb_stop(void);
void sccb_start(void);

void sccb_nack(void);
uint8_t sccb_read_byte(void);
uint8_t sccb_send_byte(uint8_t data);

#endif













