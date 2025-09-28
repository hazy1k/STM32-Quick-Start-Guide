#ifndef __CAMERA_H
#define __CAMERA_H 	
#include "common.h"	   
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//APP-照相机 代码	   
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

#define OV2640_RGB565_MODE		0		//rgb565模式
#define OV2640_JPEG_MODE		1		//jpeg模式

extern vu8 hsync_int;					//帧中断标志
extern vu8 jpeg_size;					//jpeg图片分辨率
extern vu8 ov2640_mode;					//工作模式:0,RGB565模式;1,JPEG模式

extern vu32 jpeg_buf_size;  			//定义JPEG数据缓存jpeg_buf的大小(*4字节)
extern vu32 jpeg_data_len; 				//buf中的JPEG有效数据长度 
extern vu8 jpeg_data_ok;				//JPEG数据采集完成标志 
										//0,数据没有采集完;
										//1,数据采集完了,但是还没处理;
										//2,数据已经处理完成了,可以开始下一帧接收
										
extern u32 *jpeg_buf;		 			//JPEG数据缓存buf,通过malloc申请内存


void jpeg_data_process(void);
void sw_ov2640_mode(void);
void sw_sdcard_mode(void);
void camera_new_pathname(u8 *pname,u8 mode);
u8 ov2640_jpg_photo(u8 *pname);
u8 camera_play(void);
#endif























