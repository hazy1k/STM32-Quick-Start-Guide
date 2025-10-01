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
 * @brief       外部中断初始化程序
 * @param       无
 * @retval      无
 */
void extix_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    
    KEY0_INT_GPIO_CLK_ENABLE();                              /* KEY0时钟使能 */
    KEY1_INT_GPIO_CLK_ENABLE();                              /* KEY1时钟使能 */
    WKUP_INT_GPIO_CLK_ENABLE();                              /* WKUP时钟使能 */
    
    gpio_init_struct.Pin = KEY0_INT_GPIO_PIN;                /* KEY0引脚 */
    gpio_init_struct.Mode = GPIO_MODE_IT_FALLING;            /* 下升沿触发 */
    gpio_init_struct.Pull = GPIO_PULLUP;                     /* 上拉 */
    HAL_GPIO_Init(KEY0_INT_GPIO_PORT, &gpio_init_struct);    /* KEY0引脚初始化 */
    
    gpio_init_struct.Pin=  KEY1_INT_GPIO_PIN;                /* KEY1引脚 */
    gpio_init_struct.Mode = GPIO_MODE_IT_FALLING;            /* 下升沿触发 */
    gpio_init_struct.Pull = GPIO_PULLUP;                     /* 上拉 */
    HAL_GPIO_Init(KEY1_INT_GPIO_PORT, &gpio_init_struct);    /* KEY1引脚初始化 */
    
    gpio_init_struct.Pin = WKUP_INT_GPIO_PIN;                /* WKUP引脚 */
    gpio_init_struct.Mode = GPIO_MODE_IT_RISING;             /* 上升沿触发 */
    gpio_init_struct.Pull = GPIO_PULLDOWN;                   /* 下拉 */
    HAL_GPIO_Init(WKUP_INT_GPIO_PORT, &gpio_init_struct);    /* WKUP引脚初始化 */

    HAL_NVIC_SetPriority(WKUP_INT_IRQn, 2, 0);               /* 抢占优先级为2，子优先级为0 */
    HAL_NVIC_EnableIRQ(WKUP_INT_IRQn);                       /* 使能中断线0 */
    
    HAL_NVIC_SetPriority(KEY0_INT_IRQn, 2, 1);               /* 抢占优先级为2，子优先级为1 */
    HAL_NVIC_EnableIRQ(KEY0_INT_IRQn);                       /* 使能中断线1 */

    HAL_NVIC_SetPriority(KEY1_INT_IRQn, 2, 2);               /* 抢占优先级为2，子优先级为2 */
    HAL_NVIC_EnableIRQ(KEY1_INT_IRQn);                       /* 使能中断线15 */
}


/**
 * @brief       外部中断服务程序
 * @param       无
 * @retval      无
 */
void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(WKUP_INT_GPIO_PIN);   /* 调用中断处理公用函数 */
}

void EXTI1_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(KEY0_INT_GPIO_PIN);   /* 调用中断处理公用函数 */
}

void EXTI15_10_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(KEY1_INT_GPIO_PIN);   /* 调用中断处理公用函数 */
}

/**
 * @brief       外部中断回调函数
 * @param       GPIO_Pin: 中断引脚号
 * @note        在HAL库中所有的外部中断服务函数都会调用此函数
 * @retval      无
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    /* 消抖，此处为了方便使用了延时函数，实际代码中禁止在中断服务函数中调用任何delay之类的延时函数！！！ */
    delay_ms(20);     
    switch (GPIO_Pin)
    {
        case WKUP_INT_GPIO_PIN:
            if (WK_UP == 1)        /* WK_UP中断 */
            {
                LED2_TOGGLE();  /* LED2 状态取反 */  
            }
            break;
        case KEY0_INT_GPIO_PIN:
            if (KEY0 == 0)         /* KEY0U中断 */
            {
                LED0_TOGGLE();  /* LED0 状态取反 */
            }
            break;
        case KEY1_INT_GPIO_PIN:
            if (KEY1 == 0)         /* KEY1中断 */
            {
                LED1_TOGGLE();  /* LED1 状态取反 */
            }
            break;
    }
}

