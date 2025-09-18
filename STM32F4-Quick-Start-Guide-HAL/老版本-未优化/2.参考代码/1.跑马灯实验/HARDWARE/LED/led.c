#include "led.h"

void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOF_CLK_ENABLE(); // 初始化GPIOF时钟          
    GPIO_Initure.Pin = LED0_PIN | LED1_PIN;  // 使用LED0_PIN和LED1_PIN
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP; // 推挽输出
    GPIO_Initure.Pull = GPIO_PULLUP;         // 上拉
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;    // 高速
    HAL_GPIO_Init(GPIOF, &GPIO_Initure);
}

// void HAL_GPIO_WritePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState)
// 该函数用于设置指定GPIO端口的指定引脚的电平状态
// 函数参数：
// GPIOx：GPIO端口，可以是GPIOA、GPIOB、GPIOC、GPIOD、GPIOE、GPIOF等。
// GPIO_PIN：GPIO引脚，可以是GPIO_PIN_0~GPIO_PIN_15。
// PinState：GPIO引脚的电平状态，可以是GPIO_PIN_RESET、GPIO_PIN_SET。
// 其中GPIO_PIN_RESET为低电平，GPIO_PIN_SET为高电平。
