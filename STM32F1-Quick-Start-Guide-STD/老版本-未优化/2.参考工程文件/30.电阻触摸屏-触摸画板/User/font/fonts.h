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
#define WIDTH_CH_CHAR  16 // 中文字符宽度 
#define HEIGHT_CH_CHAR 16 // 中文字符高度 
#define LINE(x) ((x)*(((sFONT*)LCD_GetFont())->Height))
#define LINEY(x) ((x)*(WIDTH_CH_CHAR))
#define GBKCODE_START_ADDRESS   387*4096
// 定义获取中文字符字模数组的函数名，ucBuffer为存放字模数组名，usChar为中文字符（国标码）
#define GetGBKCode(ucBuffer, usChar) GetGBKCode_from_EXFlash(ucBuffer, usChar)  
int GetGBKCode_from_EXFlash(uint8_t* pBuffer, uint16_t c);

#endif /*end of __FONT_H    */
