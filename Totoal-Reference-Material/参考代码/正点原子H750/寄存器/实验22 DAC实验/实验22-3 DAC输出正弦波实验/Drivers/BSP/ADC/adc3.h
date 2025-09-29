/**
 ****************************************************************************************************
 * @file        adc3.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-03-28
 * @brief       ADC3(开启内部温度传感器) 驱动代码
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

#ifndef __ADC3_H
#define __ADC3_H

#include "./SYSTEM/sys/sys.h"


/******************************************************************************************/
/* ADC3温度传感器通道 定义 */ 

#define ADC3_TEMPSENSOR_CHX             18  

/******************************************************************************************/


void adc3_init(void);                               /* ADC3初始化 */
void adc3_channel_set(uint8_t ch, uint8_t stime);   /* ADC3通道设置 */
uint32_t adc3_get_result(uint8_t ch);               /* 获得某个通道值  */
uint32_t adc3_get_result_average(uint8_t ch, uint8_t times);/* 得到某个通道给定次数采样的平均值 */

short adc3_get_temperature(void);     /* ADC3 获取内部温度传感器的温度值 */

#endif 















