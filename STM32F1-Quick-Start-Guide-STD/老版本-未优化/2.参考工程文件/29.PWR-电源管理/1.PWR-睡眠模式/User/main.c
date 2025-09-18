#include "led.h"
#include "usart.h"
#include "key.h"

static void Delay(__IO uint32_t nCount)
{
	for(; nCount != 0; nCount--);
}

int main(void)
{
	KEY_EXTI_Init();
	LED_Init();
	USART_Config();
	printf("PWR-Sleep Mode Test\r\n");
	while(1)
	{
		printf("绿灯亮，stm32正常运行\r\n");
		LED_GREEN();
		Delay(0xFFFFFF);
		printf("红灯亮，stm32进入睡眠模式\r\n");
		LED_RED();
		__WFI(); // 进入睡眠模式
		Delay(0xFFFFFF);
		printf("已退出睡眠模式\r\n");
	}
}
