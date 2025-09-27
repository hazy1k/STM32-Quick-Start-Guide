/*********************************************************************
*                    SEGGER Microcontroller GmbH                     *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2018  SEGGER Microcontroller GmbH                *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.48 - Graphical user interface for embedded applications **
emWin is protected by international copyright laws.   Knowledge of the
source code may not be used to write a similar product.  This file may
only  be used  in accordance  with  a license  and should  not be  re-
distributed in any way. We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : WM_Redraw.c
Purpose     : Demonstrates the redrawing mechanism of the window manager
Requirements: WindowManager - (x)
              MemoryDevices - ( )
              AntiAliasing  - ( )
              VNC-Server    - ( )
              PNG-Library   - ( )
              TrueTypeFonts - ( )

----------------------------------------------------------------------
*/
#include "WM.h"
#include "GUI.h"
#include "DIALOG.h"
#include "MainTask.h"
/* FreeRTOS头文件 */
#include "FreeRTOS.h"
#include "task.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/

/*******************************************************************
*
*       static code
*
********************************************************************
*/
/**
  * @brief 背景窗口回调函数
  * @note pMsg：消息指针
  * @param 无
  * @retval 无
  */
static void _cbBkWindow(WM_MESSAGE* pMsg)
{
  switch (pMsg->MsgId)
	{
		case WM_PAINT:
			GUI_ClearRect(0, 50, 319, 239);
		default:
			WM_DefaultProc(pMsg);
  }
}

/**
  * @brief 窗口回调函数
  * @note pMsg：消息指针
  * @param 无
  * @retval 无
  */
static void _cbWindow(WM_MESSAGE* pMsg)
{
	GUI_RECT Rect;

  switch (pMsg->MsgId)
	{
		case WM_PAINT:
      /* 返回窗口客户区坐标 */
			WM_GetInsideRect(&Rect);
      /* 设置窗口背景颜色 */
			GUI_SetBkColor(GUI_RED);
      /* 设置前景颜色 */
			GUI_SetColor(GUI_YELLOW);
      /* 绘制窗口 */    
			GUI_ClearRectEx(&Rect);
			GUI_DrawRectEx(&Rect);
      /* 设置文本颜色 */
			GUI_SetColor(GUI_BLACK);
      /* 设置文本格式 */
			GUI_SetFont(&GUI_Font8x16);
      /* 显示提示信息 */
			GUI_DispStringHCenterAt("Foreground window", 75, 40);
			break;
		default:
			WM_DefaultProc(pMsg);
  }
}

/**
  * @brief 窗口移动函数
  * @note 无
  * @param 无
  * @retval 无
  */
static void _MoveWindow(const char* pText)
{
  WM_HWIN hWnd;
  int     i;

  /* 创建前景窗口 */
  hWnd = WM_CreateWindow(10, 50, 150, 100, WM_CF_SHOW, _cbWindow, 0);
  GUI_Delay(500);
  /* 移动前景窗口 */
  for (i = 0; i < 40; i++)
	{
    WM_MoveWindow(hWnd, 2, 2);
    GUI_Delay(10);
  }
  /* 移动结束后显示提示文字 */
  if (pText)
	{
    GUI_DispStringAt(pText, 5, 50);
    GUI_Delay(2500);
  }
  /* 删除前景窗口 */
  WM_DeleteWindow(hWnd);
  WM_Invalidate(WM_HBKWIN);
  GUI_Exec();
}

/**
  * @brief 窗口重绘DEMO
  * @note 无
  * @param 无
  * @retval 无
  */
static void _DemoRedraw(void)
{
  WM_CALLBACK * _cbOldBk;

  GUI_SetBkColor(GUI_BLACK);
  GUI_Clear();
  GUI_SetColor(GUI_WHITE);
  GUI_SetFont(&GUI_Font20_ASCII);
  GUI_DispStringHCenterAt("WM_Redraw - Sample", 120, 5);
  GUI_SetFont(&GUI_Font8x16);
  while(1)
	{
    /* 在背景上移动窗口 */
    _MoveWindow("Background has not been redrawn");
    /* 清除背景 */
    GUI_ClearRect(0, 50, 319, 239);
    GUI_Delay(1000);
    /* 重定向背景窗口的回调函数 */
    _cbOldBk = WM_SetCallback(WM_HBKWIN, _cbBkWindow);
    /* 在背景上移动窗口 */
    _MoveWindow("Background has been redrawn");
    /* 还原背景窗口的回调函数 */
    WM_SetCallback(WM_HBKWIN, _cbOldBk);
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
  _DemoRedraw();
}

/*************************** End of file ****************************/
