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
File        : WM_Sample.c
Purpose     : Demonstrates the window manager
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

#include <string.h>

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
/* 自定义消息ID */
#define MSG_CHANGE_TEXT (WM_USER + 0)
/* 移动速度 */
#define SPEED           1500

/*******************************************************************
*
*       Static variables
*
********************************************************************
*/
/* 文本缓冲去 */
static char _acInfoText[40];

/* 颜色 */
static GUI_COLOR _WindowColor1 = GUI_GREEN;
static GUI_COLOR _FrameColor1  = GUI_BLUE;
static GUI_COLOR _WindowColor2 = GUI_RED;
static GUI_COLOR _FrameColor2  = GUI_YELLOW;
static GUI_COLOR _ChildColor   = GUI_YELLOW;
static GUI_COLOR _ChildFrame   = GUI_BLACK;

/* 回调函数 */
static WM_CALLBACK * _cbBkWindowOld;

/* 句柄*/
static WM_HWIN _hWindow1;
static WM_HWIN _hWindow2;
static WM_HWIN _hChild;

/*******************************************************************
*
*       Static code, helper functions
*
********************************************************************
*/
/**
  * @brief 将自定义消息发送到后台窗口并使其无效，因此后台窗口的回调显示新文本。
  * @note 无
  * @param 无
  * @retval 无
  */
static void _ChangeInfoText(char * pStr)
{
  WM_MESSAGE Message;

  Message.MsgId  = MSG_CHANGE_TEXT;
  Message.Data.p = pStr;
  WM_SendMessage(WM_HBKWIN, &Message);
  WM_InvalidateWindow(WM_HBKWIN);
}

/**
  * @brief 直接在显示屏上绘制信息文本
  * @note 此功能适用于未设置回调函数的时候
  * @param 无
  * @retval 无
  */
static void _DrawInfoText(char * pStr)
{
  GUI_SetColor(GUI_WHITE);
  GUI_SetFont(&GUI_Font16_ASCII);
  GUI_DispStringHCenterAt("WindowManager - Sample", 120, 5);
  GUI_SetFont(&GUI_Font8x16);
  GUI_DispStringAtCEOL(pStr, 5, 40);
}

/*******************************************************************
*
*       _LiftUp
*/
static void _LiftUp(int dy)
{
  int i;
  int tm;

  for (i = 0; i < (dy/4); i++)
	{
    tm = GUI_GetTime();
    WM_MoveWindow(_hWindow1, 0, -4);
    WM_MoveWindow(_hWindow2, 0, -4);
    while ((GUI_GetTime() - tm) < 20)
		{
      WM_Exec();
    }
  }
}

/*******************************************************************
*
*       _LiftDown
*/
static void _LiftDown(int dy)
{
  int i;
  int tm;

  for (i = 0; i < (dy/4); i++)
	{
    tm = GUI_GetTime();
    WM_MoveWindow(_hWindow1, 0, 4);
    WM_MoveWindow(_hWindow2, 0, 4);
    while ((GUI_GetTime() - tm) < 20)
		{
      WM_Exec();
    }
  }
}

/*******************************************************************
*
*       Static code, callbacks for windows
*
********************************************************************
*/
/**
  * @brief 背景窗口回调函数
  * @note 无
  * @param 无
  * @retval 无
  */
static void _cbBkWindow(WM_MESSAGE * pMsg)
{
  switch (pMsg->MsgId)
	{
		case MSG_CHANGE_TEXT:
			strcpy(_acInfoText, (char const *)pMsg->Data.p);
		case WM_PAINT:
			GUI_SetBkColor(GUI_BLACK);
			GUI_Clear();
			GUI_SetColor(GUI_WHITE);
			GUI_SetFont(&GUI_Font20_ASCII);
			GUI_DispStringHCenterAt("WindowManager - Sample", 120, 5);
			GUI_SetFont(&GUI_Font8x16);
			GUI_DispStringAt(_acInfoText, 5, 40);
			break;
		default:
			WM_DefaultProc(pMsg);
  }
}

/**
  * @brief 窗口1回调函数
  * @note 无
  * @param 无
  * @retval 无
  */
static void _cbWindow1(WM_MESSAGE * pMsg)
{
  GUI_RECT Rect;
  int      x;
  int      y;

  switch (pMsg->MsgId)
	{
		case WM_PAINT:
			WM_GetInsideRect(&Rect);
			GUI_SetBkColor(_WindowColor1);
			GUI_SetColor(_FrameColor1);
			GUI_ClearRectEx(&Rect);
			GUI_DrawRectEx(&Rect);
			GUI_SetColor(GUI_WHITE);
			GUI_SetFont(&GUI_Font24_ASCII);
			x = WM_GetWindowSizeX(pMsg->hWin);
			y = WM_GetWindowSizeY(pMsg->hWin);
			GUI_DispStringHCenterAt("Window 1", x / 2, (y / 2) - 12);
			break;
		default:
			WM_DefaultProc(pMsg);
  }
}

