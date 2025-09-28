#ifndef __SPBLCD_H
#define	__SPBLCD_H	   
#include "sys.h"  
#include "lcd.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//SPBLCD 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/7/20
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

#define SLCD_DMA_MAX_TRANS	60*1024		//DMA一次最多传输60K字节	
extern u16 *sramlcdbuf;					//SRAMLCD缓存,先在SRAM 里面将图片解码,并加入图标以及文字等信息

										
void slcd_draw_point(u16 x,u16 y,u16 color);
u16 slcd_read_point(u16 x,u16 y);
void slcd_fill_color(u16 x,u16 y,u16 width,u16 height,u16 *color);
void slcd_frame_sram2spi(u8 frame); 
void slcd_spi1_mode(u8 mode);
void slcd_dma_init(void);
void slcd_dma_enable(u32 x);  
void slcd_frame_show(u32 x);


#endif

























