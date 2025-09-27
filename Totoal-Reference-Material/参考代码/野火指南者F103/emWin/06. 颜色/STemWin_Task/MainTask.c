/**
  ***********************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   FreeRTOS v9.0.0 + STemWIN 5.44a
  ***********************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F103 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ***********************************************************************
  */
/*******************************************************************************
 * 包含的头文件
 ******************************************************************************/
#include "MainTask.h"

/*
*************************************************************************
*                                宏定义
*************************************************************************
*/
/* 起始坐标 */
#define X_START 60
#define Y_START 40

/*******************************************************************************
 * 全局变量
 ******************************************************************************/
/* */
typedef struct {
  int NumBars;

  GUI_COLOR Color;
  const char * s;
} BAR_DATA;

static const BAR_DATA _aBarData[] = {
  { 2, GUI_RED    , "Red" },
  { 2, GUI_GREEN  , "Green" },
  { 2, GUI_BLUE   , "Blue" },
  { 1, GUI_WHITE  , "Grey" },
  { 2, GUI_YELLOW , "Yellow" },
  { 2, GUI_CYAN   , "Cyan" },
  { 2, GUI_MAGENTA, "Magenta" },
};

static const GUI_COLOR _aColorStart[] = { GUI_BLACK, GUI_WHITE };

/*
*************************************************************************
*                                 函数
*************************************************************************
*/
/**
  * @brief 色条显示函数
  * @note 无
  * @param 无
  * @retval 无
  */
static void _DemoShowColorBar(void) 
{
	GUI_RECT Rect;
	int      yStep;
	int      i;
	int      j;
	int      xSize;
	int      ySize;
	int      NumBars;
	int      NumColors;

	xSize = LCD_GetXSize();
	ySize = LCD_GetYSize();
	
	/* 可以显示的色条数 */
	NumColors = GUI_COUNTOF(_aBarData);
	for (i = NumBars = 0, NumBars = 0; i < NumColors; i++) 
	{
		NumBars += _aBarData[i].NumBars;
	}
	yStep = (ySize - Y_START) / NumBars;
	
	/* 显示文本 */
	Rect.x0 = 0;
	Rect.x1 = X_START - 1;
	Rect.y0 = Y_START;
	GUI_SetFont(&GUI_Font16B_ASCII);
	for (i = 0; i < NumColors; i++) 
	{
		Rect.y1 = Rect.y0 + yStep * _aBarData[i].NumBars - 1;
		GUI_DispStringInRect(_aBarData[i].s, &Rect, GUI_TA_LEFT | GUI_TA_VCENTER);
		Rect.y0 = Rect.y1 + 1;
	}
	
  /* 绘制色条 */
	Rect.x0 = X_START;
	Rect.x1 = xSize - 1;
	Rect.y0 = Y_START;
	for (i = 0; i < NumColors; i++) 
	{
		for (j = 0; j < _aBarData[i].NumBars; j++) 
		{
			Rect.y1 = Rect.y0 + yStep - 1;
			GUI_DrawGradientH(Rect.x0, Rect.y0, Rect.x1, Rect.y1, _aColorStart[j], _aBarData[i].Color);
			Rect.y0 = Rect.y1 + 1;
		}
	}
}

/**
  * @brief GUI主任务
  * @note 无
  * @param 无
  * @retval 无
  */
void MainTask(void)
{
  /* 设置背景色 */
	GUI_SetBkColor(GUI_BLACK);
	GUI_Clear();
	
	/* 设置前景色、字体大小 */
	GUI_SetColor(GUI_RED);
	GUI_SetFont(&GUI_Font24B_ASCII);
	
	/* 显示色条 */
	_DemoShowColorBar();
	
	while(1) 
	{
		GUI_Delay(10);
	}
}
