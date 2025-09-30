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
File        : 2DGL_DrawStreamedBitmap.c
Purpose     : Shows how to draw a dta bitmap file from hard disc.
Requirements: WindowManager - ( )
              MemoryDevices - ( )
              AntiAliasing  - ( )
              VNC-Server    - ( )
              PNG-Library   - ( )
              TrueTypeFonts - ( )
----------------------------------------------------------------------
*/

#ifndef SKIP_TEST

#include <windows.h>

#include "GUI.h"

/*********************************************************************
*
*       _GetData
*/
static int _GetData(void * p, const U8 ** ppData, unsigned NumBytesReq, U32 Off) {
  HANDLE * phFile;
  DWORD    NumBytesRead;
  U8     * pData;

  pData  = (U8 *)*ppData;
  phFile = (HANDLE *)p;
  //
  // Set file pointer to the required position
  //
  SetFilePointer(*phFile, Off, 0, FILE_BEGIN);
  //
  // Read data into buffer
  //
  ReadFile(*phFile, pData, NumBytesReq, &NumBytesRead, NULL);
  //
  // Return number of available bytes
  //
  return NumBytesRead;
}

/*********************************************************************
*
*       MainTask
*/
void MainTask(void) {
  HANDLE hFile;

  GUI_Init();
  hFile = CreateFile("c:\\Work\\emWin\\Sample\\Tutorial\\2DGL_DrawStreamedBitmap\\Bitmap_50x50.dta", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  GUI_DrawStreamedBitmapExAuto(_GetData, &hFile, 20, 20);
  while (1) {
    GUI_Delay(100);
  }
}

#endif
