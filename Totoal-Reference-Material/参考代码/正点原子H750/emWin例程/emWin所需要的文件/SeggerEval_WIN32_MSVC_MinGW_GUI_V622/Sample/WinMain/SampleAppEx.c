/*********************************************************************
*                    SEGGER Microcontroller GmbH                     *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2021  SEGGER Microcontroller GmbH                *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V6.22 - Graphical user interface for embedded applications **
emWin is protected by international copyright laws.   Knowledge of the
source code may not be used to write a similar product.  This file may
only  be used  in accordance  with  a license  and should  not be  re-
distributed in any way. We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : SampleAppEx.c
Purpose     : Shows how to integrate the emWin simulation into an
              existing Windows application.
----------------------------------------------------------------------
*/

#include <windows.h>
#include <commctrl.h>

#include "GUI_SIM_Win32.h"
#include "LCD_SIM.h"
#include "GUI.h"

void MainTask(void);

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _Thread
*/
static DWORD __stdcall _Thread(void* Parameter) {
  /* Execute the target application to be simulated */
  MainTask();
  return 0;
}

/*********************************************************************
*
*       _SetParts
*/
static void _SetParts(HWND hWndStatus) {
  int Width;
  RECT RectStatus;
  GetWindowRect(hWndStatus, &RectStatus);
  Width = RectStatus.right - RectStatus.left - 20;
  SendMessage(hWndStatus, SB_SETPARTS, 1, (LPARAM)&Width);
}

/*********************************************************************
*
*       _WndProcMain
*/
static LRESULT CALLBACK _WndProcMain(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  HWND hWndStatus;
  SIM_GUI_HandleKeyEvents(message, wParam);
  switch (message) {
  case WM_SIZE:
    hWndStatus = FindWindowEx(hWnd, NULL, STATUSCLASSNAME, NULL);
    if (hWndStatus) {
      _SetParts(hWndStatus);
      SendMessage(hWndStatus, message, wParam, lParam);
    }
    break;
  case WM_DESTROY:
    PostQuitMessage(0);
    break;
  }
  return DefWindowProc(hWnd, message, wParam, lParam);
}

/*********************************************************************
*
*       _RegisterClass
*/
static void _RegisterClass(HINSTANCE hInstance) {
  WNDCLASSEX wcex;
  memset (&wcex, 0,  sizeof(wcex));
  wcex.cbSize        = sizeof(WNDCLASSEX); 
  wcex.hInstance     = hInstance;
  wcex.style         = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc   = (WNDPROC)_WndProcMain;
  wcex.hIcon         = 0;
  wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)(COLOR_APPWORKSPACE + 1);
  wcex.lpszMenuName  = 0;
  wcex.lpszClassName = "GUIApplication";
  RegisterClassEx(&wcex);
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       WinMain
*/
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
  RECT RectStatus, RectMain;
  DWORD ThreadID;
  MSG   Msg;
  HWND  hWndMain, hWndStatus;
  INITCOMMONCONTROLSEX InitComCTRLEx;
  /* Ensure that the common control DLL is loaded. */
  InitCommonControlsEx(&InitComCTRLEx);
  /* Register window class */
  _RegisterClass(hInstance);
  /* Make sure the driver configuration is done */
  SIM_GUI_Enable();
  /* Create main window */
  hWndMain = CreateWindow("GUIApplication", "Application window",
                          WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_VISIBLE, 
                          0, 0, 
                          320 + GetSystemMetrics(SM_CXSIZEFRAME) * 2, 
                          240 + GetSystemMetrics(SM_CYSIZEFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION), 
                          NULL, NULL, hInstance, NULL);
  /* Create status window */
  hWndStatus = CreateWindow(STATUSCLASSNAME, "Ready window",
                            WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP,
                            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                            hWndMain, NULL, hInstance, NULL);
  /* Adjust size of main window */
  GetWindowRect(hWndStatus, &RectStatus);
  GetWindowRect(hWndMain,   &RectMain);
  SetWindowPos(hWndMain, 0, 
               RectMain.left, 
               RectMain.top, 
               RectMain.right, 
               RectMain.bottom + (RectStatus.bottom - RectStatus.top), 
               SWP_NOZORDER);
  SetWindowPos(hWndStatus, 0, 
               RectStatus.left, 
               RectStatus.top  + (RectStatus.bottom - RectStatus.top), 
               RectStatus.right, 
               RectStatus.bottom + (RectStatus.bottom - RectStatus.top), 
               SWP_NOZORDER);
  _SetParts(hWndStatus);
  /* Initialize the GUI simulation and create a LCD window */
  SIM_GUI_Init(hInstance, hWndMain, lpCmdLine, "GUI Simulation");
  SIM_GUI_CreateLCDWindow(hWndMain, 0, 0, 320, 240, 0);
  /* Create a thread which executes the code to be simulated */
  CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)_Thread, NULL, 0, &ThreadID);
  /* Main message loop */
  while (GetMessage(&Msg, NULL, 0, 0)) {
    TranslateMessage(&Msg);
    DispatchMessage(&Msg);
  }
  SIM_GUI_Exit();
}

/*************************** End of file ****************************/