/**
  * @brief 窗口2回调函数
  * @note 无
  * @param 无
  * @retval 无
  */
static void _cbWindow2(WM_MESSAGE * pMsg)
{
  GUI_RECT Rect;
  int      x;
  int      y;

  switch (pMsg->MsgId)
	{
		case WM_PAINT:
			WM_GetInsideRect(&Rect);
			GUI_SetBkColor(_WindowColor2);
			GUI_SetColor(_FrameColor2);
			GUI_ClearRectEx(&Rect);
			GUI_DrawRectEx(&Rect);
			GUI_SetColor(GUI_WHITE);
			GUI_SetFont(&GUI_Font24_ASCII);
			x = WM_GetWindowSizeX(pMsg->hWin);
			y = WM_GetWindowSizeY(pMsg->hWin);
			GUI_DispStringHCenterAt("Window 2", x / 2, (y / 4) - 12);
			break;
		default:
			WM_DefaultProc(pMsg);
  }
}

/**
  * @brief 子窗口回调函数
  * @note 无
  * @param 无
  * @retval 无
  */
static void _cbChild(WM_MESSAGE * pMsg)
{
  GUI_RECT Rect;
  int      x;
  int      y;

  switch (pMsg->MsgId)
	{
		case WM_PAINT:
			WM_GetInsideRect(&Rect);
			GUI_SetBkColor(_ChildColor);
			GUI_SetColor(_ChildFrame);
			GUI_ClearRectEx(&Rect);
			GUI_DrawRectEx(&Rect);
			GUI_SetColor(GUI_RED);
			GUI_SetFont(&GUI_Font24_ASCII);
			x = WM_GetWindowSizeX(pMsg->hWin);
			y = WM_GetWindowSizeY(pMsg->hWin);
			GUI_DispStringHCenterAt("Child window", x / 2, (y / 2) - 12);
			break;
		default:
			WM_DefaultProc(pMsg);
  }
}

/**
  * @brief 窗口1的另一个回调函数
  * @note 无
  * @param 无
  * @retval 无
  */
static void _cbDemoCallback1(WM_MESSAGE * pMsg)
{
  int x;
  int y;

  switch (pMsg->MsgId)
	{
		case WM_PAINT:
			GUI_SetBkColor(GUI_GREEN);
			GUI_Clear();
			GUI_SetColor(GUI_RED);
			GUI_SetFont(&GUI_FontComic18B_1);
			x = WM_GetWindowSizeX(pMsg->hWin);
			y = WM_GetWindowSizeY(pMsg->hWin);
			GUI_DispStringHCenterAt("Window 1\nanother Callback", x / 2, (y / 2) - 18);
			break;
		default:
			WM_DefaultProc(pMsg);
  }
}

/**
  * @brief 窗口2的另一个回调函数
  * @note 无
  * @param 无
  * @retval 无
  */
static void _cbDemoCallback2(WM_MESSAGE * pMsg)
{
  int x;
  int y;

  switch (pMsg->MsgId)
	{
		case WM_PAINT:
			GUI_SetBkColor(GUI_MAGENTA);
			GUI_Clear();
			GUI_SetColor(GUI_YELLOW);
			GUI_SetFont(&GUI_FontComic18B_1);
			x = WM_GetWindowSizeX(pMsg->hWin);
			y = WM_GetWindowSizeY(pMsg->hWin);
			GUI_DispStringHCenterAt("Window 2\nanother Callback", x / 2, (y / 4) - 18);
			break;
		default:
			WM_DefaultProc(pMsg);
  }
}

/*******************************************************************
*
*       Static code, functions for demo
*
********************************************************************
*/
/**
  * @brief 演示WM_SetDesktopColor的使用
  * @note 无
  * @param 无
  * @retval 无
  */
static void _DemoSetDesktopColor(void)
{
  GUI_SetBkColor(GUI_BLUE);
  GUI_Clear();
  _DrawInfoText("WM_SetDesktopColor()");
  GUI_Delay(SPEED*3/2);
  WM_SetDesktopColor(GUI_BLACK);
  GUI_Delay(SPEED/2);
  /* 设置背景颜色并使桌面颜色无效，这是后续重绘演示所必需的 */
  GUI_SetBkColor(GUI_BLACK);
  WM_SetDesktopColor(GUI_INVALID_COLOR);
}

