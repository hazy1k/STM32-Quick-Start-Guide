#ifndef __PHONEPLAY_H
#define __PHONEPLAY_H 	
#include "common.h"  
#include "sim900a.h"  
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//APP-拨号器 代码	   
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

#define PHONE_MAX_INPUT		200		//最大输入200个号码长度 

 

extern u8*const PHONE_CALL_PIC[3];
extern u8*const PHONE_HANGUP_PIC[3];
extern u8*const PHONE_DEL_PIC[3];

__packed typedef struct 
{		
	u16 xoff;		//x方向偏移
	u16 yoff;		//y方向偏移
	u16 width;		//宽度
	u16 height;		//高度
	
	u16 time;		//通话时间
	
	u8 *inbuf;		//输入数字缓冲区
	u8 inlen;		//输入数字长度
	u8 *phonebuf;	//号码缓冲区
	u8 plen;		//号码长度
	u8 psize;		//号码字体大小:28,36,54 
	u8 tsize;		//时间字体大小:12,16,24 
}_phonedis_obj;
extern _phonedis_obj *pdis;


void phone_show_clear(_phonedis_obj *pdis);
void phone_show_time(_phonedis_obj *pdis);
void phone_show_calling(_phonedis_obj *pdis,__sim900dev *gsmx);
void phone_show_phone(_phonedis_obj *pdis,__sim900dev*gsmx);
void phone_show_input(_phonedis_obj *pdis,__sim900dev*gsmx);
void phone_ring(void);
void phone_incall_task(void *pdata);
u8 phone_incall_task_creat(void);
void phone_incall_task_delete(void);
u8 phone_play(void); 
#endif























