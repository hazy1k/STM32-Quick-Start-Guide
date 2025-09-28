#ifndef __PAINT_H
#define __PAINT_H 	
#include "common.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//APP-画板 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/7/20
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//*******************************************************************************
//修改信息
//无
////////////////////////////////////////////////////////////////////////////////// 	   

////////////////////////////////////////////////////////////////////////////////////////////
//各图标/图片路径
extern u8*const PAINT_COLOR_TBL_PIC;//颜色表路径
////////////////////////////////////////////////////////////////////////////////////////////

//RGB565位域定义
typedef struct 
{
	u16 b:5;
	u16 g:6;
	u16 r:5; 
}PIX_RGB565;

void paint_new_pathname(u8 *pname);
void paint_show_colorval(u16 xr,u16 yr,u16 color);
u8 paint_pen_color_set(u16 x,u16 y,u16* color,u8*caption);
u8 paint_pen_size_set(u16 x,u16 y,u16 color,u8 *mode,u8*caption);
void paint_draw_point(u16 x,u16 y,u16 color,u8 mode);	 
u8 paint_play(void);


#endif























