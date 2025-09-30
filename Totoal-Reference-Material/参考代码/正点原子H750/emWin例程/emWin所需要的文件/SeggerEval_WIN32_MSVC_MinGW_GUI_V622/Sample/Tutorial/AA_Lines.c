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
File        : AA_Lines.c
Purpose     : Shows lines with different antialiasing qualities
Requirements: WindowManager - ( )
              MemoryDevices - ( )
              AntiAliasing  - (x)
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

/*******************************************************************
*
*       Static code
*
********************************************************************
*/
/*******************************************************************
*
*       _DemoAntialiasing
*
* Function description
*   Draws lines with different antialiasing factors
*/
static void _DemoAntialiasing(void) {
  const GUI_FONT * font_old;
  int              i;
  int              x1;
  int              x2;
  int              y1;
  int              y2;

  y1 = 65;
  y2 = 5;
  //
  // Set drawing attributes 
  //
  GUI_SetColor(GUI_WHITE);
  GUI_SetBkColor(GUI_BLACK);
  GUI_SetPenShape(GUI_PS_FLAT);
  GUI_Clear();
  //
  // Draw headline
  //
  font_old = GUI_SetFont(&GUI_Font24_ASCII);
  GUI_SetTextAlign(GUI_TA_HCENTER);
  GUI_DispStringAt("AA_Lines - Sample", 160, 5);
  //
  // Draw lines without antialiased
  //
  GUI_Delay(1000);
  GUI_SetFont(&GUI_Font8x16);
  GUI_SetTextAlign(GUI_TA_LEFT);
  GUI_DispStringAtCEOL("draw normal lines using", 5, 40);
  GUI_DispStringAtCEOL("GUI_DrawLine", 5, 55);
  GUI_Delay(2500);
  x1 = 20;
  x2 = 100;
  GUI_SetFont(font_old);
  GUI_DispStringHCenterAt("Normal", (x1 + x2) / 2, 30 + y1);
  for (i = 1; i < 8; i++) {
    GUI_SetPenSize(i);
    GUI_DrawLine(x1, 40 + i * 15 + y1, x2, 40 + i * 15 + y1 + y2);
  }
  //
  // Draw lines with antialiasing quality factor 2
  //
  GUI_Delay(3000);
  GUI_SetFont(&GUI_Font8x16);
  GUI_DispStringAtCEOL("", 5, 40);
  GUI_DispStringAtCEOL("", 5, 55);
  GUI_Delay(200);
  GUI_DispStringAtCEOL("draw antialiased lines using", 5, 40);
  GUI_DispStringAtCEOL("GUI_AA_DrawLine", 5, 55);
  GUI_Delay(3500);
  x1 = 120;
  x2 = 200;
  GUI_AA_SetFactor(2);
  GUI_SetFont(font_old);
  GUI_DispStringHCenterAt("Antialiased\nusing factor 2", (x1 + x2) / 2, 30 + y1);
  for (i = 1; i < 8; i++) {
    GUI_SetPenSize(i);
    GUI_AA_DrawLine(x1, 40 + i * 15 + y1, x2, 40 + i * 15 + y1 + y2);
  }
  //
  // Draw lines with antialiasing quality factor 6
  //
  GUI_Delay(1500);
  x1 = 220;
  x2 = 300;
  GUI_AA_SetFactor(6);
  GUI_SetFont(font_old);
  GUI_DispStringHCenterAt("Antialiased\nusing factor 6", (x1 + x2) / 2, 30 + y1);
  for (i = 1; i < 8; i++) {
    GUI_SetPenSize(i);
    GUI_AA_DrawLine(x1, 40 + i * 15 + y1, x2, 40 + i * 15 + y1 + y2);
  }
  GUI_Delay(7500);
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       MainTask
*/
void MainTask(void) {
  GUI_Init();
  //
  // Check if recommended memory for the sample is available
  //
  if (GUI_ALLOC_GetNumFreeBytes() < RECOMMENDED_MEMORY) {
    GUI_ErrorOut("Not enough memory available."); 
    return;
  }
  while (1) {
    _DemoAntialiasing();
  }
}

/*************************** End of file ****************************/

