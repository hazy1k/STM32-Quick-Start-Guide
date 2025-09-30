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
File        : APP_Resources.h
Purpose     : Resource interface
---------------------------END-OF-HEADER------------------------------
*/

#ifndef APP_RESOURCES_H
#define APP_RESOURCES_H

extern GUI_CONST_STORAGE GUI_BITMAP bmLogoBitmap;

void ExecSetup(void);
void ExecAbout(void);
void ExecCalibration(void);

#endif
