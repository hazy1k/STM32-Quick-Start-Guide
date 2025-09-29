/**
 ****************************************************************************************************
 * @file        exti.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-03-13
 * @brief       外部中断 驱动代码
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
 * V1.0 20200313
 * 第一次发布
 *
 ****************************************************************************************************
 */

#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/KEY/key.h"
#include "./BSP/EXTI/exti.h"

/**
 * @brief       KEY0 外部中断服务程序
 * @param       无
 * @retval      无
 */
void KEY0_INT_IRQHandler(void)
{
    delay_ms(20);  /* 消抖 */
    EXTI_D1->PR1 = KEY0_INT_GPIO_PIN;   /* 清除KEY0所在中断线 的中断标志位 */

    if (KEY0 == 0)
    {
        LED0_TOGGLE();  /* LED0 状态取反 */ 
    }
}

/**
 * @brief       KEY1 外部中断服务程序
 * @param       无
 * @retval      无
 */
void KEY1_INT_IRQHandler(void)
{ 
    delay_ms(20);  /* 消抖 */
    EXTI_D1->PR1 = KEY1_INT_GPIO_PIN;   /* 清除KEY1所在中断线 的中断标志位 */

    if (KEY1 == 0)
    {
        LED1_TOGGLE();  /* LED1 状态取反 */ 
    }
}

/**
 * @brief       WK_UP 外部中断服务程序
 * @param       无
 * @retval      无
 */
void WKUP_INT_IRQHandler(void)
{ 
    delay_ms(20);  /* 消抖 */
    EXTI_D1->PR1 = WKUP_INT_GPIO_PIN;   /* 清除WKUP所在中断线 的中断标志位 */

    if (WK_UP == 1)
    {
        LED2_TOGGLE();  /* LED2 状态取反 */ 
    }
}

/**
 * @brief       外部中断初始化程序
 * @param       无
 * @retval      无
 */
void extix_init(void)
{
    key_init();
    sys_nvic_ex_config(KEY0_INT_GPIO_PORT, KEY0_INT_GPIO_PIN, SYS_GPIO_FTIR);   /* KEY0配置为下降沿触发中断 */
    sys_nvic_ex_config(KEY1_INT_GPIO_PORT, KEY1_INT_GPIO_PIN, SYS_GPIO_FTIR);   /* KEY1配置为下降沿触发中断 */
    sys_nvic_ex_config(WKUP_INT_GPIO_PORT, WKUP_INT_GPIO_PIN, SYS_GPIO_RTIR);   /* WKUP配置为上升沿触发中断 */

    sys_nvic_init( 0, 2, KEY0_INT_IRQn, 2); /* 抢占0，子优先级2，组2 */
    sys_nvic_init( 1, 2, KEY1_INT_IRQn, 2); /* 抢占1，子优先级2，组2 */
    sys_nvic_init( 2, 2, WKUP_INT_IRQn, 2); /* 抢占2，子优先级2，组2 */
}












