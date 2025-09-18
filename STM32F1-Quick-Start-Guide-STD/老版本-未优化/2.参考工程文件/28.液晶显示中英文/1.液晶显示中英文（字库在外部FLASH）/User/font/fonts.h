#ifndef __FONT_H
#define __FONT_H       

#include "stm32f10x.h"
#include "fonts.h"

typedef struct _tFont
{    
  const uint8_t *table;
  uint16_t Width;
  uint16_t Height;
} sFONT;

extern sFONT Font24x32;
extern sFONT Font16x24;
extern sFONT Font8x16;

#define  WIDTH_CH_CHAR	16	     
#define  HEIGHT_CH_CHAR 16		  

#define LINE(x) ((x)*(((sFONT *)LCD_GetFont())->Height))
#define LINEY(x) ((x)*(WIDTH_CH_CHAR))

#define GBKCODE_FLASH 1

#if GBKCODE_FLASH
	#define GBKCODE_START_ADDRESS 387*4096
	#define GetGBKCode(ucBuffer, usChar) GetGBKCode_from_EXFlash(ucBuffer, usChar)  
	int GetGBKCode_from_EXFlash(uint8_t* pBuffer, uint16_t c);
#endif

#endif /* end of __FONT_H */
