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
File        : SIMConf.c
Purpose     : Simulation configuration for softlayers.
---------------------------END-OF-HEADER------------------------------
*/

#ifdef WIN32

#include <windows.h>

#include "LCD_SIM.h"
#include "GUI_SIM_Win32.h"

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       SIM_X_Config
*
* Function description
*   Called during the initialization process in order to configure
*   the simulator.
*/
void SIM_X_Config() {
  SIM_GUI_SetCompositeSize(480, 272);  // Set size of composite window
  SIM_GUI_SetTransMode(1, GUI_TRANSMODE_PIXELALPHA);
  SIM_GUI_SetTransMode(2, GUI_TRANSMODE_PIXELALPHA);
  SIM_GUI_SetTransMode(3, GUI_TRANSMODE_PIXELALPHA);
  SIM_GUI_ShowDevice(1);
}

#else

void SIMConf_C(void); // Avoid empty object files
void SIMConf_C(void) {}

#endif

/*************************** End of file ****************************/
