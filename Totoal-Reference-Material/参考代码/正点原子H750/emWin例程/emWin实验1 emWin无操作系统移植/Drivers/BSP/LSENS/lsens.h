/**
 ****************************************************************************************************
 * @file        lsens.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-03-28
 * @brief       光敏传感器 驱动代码
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
 * V1.0 20200328
 * 第一次发布
 ****************************************************************************************************
 */

#ifndef __LSENS_H
#define __LSENS_H

#include "./SYSTEM/sys/sys.h"
#include "./BSP/ADC/adc3.h"


/******************************************************************************************/
/* 光敏传感器对应ADC3的输入引脚和通道 定义 */

#define LSENS_ADC3_CHX_GPIO_PORT            GPIOC
#define LSENS_ADC3_CHX_GPIO_PIN             GPIO_PIN_1
#define LSENS_ADC3_CHX_GPIO_CLK_ENABLE()    do{ __HAL_RCC_GPIOC_CLK_ENABLE(); }while(0)   /* PC口时钟使能 */


#define LSENS_ADC3_CHX                      ADC_CHANNEL_11      /* 通道Y,  0 <= Y <= 19 */ 

/******************************************************************************************/
 

void lsens_init(void);          /* 初始化光敏传感器 */
uint8_t lsens_get_val(void);    /* 读取光敏传感器的值 */
#endif 





















