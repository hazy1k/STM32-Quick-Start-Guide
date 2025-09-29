/**
 ****************************************************************************************************
 * @file        ds18b20.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-2
 * @brief       DS18B20数字温度传感器 驱动代码
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
 * V1.0 20200402
 * 第一次发布
 *
 ****************************************************************************************************
 */

#ifndef __DS18B20_H
#define __DS18B20_H

#include "./SYSTEM/sys/sys.h"


/******************************************************************************************/
/* DS18B20引脚 定义 */

#define DS18B20_DQ_GPIO_PORT                GPIOC
#define DS18B20_DQ_GPIO_PIN                 SYS_GPIO_PIN13
#define DS18B20_DQ_GPIO_CLK_ENABLE()        do{ RCC->AHB4ENR |= 1 << 2; }while(0)   /* PC口时钟使能 */

/******************************************************************************************/

/* IO操作函数 */
#define DS18B20_DQ_OUT(x)       sys_gpio_pin_set(DS18B20_DQ_GPIO_PORT, DS18B20_DQ_GPIO_PIN, x)  /* 数据端口输出 */
#define DS18B20_DQ_IN           sys_gpio_pin_get(DS18B20_DQ_GPIO_PORT, DS18B20_DQ_GPIO_PIN)     /* 数据端口输入 */


uint8_t ds18b20_init(void);         /* 初始化DS18B20 */
uint8_t ds18b20_check(void);        /* 检测是否存在DS18B20 */
short ds18b20_get_temperature(void);/* 获取温度 */

#endif















