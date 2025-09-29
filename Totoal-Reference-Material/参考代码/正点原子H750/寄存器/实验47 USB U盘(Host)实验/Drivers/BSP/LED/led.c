/**
 ****************************************************************************************************
 * @file        led.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-03-12
 * @brief       LED 驱动代码
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

#include "./BSP/LED/led.h"

/**
 * @brief       初始化LED相关IO口, 并使能时钟
 * @param       无
 * @retval      无
 */
void led_init(void)
{
    LED0_GPIO_CLK_ENABLE(); /* LED0时钟使能 */
    LED1_GPIO_CLK_ENABLE(); /* LED1时钟使能 */
    LED2_GPIO_CLK_ENABLE(); /* LED2时钟使能 */

    sys_gpio_set(LED0_GPIO_PORT, LED0_GPIO_PIN,
                 SYS_GPIO_MODE_OUT, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);   /* LED0引脚模式设置 */

    sys_gpio_set(LED1_GPIO_PORT, LED1_GPIO_PIN,
                 SYS_GPIO_MODE_OUT, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);   /* LED1引脚模式设置 */

    sys_gpio_set(LED2_GPIO_PORT, LED2_GPIO_PIN,
                 SYS_GPIO_MODE_OUT, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);   /* LED2引脚模式设置 */

    LED0(1);    /* 关闭 LED0 */
    LED1(1);    /* 关闭 LED1 */
    LED2(1);    /* 关闭 LED2 */
}








