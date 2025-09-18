#include "beep.h"

void BSP_Beep_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(Beep_GPIO_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin = Beep_GPIO_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(Beep_GPIO_Port, &GPIO_InitStructure);
    BEEP_OFF();
}
