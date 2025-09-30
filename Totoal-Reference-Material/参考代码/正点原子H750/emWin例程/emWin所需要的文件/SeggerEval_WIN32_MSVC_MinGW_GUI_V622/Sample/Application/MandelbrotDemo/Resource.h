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
Purpose     : Contains prototypes of resources and color definitions
----------------------------------------------------------------------
*/

#ifndef RESOURCE_H
#define RESOURCE_H

/*********************************************************************
*
*       Externals
*
**********************************************************************
*/
//
// Bitmaps
//
extern GUI_CONST_STORAGE GUI_BITMAP bmButtonExit_0_60x40;
extern GUI_CONST_STORAGE GUI_BITMAP bmButtonExit_1_60x40;
extern GUI_CONST_STORAGE GUI_BITMAP bmButtonHelp_0_60x40;
extern GUI_CONST_STORAGE GUI_BITMAP bmButtonHelp_1_60x40;

//
// Fonts
//
extern GUI_CONST_STORAGE GUI_FONT GUI_Font16_AA4;
extern GUI_CONST_STORAGE GUI_FONT GUI_Font21_AA4;
extern GUI_CONST_STORAGE GUI_FONT GUI_Font24_AA4;
extern GUI_CONST_STORAGE GUI_FONT GUI_Font32_AA4;

/*********************************************************************
*
*       Define
*
**********************************************************************
*/

#define ID_BUTTON_HELP (GUI_ID_USER + 0)
#define ID_BUTTON_EXIT (GUI_ID_USER + 1)
#define ID_BUTTON_BACK (GUI_ID_USER + 2)

#define FONT_SMALL    &GUI_Font24_AA4
#define FONT_MEDIUM   &GUI_Font32_AA4
#define FONT_BODY     &GUI_Font16_AA4
#define FONT_HBODY    &GUI_Font21_AA4
#define FONT_HEADER   &GUI_Font32_AA4

#define MAX_DEPTH 32
#define MAX_ITER  256

#define BORDER  20  // Distance from buttons to border
#define FRAME_X 0
#define FRAME_Y 0

#define XSIZE_BUTTON  80
#define YSIZE_BUTTON  40

#define PENSIZE 4
#define COLOR_BLUE       GUI_MAKE_COLOR(0x009C5B2C)
#define COLOR_WHITE      GUI_MAKE_COLOR(0x00FAFAFA)

#define COLOR_SHAPE      GUI_MAKE_COLOR(0x007D5332)
//#define COLOR_BUTTON_0   GUI_MAKE_COLOR(0x00FCD7B9)
#define COLOR_BUTTON_0   COLOR_WHITE
#define COLOR_BUTTON_1   GUI_MAKE_COLOR(0x00A97144)
#define COLOR_BK         COLOR_WHITE

#define TEXT_NULL   -1
#define TEXT_EXIT    0
#define TEXT_CALC    1
#define TEXT_BACK    2
#define TEXT_SELECT  3
#define TEXT_MAX     4

/*********************************************************************
*
*       Typedefs
*
**********************************************************************
*/
typedef struct {
  double xMin;
  double xMax;
  double yMin;
  double yMax;
} MANDEL_RANGE;

typedef struct {
  MANDEL_RANGE Range;
  int          MaxIter;
  int          Depth;
  GUI_COLOR*   pColor;
  int          NumColors;
  int          IsDown;
  int          IsVis;
  int          x0;
  int          y0;
  int          x1;
  int          y1;
  int          xSize;
  int          ySize;
  WM_HWIN      hWin;
} MANDEL_DATA;

/*********************************************************************
*
*       Prototypes
*
**********************************************************************
*/
void DrawMandelbrot_FPU(WM_MESSAGE * pMsg, MANDEL_DATA * pData, int x0, int y0, int x1, int y1);

#endif // RESOURCE_H

/*************************** End of file ****************************/
