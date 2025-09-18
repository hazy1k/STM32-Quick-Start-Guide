#include "led.h"
#include "usart.h"
#include "key.h"
#include "stm32f10x.h"

static void Delay(__IO uint32_t nCount)	
{
	for(; nCount != 0; nCount--);
}

static void SYSConfig_STOP(void)
{
	RCC_HSEConfig(RCC_HSE_ON); // 使能HSE
	while(RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET); // 等待HSE就绪
	RCC_PLLCmd(ENABLE); // 使能PLL
	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET); // 等待PLL就绪
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK); // 选择PLL作为系统时钟源
	while(RCC_GetSYSCLKSource() != 0x08); // 等待系统时钟源切换到PLL
}

int main(void)
{
	RCC_ClocksTypeDef clock_status_wakeup;
	RCC_ClocksTypeDef clock_status_config;
	uint8_t clock_source_wakeup;
	uint8_t clock_source_config;
	LED_Init();
	USART_Config();
	KEY_EXTI_Init();
	while(1)
	{
		printf("Green light on, program running normally\r\n");
		LED_GREEN();
		Delay(0xFFFFFF);
		printf("Red light on, entering STOP mode\r\n");
		LED_RED();
		PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI); // 进入停止模式
		// 获取刚唤醒时的时钟源
		clock_source_wakeup = RCC_GetSYSCLKSource();
		RCC_GetClocksFreq(&clock_status_wakeup);
		// 从停止模式唤醒后使用的是HSI时钟，所以需要重新配置时钟
		SYSConfig_STOP();
		clock_source_config = RCC_GetSYSCLKSource();
		RCC_GetClocksFreq(&clock_status_config);
		// 打印两种情况的时钟状态
	 	printf("\r\nClock status after reconfiguration:\r\n");
		printf("SYSCLK frequency: %d,\r\n HCLK frequency: %d,\r\n PCLK1 frequency: %d,\r\n PCLK2 frequency: %d,\r\n Clock source: %d (0 means HSI, 8 means PLLCLK)\n", 
			clock_status_config.SYSCLK_Frequency, 
			clock_status_config.HCLK_Frequency, 
			clock_status_config.PCLK1_Frequency, 
			clock_status_config.PCLK2_Frequency, 
			clock_source_config);
		printf("\r\nClock status just after wakeup:\r\n");	
		printf("SYSCLK frequency: %d,\r\n HCLK frequency: %d,\r\n PCLK1 frequency: %d,\r\n PCLK2 frequency: %d,\r\n Clock source: %d (0 means HSI, 8 means PLLCLK)\n", 
			clock_status_wakeup.SYSCLK_Frequency, 
			clock_status_wakeup.HCLK_Frequency, 
			clock_status_wakeup.PCLK1_Frequency, 
			clock_status_wakeup.PCLK2_Frequency, 
			clock_source_wakeup);
		Delay(0xFFFFFF);
		printf("Test finished\r\n");	
	}
}
