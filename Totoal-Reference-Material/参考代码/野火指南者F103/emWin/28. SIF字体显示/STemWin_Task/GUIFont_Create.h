#ifndef __GUIFONT_CREATE_H
#define	__GUIFONT_CREATE_H

#include "GUI.h"
#include "stm32f10x.h"

//设置SIF字体存储的位置：
//FLASH非文件系统区域（推荐） 	USE_FLASH_FONT             	0 
//SD卡文件系统区域							USE_SDCARD_FONT							1
#define SIF_FONT_SOURCE				1

//（速度最快）字库在FLASH的非文件系统区域，使用前需要往FLASH特定地址拷贝字体文件
#define USE_FLASH_FONT				0	
//（速度中等）字库存储在SD卡文件系统区域，调试比较方便，直接使用读卡器从电脑拷贝字体文件即可
#define USE_SDCARD_FONT				1


/*支持的字库类型*/
extern GUI_FONT     FONT_XINSONGTI_16_4BPP;
extern GUI_FONT     FONT_SIYUANHEITI_20_4BPP;


void Create_SIF_Font(void) ;

#endif /* __GUIFONT_CREATE_H */
