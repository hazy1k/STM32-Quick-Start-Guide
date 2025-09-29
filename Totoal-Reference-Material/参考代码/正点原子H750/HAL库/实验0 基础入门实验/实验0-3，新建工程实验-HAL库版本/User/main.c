/**
 ****************************************************************************************************
 * @file     main.c
 * @author   正点原子团队(ALIENTEK)
 * @version  V1.0
 * @date     2020-03-12
 * @brief    新建工程实验-HAL库版本 实验
 * @license  Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 * 
 * 实验平台:正点原子 Mini Pro H750开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 * 
 ****************************************************************************************************
 */

#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"


void led_init(void);                  /* LED初始化函数声明 */

int main(void)
{
    sys_cache_enable();                /* 打开L1-Cache */
    HAL_Init();                        /* 初始化HAL库 */
    sys_stm32_clock_init(240, 2, 2, 4);/* 设置时钟, 480Mhz */
    delay_init(480);                   /* 延时初始化 */
    led_init();                        /* LED初始化 */
    while(1)
    { 
        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_SET);   /* PB4置1 */ 
        HAL_GPIO_WritePin(GPIOE,GPIO_PIN_5,GPIO_PIN_RESET); /* PE5置0 */ 
        delay_ms(500);
        HAL_GPIO_WritePin(GPIOE,GPIO_PIN_5,GPIO_PIN_SET);   /* PE5置1 */
        HAL_GPIO_WritePin(GPIOE,GPIO_PIN_6,GPIO_PIN_RESET); /* PE6置0 */
        delay_ms(500); 
        HAL_GPIO_WritePin(GPIOE,GPIO_PIN_6,GPIO_PIN_SET);   /* PE6置1 */
        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_RESET); /* PB4置0 */
        delay_ms(500);
    }
}

/**
 * @brief       初始化LED相关IO口, 并使能时钟
 * @param       无
 * @retval      无
 */
void led_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    __HAL_RCC_GPIOB_CLK_ENABLE(); /* PB4时钟使能 */
    __HAL_RCC_GPIOE_CLK_ENABLE(); /* PE6时钟使能 */
    __HAL_RCC_GPIOE_CLK_ENABLE(); /* PE5时钟使能 */

    gpio_init_struct.Pin = GPIO_PIN_4;                      /* LED0引脚 */
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;            /* 推挽输出 */
    gpio_init_struct.Pull = GPIO_PULLUP;                    /* 上拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;     /* 高速 */
    HAL_GPIO_Init(GPIOB, &gpio_init_struct);                /* 初始化LED0引脚 */

    gpio_init_struct.Pin = GPIO_PIN_6;                      /* LED1引脚 */
    HAL_GPIO_Init(GPIOE, &gpio_init_struct);                /* 初始化LED1引脚 */
    
    gpio_init_struct.Pin = GPIO_PIN_5;                      /* LED2引脚 */
    HAL_GPIO_Init(GPIOE, &gpio_init_struct);                /* 初始化LED2引脚 */
}








