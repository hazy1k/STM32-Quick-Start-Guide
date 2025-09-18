#include "wwdg.h"

#define WWDG_CNT 0x7F

static void WWDG_NVIC_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    NVIC_InitStructure.NVIC_IRQChannel = WWDG_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

// tr:递减计数器的值：取值范围0x7F~0x40
// wr:窗口值：取值范围0x7F~0x40
// prv:时钟预分频值
void WWDG_Init(uint8_t tr, uint8_t wr, uint32_t prv)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);
    WWDG_SetCounter(tr);
    WWDG_SetWindowValue(wr); // 窗口值
    WWDG_SetPrescaler(prv); 
    WWDG_Enable(WWDG_CNT);
    WWDG_ClearFlag();
    WWDG_NVIC_Init();
    WWDG_EnableIT();
}

void WWDG_Feed(void)
{
    WWDG_SetCounter(WWDG_CNT);
}

// prv:时钟预分频值
// WWDG_Prescaler_1：WWDG counter clock = (PCLK1/4096)/1
// WWDG_Prescaler_2：WWDG counter clock = (PCLK1/4096)/2
// WWDG_Prescaler_4：WWDG counter clock = (PCLK1/4096)/4
// WWDG_Prescaler_8：WWDG counter clock = (PCLK1/4096)/8
