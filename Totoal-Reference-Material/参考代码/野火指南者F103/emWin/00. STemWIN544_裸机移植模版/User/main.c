/**
  *********************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   STemWIN544_裸机移植模版
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 STM32 全系列开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
#include <stdio.h>

/* FATFS */
#include "ff.h"
#include "diskio.h"
#include "integer.h"
/* 开发板硬件bsp头文件 */
#include "./led/bsp_led.h"
#include "./usart/bsp_usart.h"
#include "./key/bsp_key.h"
#include "./lcd/bsp_ili9341_lcd_emwin.h"
#include "./lcd/bsp_xpt2046_lcd.h"
#include "./flash/bsp_spi_flash.h"
#include "./sram/bsp_fsmc_sram.h"
#include "./TPad/bsp_tpad.h"
#include "./beep/bsp_beep.h" 
#include "./SysTick/bsp_SysTick.h"
/* STemWIN头文件 */
#include "GUI.h"
#include "DIALOG.h"

FATFS   fs;			/* FatFs文件系统对象 */
FIL     file;		/* file objects */
UINT    bw;     /* File R/W count */
FRESULT result; 
FILINFO fno;
DIR dir;

/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{
  /*CRC和emWin没有关系，只是他们为了库的保护而做的，这样STemWin的库只能用在ST的芯片上面，别的芯片是无法使用的。 */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);
  
	/* LED 端口初始化 */
	LED_GPIO_Config();	
  
  /* 初始化定时器 */
	SysTick_Init();
  
  /* 蜂鸣器初始化 */
  Beep_Init();
	
  /* 按键初始化	*/
  Key_GPIO_Config();
  
	/* 初始化串口 */
	USART_Config();
	
	/* 串口调试信息 */
	printf("emWin demo\r\n");

  /* 触摸屏初始化 */
  XPT2046_Init();
  
  /* 提前初始化LCD驱动，在GUI_Init调用LCD_X_Config之前确认LCD驱动芯片ID: lcdid */
  ILI9341_Init();
  
//  /* 挂载文件系统，挂载时会对SD卡初始化 */
//  result = f_mount(&fs,"0:",1);
//	if(result != FR_OK)
//	{
//		printf("SD卡初始化失败，请确保SD卡已正确接入开发板，或换一张SD卡测试！\n");
//		while(1);
//	}
  
	/* 初始化GUI */
	GUI_Init();
  
  while(1)
  {
    MainTask();
  }
}



/*********************************************END OF FILE**********************/
