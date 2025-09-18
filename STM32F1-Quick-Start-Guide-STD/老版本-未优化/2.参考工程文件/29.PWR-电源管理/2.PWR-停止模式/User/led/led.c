#include "led.h"

// LED初始化函数
void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // 使能GPIOB时钟
    RCC_APB2PeriphClockCmd(LED1_GPIO_CLK | LED2_GPIO_CLK | LED3_GPIO_CLK, ENABLE);

    // 初始化LED1
    GPIO_InitStructure.GPIO_Pin = LED1_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // 设置为推挽输出模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // 设置GPIO速度
    GPIO_Init(LED1_GPIO_PORT, &GPIO_InitStructure);

    // 初始化LED2
    GPIO_InitStructure.GPIO_Pin = LED2_GPIO_PIN;
    GPIO_Init(LED2_GPIO_PORT, &GPIO_InitStructure);

    // 初始化LED3
    GPIO_InitStructure.GPIO_Pin = LED3_GPIO_PIN;
    GPIO_Init(LED3_GPIO_PORT, &GPIO_InitStructure);
    // 初始化完成后，先关闭所有LED
    LED_RGBOFF();
}
