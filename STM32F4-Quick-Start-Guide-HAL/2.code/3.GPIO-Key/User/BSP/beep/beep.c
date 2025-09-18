#include "beep.h"

void bsp_beep_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    BEEP_GPIO_CLK_ENABLE();
    GPIO_InitStructure.Pin = BEEP_GPIO_Pin;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(BEEP_GPIO_Port, &GPIO_InitStructure);
    BEEP_OFF();
}