/**
  * @brief 演示WM_CreateWindow的使用
  * @note 无
  * @param 无
  * @retval 无
  */
static void _DemoCreateWindow(void)
{
  /* 重定向背景窗口回调函数 */
  _cbBkWindowOld = WM_SetCallback(WM_HBKWIN, _cbBkWindow);
  /* 创建窗口 */
  _ChangeInfoText("WM_CreateWindow()");
  GUI_Delay(SPEED);
  _hWindow1 = WM_CreateWindow( 50,  70, 120, 70, WM_CF_SHOW | WM_CF_MEMDEV, _cbWindow1, 0);
  GUI_Delay(SPEED/3);
  _hWindow2 = WM_CreateWindow(105, 125, 120, 70, WM_CF_SHOW | WM_CF_MEMDEV, _cbWindow2, 0);
  GUI_Delay(SPEED);
}

/**
  * @brief 演示WM_CreateWindowAsChild的使用
  * @note 无
  * @param 无
  * @retval 无
  */
static void _DemoCreateWindowAsChild(void)
{
  /* 创建子窗口 */
  _ChangeInfoText("WM_CreateWindowAsChild()");
  GUI_Delay(SPEED);
  _hChild = WM_CreateWindowAsChild(10, 30, 100, 40, _hWindow2, WM_CF_SHOW | WM_CF_MEMDEV, _cbChild, 0);
  GUI_Delay(SPEED);
}

/**
  * @brief 演示WM_InvalidateWindow的使用
  * @note 无
  * @param 无
  * @retval 无
  */
static void _DemoInvalidateWindow(void)
{
  _ChangeInfoText("WM_InvalidateWindow()");
  _WindowColor1 = GUI_BLUE;
  _FrameColor1  = GUI_GREEN;
  GUI_Delay(SPEED);
  WM_InvalidateWindow(_hWindow1);
  GUI_Delay(SPEED);
}

/**
  * @brief 演示WM_BringToTop的使用
  * @note 无
  * @param 无
  * @retval 无
  */
static void _DemoBringToTop(void)
{
  _ChangeInfoText("WM_BringToTop()");
  GUI_Delay(SPEED);
  WM_BringToTop(_hWindow1);    
  GUI_Delay(SPEED);
}

/**
  * @brief 演示WM_MoveTo的使用
  * @note 无
  * @param 无
  * @retval 无
  */
static void _DemoMoveTo(void)
{
  int i;
  int tm;
  int tDiff;

  _ChangeInfoText("WM_MoveTo()");
  GUI_Delay(SPEED);
  for (i = 1; i < 56; i++)
	{
    tm = GUI_GetTime();
    WM_MoveTo(_hWindow1,  50 + i,  70 + i);
    WM_MoveTo(_hWindow2, 105 - i, 125 - i);
    tDiff = 15 - (GUI_GetTime() - tm);
    GUI_Delay(tDiff);
  }
  for (i = 1; i < 56; i++)
	{
    tm = GUI_GetTime();
    WM_MoveTo(_hWindow1, 105 - i, 125 - i);
    WM_MoveTo(_hWindow2,  50 + i,  70 + i);
    tDiff = 15 - (GUI_GetTime() - tm);
    GUI_Delay(tDiff);
  }
  GUI_Delay(SPEED);
}

/**
  * @brief 演示WM_BringToBottom的使用
  * @note 无
  * @param 无
  * @retval 无
  */
static void _DemoBringToBottom(void)
{
  _ChangeInfoText("WM_BringToBottom()");
  GUI_Delay(SPEED);
  WM_BringToBottom(_hWindow1);
  GUI_Delay(SPEED);
}

/**
  * @brief 演示WM_MoveWindow的使用
  * @note 无
  * @param 无
  * @retval 无
  */
static void _DemoMoveWindow(void)
{
  int i;
  int tm;
  int tDiff;

  _ChangeInfoText("WM_MoveWindow()");
  GUI_Delay(SPEED);
  for (i = 0; i < 55; i++)
	{
    tm = GUI_GetTime();
    WM_MoveWindow(_hWindow1,  1,  1);
    WM_MoveWindow(_hWindow2, -1, -1);
    tDiff = 15 - (GUI_GetTime() - tm);
    GUI_Delay(tDiff);
  }
  for (i = 0; i < 55; i++)
	{
    tm = GUI_GetTime();
    WM_MoveWindow(_hWindow1, -1, -1);
    WM_MoveWindow(_hWindow2,  1,  1);
    tDiff = 15 - (GUI_GetTime() - tm);
    GUI_Delay(tDiff);
  }
  GUI_Delay(SPEED);
}

