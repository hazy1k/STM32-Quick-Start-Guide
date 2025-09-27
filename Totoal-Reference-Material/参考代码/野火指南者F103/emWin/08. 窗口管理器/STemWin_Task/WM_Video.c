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
File        : WM_Video.c
Purpose     : The sample shows how to find out if a window is
              completely visible and/or covered.
Requirements: WindowManager - (x)
              MemoryDevices - ( )
              AntiAliasing  - ( )
              VNC-Server    - ( )
              PNG-Library   - ( )
              TrueTypeFonts - ( )
----------------------------------------------------------------------
*/

#include <stddef.h>
#include <stdio.h>

#include "WM.h"
#include "GUI.h"
#include "FRAMEWIN.h"
#include "BUTTON.h"
#include "TEXT.h"
#include "MainTask.h"
/* FreeRTOS头文件 */
#include "FreeRTOS.h"
#include "task.h"

static void _cbFrameWinTest(WM_MESSAGE * pMsg);

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define MAX_WINDOWS   50

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

static WM_HWIN _ahWin[MAX_WINDOWS];
static char    _IsCompletelyVis;

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/**
  * @brief 窗口创建函数
  * @note 无
  * @param 无
  * @retval 无
  */
static void _CreateWindow(void)
{
  unsigned i;
  WM_HWIN  hWin;
  char     ac[32];
  int      j;

  for (i = 0; i < MAX_WINDOWS; i++)
	{
    if (_ahWin[i] == 0)
		{
      j = i + 1;
      sprintf(ac, "Test window %d", j);
      hWin = FRAMEWIN_CreateEx(5 + 10 * i, 135 + 10 * i, 120, 60, 0, WM_CF_SHOW, FRAMEWIN_CF_MOVEABLE, 0, ac, _cbFrameWinTest);
      FRAMEWIN_SetClientColor(hWin, GUI_INVALID_COLOR);
      _ahWin[i] = hWin;
      break;
    }
  }
}

/**
  * @brief 窗口删除函数
  * @note 无
  * @param 无
  * @retval 无
  */
static void _DeleteWindow(void)
{
  unsigned i;

  for (i = 0; i < MAX_WINDOWS; i++)
	{
    if (WM_IsVisible(_ahWin[i]))
		{
      WM_DeleteWindow(_ahWin[i]);
      _ahWin[i] = 0;
      break;
    }
  }
}

/**
  * @brief 窗口显示函数
  * @note 无
  * @param 无
  * @retval 无
  */
static void _ShowWindow(void)
{
  unsigned i;

  for (i = 0; i < MAX_WINDOWS; i++)
	{
    if (_ahWin[i] != 0) {
      if (WM_IsVisible(_ahWin[i]) == 0)
			{
        WM_ShowWindow(_ahWin[i]);
        break;
      }
    }
  }
}

/**
  * @brief 窗口隐藏函数
  * @note 无
  * @param 无
  * @retval 无
  */
static void _HideWindow(void)
{
  unsigned i;

  for (i = 0; i < MAX_WINDOWS; i++)
	{
    if (WM_IsVisible(_ahWin[i]))
		{
      WM_HideWindow(_ahWin[i]);
      break;
    }
  }
}

/*********************************************************************
*
*       Static code, callbacks
*
**********************************************************************
*/
/**
  * @brief 背景窗口回调函数
  * @note pMsg：消息指针
  * @param 无
  * @retval 无
  */
static void _cbBkWin(WM_MESSAGE* pMsg)
{
  int Id;

  switch (pMsg->MsgId)
	{
		case WM_PAINT:
			GUI_SetBkColor(GUI_BLACK);
			GUI_Clear();
			break;
		case WM_NOTIFY_PARENT:
			if (pMsg->Data.v == WM_NOTIFICATION_RELEASED)
			{
				Id = WM_GetId(pMsg->hWinSrc);
				switch (Id) {
				case GUI_ID_BUTTON0:
					/*由于内存限制，无法增加窗口*/
//					_CreateWindow();
					break;
				case GUI_ID_BUTTON1:
					_DeleteWindow();
					break;
				case GUI_ID_BUTTON2:
					_ShowWindow();
					break;
				case GUI_ID_BUTTON3:
					_HideWindow();
					break;
				}
			}
			break;
		default:
			WM_DefaultProc(pMsg);
  }
}

