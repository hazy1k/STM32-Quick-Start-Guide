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
File        : Resource.h
Content     : Main resource header file of weather forecast demo
---------------------------END-OF-HEADER------------------------------
*/

#ifndef RESOURCE_H
#define RESOURCE_H

//
// Bitmaps
//
extern GUI_CONST_STORAGE GUI_BITMAP bmBkCotton_B_480x272;
extern GUI_CONST_STORAGE GUI_BITMAP bmBkEco_B_480x272;
extern GUI_CONST_STORAGE GUI_BITMAP bmBkSilk_B_480x272;
extern GUI_CONST_STORAGE GUI_BITMAP bmBkSpin_B_480x272;
extern GUI_CONST_STORAGE GUI_BITMAP bmBkSynthetic_B_480x272;
extern GUI_CONST_STORAGE GUI_BITMAP bmBkWoll_B_480x272;

//
// Fonts
//
extern GUI_CONST_STORAGE GUI_FONT GUI_Font_AA4_19_L;
extern GUI_CONST_STORAGE GUI_FONT GUI_Font_AA4_26_R;

//
// Colors
//
#define COLOR_GREEN  GUI_MAKE_COLOR(0x0014b6a9)
#define COLOR_BLUE   GUI_MAKE_COLOR(0x00623700)
#define COLOR_RED    GUI_MAKE_COLOR(0x000000ff)
#define COLOR_WHITE  GUI_MAKE_COLOR(0x00F2F2F2)

//
// Positions and sizes
//
#define WIN0_XPOS       14
#define WIN0_XSIZE      99
#define WIN1_XPOS       114
#define WIN1_XSIZE      59
#define WIN2_XPOS       174
#define WIN2_XSIZE      59
#define WIN3_XPOS       234
#define WIN3_XSIZE      101
#define WIN4_XPOS       336
#define WIN4_XSIZE      64
#define WIN_BAR_XSIZE   424
#define WIN_BAR_YSIZE   90
#define BUTTON_XSIZE    80
#define BUTTON_YSIZE    20
#define WIN_START_YSIZE 30
#define WIN_DESCR_YSIZE 30
#define TEXT_XSIZE      80
#define TEXT_YSIZE      30

//
// Default time
//
#define DEFAULT_HOUR    19
#define DEFAULT_MIN     15

//
// Define the fonts
//
#define FONT_SELECTION    &GUI_Font_AA4_26_R
#define FONT_DESCRIPTION  &GUI_Font_AA4_19_L

//
// Animation data
//
typedef struct {
  WM_HWIN hWin;
  int     Value;
} ANIM_DATA;

//
// Window data
//
typedef struct {
  WM_HWIN       hWin;
  const char ** ppText;
  int           NumItems;
  int           Align;
} WIN_DATA;

#endif // RESOURCE_H

/*************************** End of file ****************************/
