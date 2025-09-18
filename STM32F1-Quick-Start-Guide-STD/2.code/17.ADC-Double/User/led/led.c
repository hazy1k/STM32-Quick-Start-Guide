#include "led.h"

void BSP_LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(LED_GPIO_CLK, ENABLE); // 打开GPIO时钟
    
    // 初始化红色 LED 引脚
    GPIO_InitStructure.GPIO_Pin = RED_LED_Pin; // 选择引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // 输出速度
    GPIO_Init(LED_GPIO_Port, &GPIO_InitStructure);
    
    // 初始化绿色 LED 引脚
    GPIO_InitStructure.GPIO_Pin = GREEN_LED_Pin;
    GPIO_Init(LED_GPIO_Port, &GPIO_InitStructure);
    
    // 初始化蓝色 LED 引脚
    GPIO_InitStructure.GPIO_Pin = BLUE_LED_Pin;
    GPIO_Init(LED_GPIO_Port, &GPIO_InitStructure);
    
    // 初始LED全灭
    LED_OFF(RED_LED_Pin);
    LED_OFF(GREEN_LED_Pin);
    LED_OFF(BLUE_LED_Pin);
}

void GPIO_ToggleBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    if (GPIO_ReadOutputDataBit(GPIOx, GPIO_Pin) == Bit_SET)
    {
        GPIO_ResetBits(GPIOx, GPIO_Pin);
    }
    else
    {
        GPIO_SetBits(GPIOx, GPIO_Pin);
    }
}
