#include "pwr.h"
#include "key.h"
#include "usart.h"

// 低功耗模式下按键初始化，用于唤醒
void pwr_wkup_key_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    WKUP_GPIO_CLK_ENABLE();
    GPIO_InitStructure.Pin = WKUP_GPIO_Pin;
    GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING; // 上升沿触发
    GPIO_InitStructure.Pull = GPIO_PULLDOWN; // 下拉
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(WKUP_GPIO_Port, &GPIO_InitStructure);
    HAL_NVIC_SetPriority(EXTI0_IRQn, 2, 2);
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(WKUP_GPIO_Pin);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if(GPIO_Pin == WKUP_GPIO_Pin)
    {
        printf("WKUP Key pressed\r\n");
    }
    /* HAL_GPIO_EXTI_IRQHandler()函数已经为我们清除了中断标志位，所以我们进了回调函数可以不做任何事 */
}

// 进入睡眠模式
void pwr_enter_sleep(void)
{
    HAL_SuspendTick(); // 关闭Tick中断
    HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI); // 进入低功耗模式
}