/**
  * @brief 演示在父窗口中WM_HideWindow和WM_ShowWindow的使用
  * @note 无
  * @param 无
  * @retval 无
  */
static void _DemoHideShowParent(void)
{
  _ChangeInfoText("WM_HideWindow(Parent)");
  GUI_Delay(SPEED);
  WM_HideWindow(_hWindow2);
  GUI_Delay(SPEED/3);
  WM_HideWindow(_hWindow1);
  GUI_Delay(SPEED);
  _ChangeInfoText("WM_ShowWindow(Parent)");
  GUI_Delay(SPEED);
  WM_ShowWindow(_hWindow1);
  GUI_Delay(SPEED/3);
  WM_ShowWindow(_hWindow2);
  GUI_Delay(SPEED);
}

/**
  * @brief 演示在子窗口中WM_HideWindow和WM_ShowWindow的使用
  * @note 无
  * @param 无
  * @retval 无
  */
static void _DemoHideShowChild(void)
{
  _ChangeInfoText("WM_HideWindow(Child)");
  GUI_Delay(SPEED);
  WM_HideWindow(_hChild);
  GUI_Delay(SPEED);
  _ChangeInfoText("WM_ShowWindow(Child)");
  GUI_Delay(SPEED);
  WM_ShowWindow(_hChild);
  GUI_Delay(SPEED);
}

/**
  * @brief 演示父边界的裁剪
  * @note 无
  * @param 无
  * @retval 无
  */
static void _DemoClipping(void)
{
  int i;
  int tm;
  int tDiff;

  _ChangeInfoText("Demonstrating clipping of child");
  GUI_Delay(SPEED);
  for (i = 0; i < 25; i++)
	{
    tm = GUI_GetTime();
    WM_MoveWindow(_hChild,  1,  0);
    tDiff = 15 - (GUI_GetTime() - tm);
    GUI_Delay(tDiff);
  }
  for (i = 0; i < 25; i++)
	{
    tm = GUI_GetTime();
    WM_MoveWindow(_hChild,  0,  1);
    tDiff = 15 - (GUI_GetTime() - tm);
    GUI_Delay(tDiff);
  }
  for (i = 0; i < 50; i++)
	{
    tm = GUI_GetTime();
    WM_MoveWindow(_hChild, -1,  0);
    tDiff = 15 - (GUI_GetTime() - tm);
    GUI_Delay(tDiff);
  }
  for (i = 0; i < 25; i++)
	{
    tm = GUI_GetTime();
    WM_MoveWindow(_hChild,  0, -1);
    tDiff = 15 - (GUI_GetTime() - tm);
    GUI_Delay(tDiff);
  }
  for (i = 0; i < 25; i++)
	{
    tm = GUI_GetTime();
    WM_MoveWindow(_hChild,  1,  0);
    tDiff = 15 - (GUI_GetTime() - tm);
    GUI_Delay(tDiff);
  }
  GUI_Delay(SPEED);
}

/**
  * @brief 演示回调函数的使用
  * @note 无
  * @param 无
  * @retval 无
  */
static void _DemoRedrawing(void)
{
  int i;
  int tm;
  int tDiff;

  _ChangeInfoText("Demonstrating redrawing");
  GUI_Delay(SPEED);
  _LiftUp(40);
  GUI_Delay(SPEED/3);
  _ChangeInfoText("Using a callback for redrawing");
  GUI_Delay(SPEED/3);
  for (i = 0; i < 55; i++)
	{
    tm = GUI_GetTime();
    WM_MoveWindow(_hWindow1,  1,  1);
    WM_MoveWindow(_hWindow2, -1, -1);
    tDiff = 15 - (GUI_GetTime() - tm);
    GUI_Delay(tDiff);
  }
  for (i = 0; i < 55; i++)
	{
    tm = GUI_GetTime();
    WM_MoveWindow(_hWindow1, -1, -1);
    WM_MoveWindow(_hWindow2,  1,  1);
    tDiff = 15 - (GUI_GetTime() - tm);
    GUI_Delay(tDiff);
  }
  GUI_Delay(SPEED/4);
  _LiftDown(30);
  GUI_Delay(SPEED/2);
  _ChangeInfoText("Without redrawing");
  GUI_Delay(SPEED);
  _LiftUp(30);
  GUI_Delay(SPEED/4);
  WM_SetCallback(WM_HBKWIN, _cbBkWindowOld);
  for (i = 0; i < 55; i++)
	{
    tm = GUI_GetTime();
    WM_MoveWindow(_hWindow1,  1,  1);
    WM_MoveWindow(_hWindow2, -1, -1);
    tDiff = 15 - (GUI_GetTime() - tm);
    GUI_Delay(tDiff);
  }
  for (i = 0; i < 55; i++)
	{
    tm = GUI_GetTime();
    WM_MoveWindow(_hWindow1, -1, -1);
    WM_MoveWindow(_hWindow2,  1,  1);
    tDiff = 15 - (GUI_GetTime() - tm);
    GUI_Delay(tDiff);
  }
  GUI_Delay(SPEED/3);
  WM_SetCallback(WM_HBKWIN, _cbBkWindow);
  _LiftDown(40);
  GUI_Delay(SPEED);
}

