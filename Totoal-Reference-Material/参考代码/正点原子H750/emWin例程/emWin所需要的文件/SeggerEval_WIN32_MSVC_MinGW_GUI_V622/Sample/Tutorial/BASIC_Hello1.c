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
File        : BASIC_Hello1.c
Purpose     : Simple demo drawing "Hello world"
Requirements: WindowManager - ( )
              MemoryDevices - ( )
              AntiAliasing  - ( )
              VNC-Server    - ( )
              PNG-Library   - ( )
              TrueTypeFonts - ( )
----------------------------------------------------------------------
*/

#include "GUI.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
//
// Recommended memory to run the sample with adequate performance
//
#define RECOMMENDED_MEMORY (1024L * 5)

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*******************************************************************
*
*       MainTask
*/
void MainTask(void) {
  int xPos;
  int yPos;
  int xSize;
  int i;
  
  i = 0;
  GUI_Init();
  //
  // Check if recommended memory for the sample is available
  //
  if (GUI_ALLOC_GetNumFreeBytes() < RECOMMENDED_MEMORY) {
    GUI_ErrorOut("Not enough memory available."); 
    return;
  }
  xPos = LCD_GetXSize() / 2;
  yPos = LCD_GetYSize() / 3;
  GUI_SetTextMode(GUI_TM_REV);
  GUI_SetFont(GUI_FONT_20F_ASCII);
  GUI_DispStringHCenterAt("Hello world!", xPos, yPos);
  GUI_SetFont(GUI_FONT_D24X32);
  xSize = GUI_GetStringDistX("0000");
  xPos -= xSize / 2;
  yPos += 24 + 10;
  while (1) {
    GUI_DispDecAt( i++, xPos, yPos, 4);
    if (i > 9999) {
      i = 0;
    }
  }
}

/*************************** End of file ****************************/

