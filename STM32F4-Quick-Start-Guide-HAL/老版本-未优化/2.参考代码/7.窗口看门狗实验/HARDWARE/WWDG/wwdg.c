#include "wwdg.h"
#include "led.h"

WWDG_HandleTypeDef WWDG_Handler; // 定义看门狗句柄

// 初始化窗口看门狗
/*
   参数：
   tr：T[6:0]，计数器值
   wr：W[6:0]，窗口值
   fp：分频系数，最低2位有效
   计算公式：
   Fwwdg = PCLK1/(4096*(2^fp)) ,其中PCLK1为时钟频率一般为42Mhz
*/
void WWDG_Init(uint8_t tr, uint8_t wr, uint32_t fp)
{
    WWDG_Handler.Instance = WWDG; // 选择看门狗外设
    WWDG_Handler.Init.Prescaler = fp; // 设置分频系数
    WWDG_Handler.Init.Window = wr; // 设置窗口值
    WWDG_Handler.Init.Counter = tr; // 设置计数器值
    WWDG_Handler.Init.EWIMode = WWDG_EWI_ENABLE; // 使能提前唤醒模式
    HAL_WWDG_Init(&WWDG_Handler); // 初始化看门狗
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
    HAL_WWDG_Refresh(hwwdg); // 刷新看门狗计数器
    LED1 = !LED1;
}
