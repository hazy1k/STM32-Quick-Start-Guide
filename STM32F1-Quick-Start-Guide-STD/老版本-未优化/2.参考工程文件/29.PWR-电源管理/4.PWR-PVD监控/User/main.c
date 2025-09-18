#include "stm32f10x.h"
#include "led.h"
#include "key.h" 
#include "usart.h"
#include "pvd.h"

int main(void)
{	
	LED_Init();	
	LED_GREEN(); 
	// 配置PVD，当电压过低时，会进入中断服务函数，亮红灯
	PVD_Config();
	while(1)
	{					
	}
}
