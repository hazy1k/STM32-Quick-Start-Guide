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
File    : GUIDEV_NormalMapping.h
Purpose : Header for normal mapping of memory devices.
--------  END-OF-HEADER  ---------------------------------------------
*/
#ifndef GUIDEV_NORMALMAPPING_H
#define GUIDEV_NORMALMAPPING_H

#include "GUI.h"

/*********************************************************************
*
*       Typedef
*
**********************************************************************
*/
typedef struct {
  GUI_MEMDEV_Handle   hMemDest;
  const U16         * pNMap;
  const GUI_BITMAP  * pBmImage;
  GUI_MEMDEV_Handle   hMemImage;
  const GUI_BITMAP  * pbmLight;
  int                 xSize;
  int                 ySize;
} GUI_MAPPING_CONTEXT;

/*********************************************************************
*
*       Prototype
*
**********************************************************************
*/
int GUI_MEMDEV_NormalMapping(GUI_MAPPING_CONTEXT * pContext, int xPosLight, int yPosLight);

#endif

/****** End of File *************************************************/
