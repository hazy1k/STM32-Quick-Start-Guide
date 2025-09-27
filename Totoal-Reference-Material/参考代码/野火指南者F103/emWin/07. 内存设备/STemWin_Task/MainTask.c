/**
  *********************************************************************
  * @file    MainTask.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   FreeRTOS v9.0.0 + STM32 工程模版
  *********************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F103 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
/*******************************************************************************
 * 包含的头文件
 ******************************************************************************/

#include <stdio.h>

#include "GUI.h"
#include "DIALOG.h"
#include "MainTask.h"
/* FreeRTOS头文件 */
#include "FreeRTOS.h"
#include "task.h"

/*******************************************************************
*
*       static code
*
********************************************************************
*/
/**
  * @brief 绘图函数
  * @note 无
  * @param 
  * @retval 无
  */
static void _Draw(int x0, int y0, int x1, int y1, int i)
{
  char buf[] = {0};

  /* 绘制矩形背景 */
  GUI_SetColor(GUI_BLUE);
	GUI_FillRect(x0, y0, x1, y1);
  
  /* 绘制文本 */
	GUI_SetFont(GUI_FONT_D64);
  GUI_SetTextMode(GUI_TEXTMODE_XOR);
  sprintf(buf, "%d", i);
	GUI_DispStringHCenterAt(buf, x0 + (x1 - x0)/2, (y0 + (y1 - y0)/2) - 32);
}

/**
  * @brief 内存设备演示函数
  * @note 无
  * @param 无
  * @retval 无
  */
static void _DemoMemDev(void)
{
  GUI_MEMDEV_Handle hMem = 0;
	int i = 0;
  
	/* 设置背景色 */
  GUI_SetBkColor(GUI_BLACK);
  GUI_Clear();
  
	/* 显示提示文字 */
  GUI_SetColor(GUI_WHITE);
  GUI_SetFont(GUI_FONT_20_ASCII);
  GUI_DispStringHCenterAt("MEMDEV_MemDev - Sample", 120, 5);
  GUI_SetFont(GUI_FONT_16_ASCII);
  GUI_DispStringHCenterAt("Shows the advantage of using a\nmemorydevice", 120, 45);
  GUI_SetFont(GUI_FONT_13_ASCII);
  GUI_DispStringHCenterAt("Draws the number\nwithout a\nmemory device", 65, 220);
  GUI_DispStringHCenterAt("Draws the number\nusing a\nmemory device", 175, 220);
  
  /* 创建内存设备 */
  hMem = GUI_MEMDEV_Create(125, 100, 100, 100);
  
	while (1)
	{
    /* 直接绘制 */
    _Draw(15, 100, 115, 200, i);
    
    /* 激活内存设备 */
    GUI_MEMDEV_Select(hMem);
    /* 向内存设备中绘制图形 */
    _Draw(125, 100, 225, 200, i);
    /* 选择LCD */
    GUI_MEMDEV_Select(0);
    /* 将内存设备中的内容复制到LCD */
    GUI_MEMDEV_CopyToLCDAt(hMem, 125, 100);

		GUI_Delay(40);
    i++;
		if (i > 99)
		{
			i = 0;
		}
	}
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/**
  * @brief GUI主任务
  * @note 无
  * @param 无
  * @retval 无
  */
void MainTask(void)
{
	/* 运行内存设备演示DEMO */
  _DemoMemDev();
}

/*************************** End of file ****************************/
