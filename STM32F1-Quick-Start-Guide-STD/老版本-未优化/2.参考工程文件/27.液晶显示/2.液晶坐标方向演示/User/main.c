#include "stm32f10x.h"
#include "lcd.h"
#include <stdio.h>

static void LCD_Test(void);	
static void LCD_Direction_Show(void);	
static void Delay(__IO uint32_t nCount);

int main(void)
{
	ST7789V_Init();
	while(1)
	{
		LCD_Direction_Show();
	}		
}
void LCD_Direction_Show(void) // 用于展示LCD的八种方向模式
{
	uint8_t i = 0;	
	char dispBuff[100];
	// 轮流展示各个方向模式
	for(i = 0; i < 8; i++)
	{	
		LCD_SetFont(&Font16x24);
		LCD_SetColors(RED,BLACK);
		ST7789V_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);
	    // 其中0、3、5、6 模式适合从左至右显示文字，
	    // 不推荐使用其它模式显示文字	其它模式显示文字会有镜像效果			 
		ST7789V_GramScan(i);
		sprintf(dispBuff,"o%d. X --->",i);
		ST7789V_DispStringLine_EN(LINE(0),dispBuff);	
		sprintf(dispBuff,"o%d.Y|V",i);	
		ST7789V_DispString_EN_YDir(0,0,dispBuff);
		Delay(0xFFFFFF);		
		LCD_Test();
	}
}
extern uint16_t lcdid;
void LCD_Test(void)
{
	static uint8_t testCNT = 0;	
	char dispBuff[100];
	testCNT++;	
	LCD_SetFont(&Font8x16);
	LCD_SetColors(RED,BLACK);
  	ST7789V_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);
  	if(lcdid == LCDID_ST7789V)
  	{
    	ST7789V_DispStringLine_EN(LINE(0),"ST7789V LCD driver");
  	}
	LCD_SetFont(&Font16x24);
	LCD_SetTextColor(GREEN);
	sprintf(dispBuff,"Count : %d ",testCNT);
  	LCD_ClearLine(LINE(1));
	ST7789V_DispStringLine_EN(LINE(1),dispBuff);
	LCD_SetFont(&Font24x32);  
  	LCD_ClearLine(LINE(2));
	LCD_SetTextColor(BLUE);
  	ST7789V_DispStringLine_EN(LINE(2),"Draw line:");
	LCD_SetTextColor(RED);
  	ST7789V_DrawLine(50,170,210,230);  
  	ST7789V_DrawLine(50,200,210,240);
	LCD_SetTextColor(GREEN);
  	ST7789V_DrawLine(100,170,200,230);  
  	ST7789V_DrawLine(200,200,220,240);
	LCD_SetTextColor(BLUE);
  	ST7789V_DrawLine(110,170,110,230);  
  	ST7789V_DrawLine(130,200,220,240);
  	Delay(0xFFFFFF);
  	ST7789V_Clear(0,16*8,LCD_X_LENGTH,LCD_Y_LENGTH-16*8);	
  	LCD_ClearLine(LINE(2));
	LCD_SetTextColor(BLUE);
 	ST7789V_DispStringLine_EN(LINE(2),"Draw Rect:");
	LCD_SetTextColor(RED);
  	ST7789V_DrawRectangle(50,200,100,30,1);
	LCD_SetTextColor(GREEN);
  	ST7789V_DrawRectangle(160,200,20,40,0);
	LCD_SetTextColor(BLUE);
  	ST7789V_DrawRectangle(170,200,50,20,1);
  	Delay(0xFFFFFF);
	ST7789V_Clear(0,16*8,LCD_X_LENGTH,LCD_Y_LENGTH-16*8);
  	LCD_ClearLine(LINE(2));
	LCD_SetTextColor(BLUE);
  	ST7789V_DispStringLine_EN(LINE(2),"Draw Cir:");
	LCD_SetTextColor(RED);
  	ST7789V_DrawCircle(100,200,20,0);
	LCD_SetTextColor(GREEN);
  	ST7789V_DrawCircle(100,200,10,1);
	LCD_SetTextColor(BLUE);
	ST7789V_DrawCircle(140,200,20,0);
  	Delay(0xFFFFFF);
  	ST7789V_Clear(0,16*8,LCD_X_LENGTH,LCD_Y_LENGTH-16*8);
}
static void Delay ( __IO uint32_t nCount )
{
  for ( ; nCount != 0; nCount -- );	
}
