#include "stm32f10x.h"
#include "sdio_sdcard.h"
#include "sdio_test.h"
#include "usart.h"	
#include "led.h"
#include "key.h" 

int main(void)
{
	LED_Init();	
	KEY_GPIO_Init();
	USART_Config();
	printf("按下KEY1键开始测试\r\n");
	while(1)
	{
		if(KEY_Scan(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == KEY_ON)
		{
			printf("开始测试SDIO-SD卡读写\r\n");
			SD_Test();
		}
	}							   
}
