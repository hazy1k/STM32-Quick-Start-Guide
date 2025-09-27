#ifndef __DMA_LCD_H__
#define __DMA_LCD_H__

#ifdef	__cplusplus
extern "C"{
#endif

#include "def.h"
#include "BSP.h"
/*=========================================================================================*/

void LCD_DMA_Init(u32 lcd_dat_addr);
BOOL LCD_DMA_WritePixels(u16 val,s32 count);
BOOL LCD_DMA_WriteBitmap(u16 *src,int count);

/*=========================================================================================*/

#ifdef	__cplusplus
}
#endif

#endif /*__DMA_LCD_H__*/














