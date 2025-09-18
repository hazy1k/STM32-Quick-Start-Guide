#include "stm32f10x.h"
#include "systick.h"
#include "led.h"
#include "key.h"
#include "usart.h"
#include <stdio.h>

void PVD_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    NVIC_InitStructure.NVIC_IRQChannel = PVD_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    EXTI_ClearITPendingBit(EXTI_Line16);
    EXTI_InitStructure.EXTI_Line = EXTI_Line16;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    PWR_PVDLevelConfig(PWR_PVDLevel_2V6); // VDD电压低于2.6V时触发中断
    PWR_PVDCmd(ENABLE);
}

void PVD_IRQHandler(void)
{
    if(PWR_GetFlagStatus(PWR_FLAG_PVDO) == SET)
    {
        LED_ON(RED_LED_Pin);
    } 
    EXTI_ClearITPendingBit(EXTI_Line16);
}

int main(void)
{   
    PVD_Config();
    BSP_LED_Init();
    SysTick_Init();
    while(1);
}
