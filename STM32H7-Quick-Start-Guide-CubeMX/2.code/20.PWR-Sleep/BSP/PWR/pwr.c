#include "pwr.h"
#include "key.h"

// 低功耗模式下的按键初始化(用于唤醒睡眠模式/停止模式)
void pwr_key_init(void)
{
	GPIO_InitTypeDef GPIO_Init;
	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_Init.Pin = WK_UP_Pin;
	GPIO_Init.Mode = GPIO_MODE_IT_RISING; // 上升沿中断
	GPIO_Init.Pull = GPIO_PULLDOWN;
	GPIO_Init.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(WK_UP_GPIO_Port, &GPIO_Init);
	HAL_NVIC_SetPriority(EXTI0_IRQn, 2, 2);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

// 进入睡眠模式
void pwr_sleep(void)
{
	HAL_SuspendTick(); // 暂停滴答时钟
	HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI); // 进入睡眠模式
}

void EXTI0_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(WK_UP_Pin);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == WK_UP_Pin)
	{

	}
}
