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
File        : SIM_Hardkey.c
Purpose     : Example demonstrating the HARDKEY simulation
Requirements: WindowManager - (x)
              MemoryDevices - ( )
              AntiAliasing  - ( )
              VNC-Server    - ( )
              PNG-Library   - ( )
              TrueTypeFonts - ( )
----------------------------------------------------------------------
*/

#include "GUI.h"
#include "LCD_SIM.h"
#include "FRAMEWIN.h"
#include "LISTBOX.h"
#include <stddef.h>

//
// Recommended memory to run the sample with adequate performance
//
#define RECOMMENDED_MEMORY (1024L * 5)

/*******************************************************************
*
*       Static data
*
********************************************************************
*/
static const GUI_ConstString _aListBox[] = {
  "English", "Deutsch", "Français", "Japanese", "Italiano", "Español", NULL
};
static int _Ready;

/*******************************************************************
*
*       Static code
*
********************************************************************
*/
/*******************************************************************
*
*       _DemoHardkey
*
* Function description
*   Demonstrates HARDKEY simulation with a listbox
*/
static void _DemoHardkey(void) {
  LISTBOX_Handle hListBox;

  GUI_SetBkColor(GUI_BLACK);
  GUI_Clear();
  GUI_SetColor(GUI_WHITE);
  GUI_SetTextAlign(GUI_TA_HCENTER);
  GUI_SetFont(&GUI_Font24_ASCII);
  GUI_DispStringAt("Hardkey - Sample", 160, 5);
  GUI_SetFont(&GUI_Font8x8);
  GUI_DispStringHCenterAt("Exit",  65, 230);
  GUI_DispStringHCenterAt("Up",   160, 230);
  GUI_DispStringHCenterAt("Down", 255, 230);
  /* Create the listbox */
  hListBox = LISTBOX_Create(_aListBox, 100, 100, 120, 75, WM_CF_SHOW);
  WM_SetFocus(hListBox);
  LISTBOX_SetFont(hListBox, &GUI_Font13B_1);
  SCROLLBAR_CreateAttached(hListBox, SCROLLBAR_CF_VERTICAL);
  /* Handle the listbox */
  while (!_Ready) {
    GUI_Delay(10);
  }
  /* Delete listbox widget */
  WM_DeleteWindow(hListBox);
  GUI_Clear();
}

/*******************************************************************
*
*       _cbReady
*
* Function description
*   Callback for the ready-key
*/
static void _cbReady(int key, int state) {
  GUI_USE_PARA(key);
  GUI_USE_PARA(state);
  _Ready = 1;
}

/*******************************************************************
*
*       _cbKeyUp
*
* Function description
*   Callback for the up-key
*/
static void _cbKeyUp(int key, int state) {
  GUI_USE_PARA(key);
  GUI_StoreKeyMsg(GUI_KEY_UP, state);
}

/*******************************************************************
*
*       _cbKeyDown
* 
* Function description
*  Callback for the down-key
*/
static void _cbKeyDown(int key, int state) {
  GUI_USE_PARA(key);
  GUI_StoreKeyMsg(GUI_KEY_DOWN, state);
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
  //
  // Set callback routine for hardkey
  //
  SIM_HARDKEY_SetCallback(0, _cbReady);
  SIM_HARDKEY_SetCallback(1, _cbKeyUp);
  SIM_HARDKEY_SetCallback(2, _cbKeyDown);
  //
  // Start demo
  //
  _DemoHardkey();
}

/*************************** End of file ****************************/
