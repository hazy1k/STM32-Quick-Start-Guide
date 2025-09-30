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
File        : bmPlay.c
Purpose     : Bitmap file.
----------------------------------------------------------------------
*/

#include <stdlib.h>

#include "GUI.h"

#ifndef GUI_CONST_STORAGE
  #define GUI_CONST_STORAGE const
#endif

extern GUI_CONST_STORAGE GUI_BITMAP bmPlay;

static GUI_CONST_STORAGE unsigned char _acPlay[] = {
  /* RLE: 007 Pixels @ 000,000 */ 7, 0xFF, 
  /* ABS: 010 Pixels @ 007,000 */ 0, 10, 0xDE, 0x86, 0x44, 0x19, 0x04, 0x04, 0x19, 0x44, 0x86, 0xDE, 
  /* RLE: 012 Pixels @ 017,000 */ 12, 0xFF, 
  /* ABS: 003 Pixels @ 005,001 */ 0, 3, 0xDF, 0x56, 0x03, 
  /* RLE: 008 Pixels @ 008,001 */ 8, 0x00, 
  /* ABS: 003 Pixels @ 016,001 */ 0, 3, 0x03, 0x56, 0xDF, 
  /* RLE: 009 Pixels @ 019,001 */ 9, 0xFF, 
  /* ABS: 002 Pixels @ 004,002 */ 0, 2, 0xA3, 0x0C, 
  /* RLE: 012 Pixels @ 006,002 */ 12, 0x00, 
  /* ABS: 002 Pixels @ 018,002 */ 0, 2, 0x0C, 0xA3, 
  /* RLE: 007 Pixels @ 020,002 */ 7, 0xFF, 
  /* ABS: 002 Pixels @ 003,003 */ 0, 2, 0x8A, 0x01, 
  /* RLE: 014 Pixels @ 005,003 */ 14, 0x00, 
  /* ABS: 002 Pixels @ 019,003 */ 0, 2, 0x01, 0x8A, 
  /* RLE: 005 Pixels @ 021,003 */ 5, 0xFF, 
  /* ABS: 002 Pixels @ 002,004 */ 0, 2, 0xA3, 0x01, 
  /* RLE: 016 Pixels @ 004,004 */ 16, 0x00, 
  /* ABS: 007 Pixels @ 020,004 */ 0, 7, 0x01, 0xA3, 0xFF, 0xFF, 0xFF, 0xDF, 0x0C, 
  /* RLE: 018 Pixels @ 003,005 */ 18, 0x00, 
  /* ABS: 005 Pixels @ 021,005 */ 0, 5, 0x0C, 0xDF, 0xFF, 0xFF, 0x57, 
  /* RLE: 020 Pixels @ 002,006 */ 20, 0x00, 
  /* ABS: 004 Pixels @ 022,006 */ 0, 4, 0x57, 0xFF, 0xDE, 0x03, 
  /* RLE: 006 Pixels @ 002,007 */ 6, 0x00, 
  /* ABS: 003 Pixels @ 008,007 */ 0, 3, 0x32, 0x7E, 0x0F, 
  /* RLE: 011 Pixels @ 011,007 */ 11, 0x00, 
  /* ABS: 003 Pixels @ 022,007 */ 0, 3, 0x03, 0xDE, 0x86, 
  /* RLE: 007 Pixels @ 001,008 */ 7, 0x00, 
  /* ABS: 005 Pixels @ 008,008 */ 0, 5, 0x7B, 0xFF, 0xE8, 0x64, 0x02, 
  /* RLE: 010 Pixels @ 013,008 */ 10, 0x00, 
  /* ABS: 002 Pixels @ 023,008 */ 0, 2, 0x86, 0x44, 
  /* RLE: 007 Pixels @ 001,009 */ 7, 0x00, 
  /* ABS: 006 Pixels @ 008,009 */ 0, 6, 0x7B, 0xFF, 0xFF, 0xFF, 0xCD, 0x3D, 
  /* RLE: 009 Pixels @ 014,009 */ 9, 0x00, 
  /* ABS: 002 Pixels @ 023,009 */ 0, 2, 0x44, 0x19, 
  /* RLE: 007 Pixels @ 001,010 */ 7, 0x00, 
  /* RLE: 001 Pixels @ 008,010 */ 1, 0x7B, 
  /* RLE: 004 Pixels @ 009,010 */ 4, 0xFF, 
  /* ABS: 003 Pixels @ 013,010 */ 0, 3, 0xFE, 0xA8, 0x1E, 
  /* RLE: 007 Pixels @ 016,010 */ 7, 0x00, 
  /* ABS: 002 Pixels @ 023,010 */ 0, 2, 0x19, 0x04, 
  /* RLE: 007 Pixels @ 001,011 */ 7, 0x00, 
  /* RLE: 001 Pixels @ 008,011 */ 1, 0x7B, 
  /* RLE: 006 Pixels @ 009,011 */ 6, 0xFF, 
  /* ABS: 002 Pixels @ 015,011 */ 0, 2, 0xF5, 0x6D, 
  /* RLE: 006 Pixels @ 017,011 */ 6, 0x00, 
  /* ABS: 002 Pixels @ 023,011 */ 0, 2, 0x04, 0x04, 
  /* RLE: 007 Pixels @ 001,012 */ 7, 0x00, 
  /* RLE: 001 Pixels @ 008,012 */ 1, 0x7B, 
  /* RLE: 006 Pixels @ 009,012 */ 6, 0xFF, 
  /* ABS: 002 Pixels @ 015,012 */ 0, 2, 0xFE, 0x89, 
  /* RLE: 006 Pixels @ 017,012 */ 6, 0x00, 
  /* ABS: 002 Pixels @ 023,012 */ 0, 2, 0x04, 0x19, 
  /* RLE: 007 Pixels @ 001,013 */ 7, 0x00, 
  /* RLE: 001 Pixels @ 008,013 */ 1, 0x7B, 
  /* RLE: 005 Pixels @ 009,013 */ 5, 0xFF, 
  /* ABS: 002 Pixels @ 014,013 */ 0, 2, 0xCA, 0x39, 
  /* RLE: 007 Pixels @ 016,013 */ 7, 0x00, 
  /* ABS: 002 Pixels @ 023,013 */ 0, 2, 0x19, 0x44, 
  /* RLE: 007 Pixels @ 001,014 */ 7, 0x00, 
  /* ABS: 007 Pixels @ 008,014 */ 0, 7, 0x7B, 0xFF, 0xFF, 0xFF, 0xE6, 0x60, 0x02, 
  /* RLE: 008 Pixels @ 015,014 */ 8, 0x00, 
  /* ABS: 002 Pixels @ 023,014 */ 0, 2, 0x44, 0x86, 
  /* RLE: 007 Pixels @ 001,015 */ 7, 0x00, 
  /* ABS: 005 Pixels @ 008,015 */ 0, 5, 0x7B, 0xFF, 0xF8, 0x87, 0x0D, 
  /* RLE: 010 Pixels @ 013,015 */ 10, 0x00, 
  /* ABS: 003 Pixels @ 023,015 */ 0, 3, 0x86, 0xDE, 0x03, 
  /* RLE: 006 Pixels @ 002,016 */ 6, 0x00, 
  /* ABS: 003 Pixels @ 008,016 */ 0, 3, 0x42, 0xA2, 0x23, 
  /* RLE: 011 Pixels @ 011,016 */ 11, 0x00, 
  /* ABS: 004 Pixels @ 022,016 */ 0, 4, 0x03, 0xDE, 0xFF, 0x57, 
  /* RLE: 020 Pixels @ 002,017 */ 20, 0x00, 
  /* ABS: 005 Pixels @ 022,017 */ 0, 5, 0x57, 0xFF, 0xFF, 0xDF, 0x0C, 
  /* RLE: 018 Pixels @ 003,018 */ 18, 0x00, 
  /* ABS: 007 Pixels @ 021,018 */ 0, 7, 0x0C, 0xDF, 0xFF, 0xFF, 0xFF, 0xA3, 0x01, 
  /* RLE: 016 Pixels @ 004,019 */ 16, 0x00, 
  /* ABS: 002 Pixels @ 020,019 */ 0, 2, 0x01, 0xA3, 
  /* RLE: 005 Pixels @ 022,019 */ 5, 0xFF, 
  /* ABS: 002 Pixels @ 003,020 */ 0, 2, 0x8A, 0x01, 
  /* RLE: 014 Pixels @ 005,020 */ 14, 0x00, 
  /* ABS: 002 Pixels @ 019,020 */ 0, 2, 0x01, 0x8A, 
  /* RLE: 007 Pixels @ 021,020 */ 7, 0xFF, 
  /* ABS: 002 Pixels @ 004,021 */ 0, 2, 0xA3, 0x0C, 
  /* RLE: 012 Pixels @ 006,021 */ 12, 0x00, 
  /* ABS: 002 Pixels @ 018,021 */ 0, 2, 0x0C, 0xA3, 
  /* RLE: 009 Pixels @ 020,021 */ 9, 0xFF, 
  /* ABS: 003 Pixels @ 005,022 */ 0, 3, 0xDF, 0x56, 0x03, 
  /* RLE: 008 Pixels @ 008,022 */ 8, 0x00, 
  /* ABS: 003 Pixels @ 016,022 */ 0, 3, 0x03, 0x56, 0xDF, 
  /* RLE: 012 Pixels @ 019,022 */ 12, 0xFF, 
  /* ABS: 010 Pixels @ 007,023 */ 0, 10, 0xDE, 0x86, 0x44, 0x19, 0x04, 0x04, 0x19, 0x44, 0x86, 0xDE, 
  /* RLE: 007 Pixels @ 017,023 */ 7, 0xFF, 
  0
};  // 324 bytes for 576 pixels

GUI_CONST_STORAGE GUI_BITMAP bmPlay = {
  24, // xSize
  24, // ySize
  24, // BytesPerLine
  GUI_COMPRESS_RLE8, // BitsPerPixel
  (unsigned char *)_acPlay,  // Pointer to picture data
  NULL,  // Pointer to palette
  GUI_DRAW_RLEALPHA
};

/*************************** End of file ****************************/
