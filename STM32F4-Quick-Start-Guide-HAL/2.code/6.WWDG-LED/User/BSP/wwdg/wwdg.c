#include "wwdg.h"

WWDG_HandleTypeDef WWDG_Handler;
/*
    tr：T[6:0]，计数器值
    tw：W[6:0]，窗口值
    fprer：分频系数，Fwwdg=PLCK1/(4096*2^fprer)
*/
void bsp_wwdg_init(uint8_t tr, uint8_t wr, uint32_t fprer)
{
    /* WWDG初始化 */
    WWDG_Handler.Instance = WWDG;
    WWDG_Handler.Init.Prescaler = fprer;
    WWDG_Handler.Init.Window = wr;
    WWDG_Handler.Init.Counter = tr;
    WWDG_Handler.Init.EWIMode = WWDG_EWI_ENABLE; // 使能WWDG提前唤醒中断
    HAL_WWDG_Init(&WWDG_Handler);
}

// 此函数会被WWDG_Init()调用
void HAL_WWDG_MspInit(WWDG_HandleTypeDef *hwwdg)
{
    __HAL_RCC_WWDG_CLK_ENABLE(); // 使能看门狗时钟
    HAL_NVIC_SetPriority(WWDG_IRQn, 2, 3); // 设置看门狗中断优先级
    HAL_NVIC_EnableIRQ(WWDG_IRQn); // 使能看门狗中断
}

// 看门狗中断服务函数
void WWDG_IRQHandler(void)
{
    HAL_WWDG_IRQHandler(&WWDG_Handler); // 调用WWDG共用中断处理函数
}

// 此函数会被HAL_WWDG_IRQHandler调用
void HAL_WWDG_EarlyWakeupCallback(WWDG_HandleTypeDef *hwwdg)
{
    LED_TOGGLE(LED0_GPIO_Pin);
    HAL_WWDG_Refresh(hwwdg); // 刷新看门狗计数器
}