/**
  * @brief 框架窗口回调函数
  * @note pMsg：消息指针
  * @param 无
  * @retval 无
  */
static void _cbFrameWinVideo(WM_MESSAGE* pMsg)
{
  WM_HWIN hWin;
  WM_HWIN hText;
  int     IsCompletelyVis;
  int     IsCompletelyCovered;

  switch (pMsg->MsgId)
	{
		case WM_PAINT:
			if (_IsCompletelyVis)
			{
				GUI_SetBkColor(GUI_DARKGREEN);
				GUI_Clear();
				GUI_SetColor(GUI_WHITE);
				GUI_DispStringAt("Completely visible", 5, 5);
			}
			else
			{
				GUI_SetBkColor(GUI_GRAY);
				GUI_Clear();
				GUI_SetColor(GUI_WHITE);
				GUI_DispStringAt("Not completely visible", 5, 5);
			}
			break;
		case WM_NOTIFY_VIS_CHANGED:
			hText = WM_GetDialogItem(WM_HBKWIN, GUI_ID_TEXT1);
			hWin  = WM_GetClientWindow(pMsg->hWin);
			IsCompletelyVis     = WM_IsCompletelyVisible(hWin);
			IsCompletelyCovered = WM_IsCompletelyCovered(hWin);
			if (IsCompletelyCovered)
			{
				TEXT_SetText(hText, "completely\ncovered");
			} else
			{
				TEXT_SetText(hText, "not completely\ncovered");
			}
			if (_IsCompletelyVis != IsCompletelyVis)
			{
				_IsCompletelyVis = IsCompletelyVis;
				WM_InvalidateWindow(hWin);    /* Only required if content changes if partially hidden */
			}
			break;
		default:
			WM_DefaultProc(pMsg);
  }
}

/**
  * @brief 框架窗口测试回调函数
  * @note pMsg：消息指针
  * @param 无
  * @retval 无
  */
static void _cbFrameWinTest(WM_MESSAGE* pMsg)
{
  switch (pMsg->MsgId)
	{
		case WM_PAINT:
			GUI_SetBkColor(GUI_DARKRED);
			GUI_Clear();
			break;
		default:
			WM_DefaultProc(pMsg);
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
  FRAMEWIN_Handle hWinVideo;
  BUTTON_Handle   hBut;
  WM_HWIN         hText;

  WM_SetCallback(WM_HBKWIN, _cbBkWin);
  hText = TEXT_CreateEx(240,  85, 80, 26, WM_HBKWIN, WM_CF_SHOW, 0, GUI_ID_TEXT0, "The client\nwindow is");
  TEXT_SetTextColor(hText, GUI_WHITE);
  hText = TEXT_CreateEx(240, 111, 80, 26, WM_HBKWIN, WM_CF_SHOW, 0, GUI_ID_TEXT1, NULL);
  TEXT_SetTextColor(hText, GUI_WHITE);
	
  /* 创建按钮 */
  hBut = BUTTON_CreateEx(0,  5, 75, 18, WM_HBKWIN, WM_CF_SHOW, 0, GUI_ID_BUTTON0);
  BUTTON_SetText(hBut, "Create win");
  hBut = BUTTON_CreateEx(0, 25, 75, 18, WM_HBKWIN, WM_CF_SHOW, 0, GUI_ID_BUTTON1);
  BUTTON_SetText(hBut, "Delete win");
  hBut = BUTTON_CreateEx(0, 45, 75, 18, WM_HBKWIN, WM_CF_SHOW, 0, GUI_ID_BUTTON2);
  BUTTON_SetText(hBut, "Show win");
  hBut = BUTTON_CreateEx(0, 65, 75, 18, WM_HBKWIN, WM_CF_SHOW, 0, GUI_ID_BUTTON3);
  BUTTON_SetText(hBut, "Hide win");
	
  /* 创建框架窗口 */  
  hWinVideo = FRAMEWIN_CreateEx(80, 5, 160, 110, 0, WM_CF_SHOW, FRAMEWIN_CF_MOVEABLE, 0, "Video window", _cbFrameWinVideo);
  FRAMEWIN_SetClientColor(hWinVideo, GUI_INVALID_COLOR);
	
  /* 创建测试窗口 */
  _CreateWindow();
  _CreateWindow();
  _CreateWindow();
  while (1)
	{
    GUI_Delay(1000);
  }
}

/*************************** End of file ****************************/

