#include "key.h"

void BSP_KEY_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(KEY1_GPIO_CLK|KEY2_GPIO_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin = KEY1_GPIO_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 浮空输入
    GPIO_Init(KEY1_GPIO_Port, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = KEY2_GPIO_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(KEY2_GPIO_Port, &GPIO_InitStructure);
}

uint8_t KEY_Scan(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    if(GPIO_ReadInputDataBit(GPIOx, GPIO_Pin) == KEY_ON) // 读取按键电平
    {
        while(GPIO_ReadInputDataBit(GPIOx, GPIO_Pin) == KEY_ON); // 判断按键是否长时间按下
        return KEY_ON;
    }
    else
    {
        return KEY_OFF;
    }
}
