#include "stm32f10x.h"
#include "ili9341.h"
#include "xpt2046.h"
#include "flash.h"
#include "led.h" 
#include "palette.h"
#include <string.h>

int main(void)
{		
	ILI9341_Init();  	
	XPT2046_Init();
	//从FLASH里获取校正参数，若FLASH无参数，则使用模式3进行校正
	Calibrate_or_Get_TouchParaWithFlash(3,0);
	LED_GPIO_Config();
  	ILI9341_GramScan(3);	
	Palette_Init(LCD_SCAN_MODE);
	while(1)
	{
		// 触摸检测函数，本函数至少10ms调用一次
		XPT2046_TouchEvenHandler();
	}
}
