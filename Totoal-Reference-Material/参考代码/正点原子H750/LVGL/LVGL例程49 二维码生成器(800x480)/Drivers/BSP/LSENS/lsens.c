/**
 ****************************************************************************************************
 * @file        lsens.c
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

#include "./BSP/LSENS/lsens.h"
#include "./SYSTEM/delay/delay.h"


extern ADC_HandleTypeDef  adc3_handle; /* ADC句柄 */

/**
 * @brief       初始化光敏传感器
 * @param       无
 * @retval      无
 */
void lsens_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;

    __HAL_RCC_GPIOC_CLK_ENABLE();                   /* 开启GPIOC时钟 */

    gpio_init_struct.Pin = LSENS_ADC3_CHX_GPIO_PIN; /* PC1 */
    gpio_init_struct.Mode = GPIO_MODE_ANALOG;       /* 模拟 */
    gpio_init_struct.Pull = GPIO_NOPULL;            /* 不带上下拉 */
    HAL_GPIO_Init(LSENS_ADC3_CHX_GPIO_PORT, &gpio_init_struct);
    
    adc3_init();    /* 初始化ADC3 */
}

/**
 * @brief       读取光敏传感器值
 * @param       无
 * @retval      0~100:0,最暗;100,最亮
 */
uint8_t lsens_get_val(void)
{
    uint32_t temp_val = 0;
    temp_val = adc3_get_result_average(adc3_handle, LSENS_ADC3_CHX, 10); /* 读取平均值 */
    temp_val /= 655;

    if (temp_val > 100)temp_val = 100;

    return (uint8_t)(100 - temp_val);
}












