/**
 ****************************************************************************************************
 * @file        CT_IIC.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-03-29
 * @brief       电容触摸屏 驱动代码
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
 * V1.0 20200329
 * 第一次发布
 *
 ****************************************************************************************************
 */

#ifndef __CT_IIC_H
#define __CT_IIC_H

#include "./SYSTEM/sys/sys.h"


/******************************************************************************************/
/* CT_IIC 引脚 定义 */

#define CT_IIC_SCL_GPIO_PORT            GPIOB
#define CT_IIC_SCL_GPIO_PIN             SYS_GPIO_PIN0
#define CT_IIC_SCL_GPIO_CLK_ENABLE()    do{ RCC->AHB4ENR |= 1 << 1; }while(0)   /* PB口时钟使能 */

#define CT_IIC_SDA_GPIO_PORT            GPIOB
#define CT_IIC_SDA_GPIO_PIN             SYS_GPIO_PIN3
#define CT_IIC_SDA_GPIO_CLK_ENABLE()    do{ RCC->AHB4ENR |= 1 << 1; }while(0)   /* PB口时钟使能 */

/******************************************************************************************/

/* IO操作函数 */
#define CT_IIC_SCL(x)      sys_gpio_pin_set(CT_IIC_SCL_GPIO_PORT, CT_IIC_SCL_GPIO_PIN, x)   /* SCL */
#define CT_IIC_SDA(x)      sys_gpio_pin_set(CT_IIC_SDA_GPIO_PORT, CT_IIC_SDA_GPIO_PIN, x)   /* SDA */
#define CT_READ_SDA        sys_gpio_pin_get(CT_IIC_SDA_GPIO_PORT, CT_IIC_SDA_GPIO_PIN)      /* 读取SDA */


/* IIC所有操作函数 */
void ct_iic_init(void);             /* 初始化IIC的IO口 */
void ct_iic_stop(void);             /* 发送IIC停止信号 */
void ct_iic_start(void);            /* 发送IIC开始信号 */

void ct_iic_ack(void);              /* IIC发送ACK信号 */
void ct_iic_nack(void);             /* IIC不发送ACK信号 */
uint8_t ct_iic_wait_ack(void);      /* IIC等待ACK信号 */

void ct_iic_send_byte(uint8_t txd);         /* IIC发送一个字节 */
uint8_t ct_iic_read_byte(unsigned char ack);/* IIC读取一个字节 */

#endif







