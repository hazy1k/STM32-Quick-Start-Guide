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
File        : Resource.c
Purpose     : Resource declarations
----------------------------------------------------------------------
*/

#ifndef RESOURCE_H
#define RESOURCE_H

#include "GUI.h"

//
// Bitmaps
//
extern GUI_CONST_STORAGE GUI_BITMAP bmBack;
extern GUI_CONST_STORAGE GUI_BITMAP bmEject;
extern GUI_CONST_STORAGE GUI_BITMAP bmNext;
extern GUI_CONST_STORAGE GUI_BITMAP bmPause;
extern GUI_CONST_STORAGE GUI_BITMAP bmPlay;
extern GUI_CONST_STORAGE GUI_BITMAP bmStop;
extern GUI_CONST_STORAGE GUI_BITMAP bmSeggerLogo_120x60;

//
// Fonts
//
extern GUI_CONST_STORAGE GUI_FONT GUI_Font24_AA4;

//
// Colors
//
#define COLOR_LIME      GUI_MAKE_COLOR(0x00A6A2)
#define COLOR_BLUE      GUI_MAKE_COLOR(0x633500)
#define COLOR_BRIGHT    GUI_MAKE_COLOR(0xE0E0E0)
#define COLOR_WHITE     GUI_MAKE_COLOR(0xF0F0F0)
#define COLOR_LIGHTGRAY GUI_MAKE_COLOR(0xE9E8E7)
#define COLOR_DARKBLUE  GUI_MAKE_COLOR(0x623700)
#define COLOR_TEXTGRAY  GUI_MAKE_COLOR(0XA6A39F)

#endif // RESOURCE_H

/*************************** End of file ****************************/
