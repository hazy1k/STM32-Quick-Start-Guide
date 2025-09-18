#include "stm32f10x.h"
#include "ili9341_lcd.h"
#include "usart.h" 
#include "key.h"  
#include "led.h"   
#include "bmp.h"
#include "ff.h"
#include <stdio.h>

FATFS fs;				
FRESULT res_sd; 

int main ( void )
{

	ILI9341_Init ();         //LCD 初始化
	
	LCD_SetFont(&Font8x16);
	LCD_SetColors(RED,BLACK);
  ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	/* 清屏，显示全黑 */
  ILI9341_GramScan ( 6 );
	
	USART_Config();		
	LED_GPIO_Config();
	Key_GPIO_Config();
	
	printf("\r\n ********** 显示BMP文件及截图*********** \r\n"); 
	printf("\r\n 实验前请确保SD卡的目录下具有实验使用的BMP图片文件, \r\n"); 
	printf("\r\n 按下板子的KEY1按键可以生成屏幕截图 \r\n"); 
	
	/*挂载sd文件系统*/
	res_sd = f_mount(&fs,"0:",1);
	if(res_sd != FR_OK)
	{
		printf("\r\n请给开发板插入已格式化成fat格式的SD卡。\r\n");
	}
	
	ILI9341_GramScan ( 6 );
	/*显示SD卡内的BMP文件，请确保SD卡该路径下有该文件，
	可在工程目录下的“需要拷贝到SD卡”的文件夹中复制到SD卡的该目录*/
	LCD_Show_BMP(0,0,"0:wildfire_1.bmp");  
	

	while ( 1 )
	{
		/*检测按键*/
		if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON  )
		{		
			static uint8_t name_count = 0;
			char name[40];
			
			//用来设置截图名字，防止重复，实际应用中可以使用系统时间来命名。
			name_count++; 
			sprintf(name,"0:screen_shot_%d.bmp",name_count);

			LED_BLUE;
			printf("\r\n正在截图...");
			
			/*截图必需设置好液晶显示方向和截图窗口*/
			ILI9341_GramScan ( 6 );			
			
			if(Screen_Shot(0,0,LCD_X_LENGTH,LCD_Y_LENGTH,name) == 0)
			{
				printf("\r\n截图成功！");
				LED_GREEN;
			}
			else
			{
				printf("\r\n截图失败！");
				LED_RED;
			}
		}
	}	
}