/**
  * @brief 演示WM_ResizeWindow的使用
  * @note 无
  * @param 无
  * @retval 无
  */
static void _DemoResizeWindow(void)
{
  int i;
  int tm;
  int tDiff;

  _ChangeInfoText("WM_ResizeWindow()");
  GUI_Delay(SPEED);
  _LiftUp(30);
  for (i = 0; i < 20; i++)
	{
    tm = GUI_GetTime();
    WM_ResizeWindow(_hWindow1,  1,  1);
    WM_ResizeWindow(_hWindow2, -1, -1);
    tDiff = 15 - (GUI_GetTime() - tm);
    GUI_Delay(tDiff);
  }
  for (i = 0; i < 40; i++)
	{
    tm = GUI_GetTime();
    WM_ResizeWindow(_hWindow1, -1, -1);
    WM_ResizeWindow(_hWindow2,  1,  1);
    tDiff = 15 - (GUI_GetTime() - tm);
    GUI_Delay(tDiff);
  }
  for (i = 0; i < 20; i++)
	{
    tm = GUI_GetTime();
    WM_ResizeWindow(_hWindow1,  1,  1);
    WM_ResizeWindow(_hWindow2, -1, -1);
    tDiff = 15 - (GUI_GetTime() - tm);
    GUI_Delay(tDiff);
  }
  _LiftDown(30);
  GUI_Delay(SPEED);
}

/**
  * @brief 演示WM_SetCallback的使用
  * @note 无
  * @param 无
  * @retval 无
  */
static void _DemoSetCallback(void)
{
  _ChangeInfoText("WM_SetCallback()");
  GUI_Delay(SPEED);
  WM_SetCallback(_hWindow1, _cbDemoCallback1);
  WM_InvalidateWindow(_hWindow1);
  GUI_Delay(SPEED/2);
  WM_SetCallback(_hWindow2, _cbDemoCallback2);
  WM_InvalidateWindow(_hWindow2);
  GUI_Delay(SPEED*3);
  WM_SetCallback(_hWindow1, _cbWindow1);
  WM_InvalidateWindow(_hWindow1);
  GUI_Delay(SPEED/2);
  WM_SetCallback(_hWindow2, _cbWindow2);
  WM_InvalidateWindow(_hWindow2);
  GUI_Delay(SPEED);
}

/**
  * @brief 演示WM_DeleteWindow的使用
  * @note 无
  * @param 无
  * @retval 无
  */
static void _DemoDeleteWindow(void)
{
  _ChangeInfoText("WM_DeleteWindow()");
  GUI_Delay(SPEED);
  WM_DeleteWindow(_hWindow2);
  GUI_Delay(SPEED/3);
  WM_DeleteWindow(_hWindow1);
  GUI_Delay(SPEED);
  _ChangeInfoText("");
  GUI_Delay(SPEED);
  /* 还原背景窗口回调函数和颜色 */
  WM_SetCallback(WM_HBKWIN, _cbBkWindowOld);
  _WindowColor1 = GUI_GREEN;
  _WindowColor2 = GUI_RED;
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
  GUI_SetBkColor(GUI_BLACK);
  WM_SetCreateFlags(WM_CF_MEMDEV);
  WM_EnableMemdev(WM_HBKWIN);
  while (1)
	{
    /*由于内存限制，部分demo运行不稳定*/
    _DemoSetDesktopColor();
    _DemoCreateWindow();
//  _DemoCreateWindowAsChild();
    _DemoInvalidateWindow();
    _DemoBringToTop();
//  _DemoMoveTo();
    _DemoBringToBottom();
//  _DemoMoveWindow();
    _DemoHideShowParent();
    _DemoHideShowChild();
    _DemoClipping();
    _DemoRedrawing();
    _DemoResizeWindow();
    _DemoSetCallback();
    _DemoDeleteWindow();
  }
}

/*************************** End of file ****************************/
