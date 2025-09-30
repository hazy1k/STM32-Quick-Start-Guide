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
File        : bmNext.c
Purpose     : Bitmap file.
----------------------------------------------------------------------
*/

#include <stdlib.h>

#include "GUI.h"

#ifndef GUI_CONST_STORAGE
  #define GUI_CONST_STORAGE const
#endif

extern GUI_CONST_STORAGE GUI_BITMAP bmNext;

static GUI_CONST_STORAGE unsigned char _acNext[] = {
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
  /* RLE: 007 Pixels @ 002,006 */ 7, 0x00, 
  /* ABS: 003 Pixels @ 009,006 */ 0, 3, 0x73, 0xAE, 0x17, 
  /* RLE: 010 Pixels @ 012,006 */ 10, 0x00, 
  /* ABS: 004 Pixels @ 022,006 */ 0, 4, 0x57, 0xFF, 0xDE, 0x03, 
  /* RLE: 007 Pixels @ 002,007 */ 7, 0x00, 
  /* ABS: 004 Pixels @ 009,007 */ 0, 4, 0xDF, 0xFF, 0xD5, 0x17, 
  /* RLE: 009 Pixels @ 013,007 */ 9, 0x00, 
  /* ABS: 003 Pixels @ 022,007 */ 0, 3, 0x03, 0xDE, 0x86, 
  /* RLE: 008 Pixels @ 001,008 */ 8, 0x00, 
  /* ABS: 005 Pixels @ 009,008 */ 0, 5, 0x46, 0xF6, 0xFF, 0xD5, 0x17, 
  /* RLE: 009 Pixels @ 014,008 */ 9, 0x00, 
  /* ABS: 002 Pixels @ 023,008 */ 0, 2, 0x86, 0x44, 
  /* RLE: 009 Pixels @ 001,009 */ 9, 0x00, 
  /* ABS: 005 Pixels @ 010,009 */ 0, 5, 0x46, 0xF6, 0xFF, 0xD5, 0x17, 
  /* RLE: 008 Pixels @ 015,009 */ 8, 0x00, 
  /* ABS: 002 Pixels @ 023,009 */ 0, 2, 0x44, 0x19, 
  /* RLE: 010 Pixels @ 001,010 */ 10, 0x00, 
  /* ABS: 005 Pixels @ 011,010 */ 0, 5, 0x46, 0xF6, 0xFF, 0xD5, 0x17, 
  /* RLE: 007 Pixels @ 016,010 */ 7, 0x00, 
  /* ABS: 002 Pixels @ 023,010 */ 0, 2, 0x19, 0x04, 
  /* RLE: 011 Pixels @ 001,011 */ 11, 0x00, 
  /* ABS: 004 Pixels @ 012,011 */ 0, 4, 0x46, 0xF6, 0xFF, 0xD0, 
  /* RLE: 007 Pixels @ 016,011 */ 7, 0x00, 
  /* ABS: 002 Pixels @ 023,011 */ 0, 2, 0x04, 0x04, 
  /* RLE: 011 Pixels @ 001,012 */ 11, 0x00, 
  /* ABS: 004 Pixels @ 012,012 */ 0, 4, 0x46, 0xF6, 0xFF, 0xD0, 
  /* RLE: 007 Pixels @ 016,012 */ 7, 0x00, 
  /* ABS: 002 Pixels @ 023,012 */ 0, 2, 0x04, 0x19, 
  /* RLE: 010 Pixels @ 001,013 */ 10, 0x00, 
  /* ABS: 005 Pixels @ 011,013 */ 0, 5, 0x46, 0xF6, 0xFF, 0xD6, 0x18, 
  /* RLE: 007 Pixels @ 016,013 */ 7, 0x00, 
  /* ABS: 002 Pixels @ 023,013 */ 0, 2, 0x19, 0x44, 
  /* RLE: 009 Pixels @ 001,014 */ 9, 0x00, 
  /* ABS: 005 Pixels @ 010,014 */ 0, 5, 0x46, 0xF6, 0xFF, 0xD6, 0x18, 
  /* RLE: 008 Pixels @ 015,014 */ 8, 0x00, 
  /* ABS: 002 Pixels @ 023,014 */ 0, 2, 0x44, 0x86, 
  /* RLE: 008 Pixels @ 001,015 */ 8, 0x00, 
  /* ABS: 005 Pixels @ 009,015 */ 0, 5, 0x45, 0xF6, 0xFF, 0xD5, 0x18, 
  /* RLE: 009 Pixels @ 014,015 */ 9, 0x00, 
  /* ABS: 003 Pixels @ 023,015 */ 0, 3, 0x86, 0xDE, 0x03, 
  /* RLE: 007 Pixels @ 002,016 */ 7, 0x00, 
  /* ABS: 004 Pixels @ 009,016 */ 0, 4, 0xE3, 0xFF, 0xD5, 0x18, 
  /* RLE: 009 Pixels @ 013,016 */ 9, 0x00, 
  /* ABS: 004 Pixels @ 022,016 */ 0, 4, 0x03, 0xDE, 0xFF, 0x57, 
  /* RLE: 007 Pixels @ 002,017 */ 7, 0x00, 
  /* ABS: 003 Pixels @ 009,017 */ 0, 3, 0x7C, 0xB2, 0x18, 
  /* RLE: 010 Pixels @ 012,017 */ 10, 0x00, 
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
};  // 330 bytes for 576 pixels

GUI_CONST_STORAGE GUI_BITMAP bmNext = {
  24, // xSize
  24, // ySize
  24, // BytesPerLine
  GUI_COMPRESS_RLE8, // BitsPerPixel
  (unsigned char *)_acNext,  // Pointer to picture data
  NULL,  // Pointer to palette
  GUI_DRAW_RLEALPHA
};

/*************************** End of file ****************************/
