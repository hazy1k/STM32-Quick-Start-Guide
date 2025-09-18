#include "bsp_init.h" 
#include "lcd.h"      
#include "stdio.h"    
#include "delay.h"    
#include "usmart.h"

int main(void)
{
	bsp_init();
	usmart_dev.init(84); 
	LCD_Init();
	LCD_ShowString(30, 50, 200, 16, 16, "LCD TEST");
	while(1)
	{
		LED_TOGGLE(LED0_GPIO_Pin);
		delay_ms(1000);
	}
}
