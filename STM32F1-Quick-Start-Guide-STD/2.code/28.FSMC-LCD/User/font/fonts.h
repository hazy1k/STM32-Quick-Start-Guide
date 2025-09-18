#ifndef __FONT_H
#define __FONT_H       

#include "stm32f10x.h"
#include "fonts.h"

#define LINE(x) ((x)*(((sFONT *)LCD_GetFont())->Height)) 
#define LINEY(x) ((x)*(((sFONT *)LCD_GetFont())->Width)) 

typedef struct _tFont
{    
  const uint8_t *table; 
  uint16_t Width; 
  uint16_t Height; 
}sFONT;

extern sFONT Font24x32;
extern sFONT Font16x24;
extern sFONT Font8x16;

#endif 
