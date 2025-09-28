#ifndef __CALENDAR_H
#define __CALENDAR_H
#include "sys.h"
#include "includes.h" 	   	 
#include "common.h" 
#include "rtc.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//APP-日历 代码	   
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
     
//时间结构体
typedef struct 
{
	u8 hour;
	u8 min;
	u8 sec;			
	//公历日月年周
	u16 w_year;
	u8  w_month;
	u8  w_date;
	u8  week;	//1~7,代表周1~周日	
}_calendar_obj;					 
extern _calendar_obj calendar;	//日历结构体

//闹钟结构体						   
__packed typedef struct  
{																				   			  
	u8  weekmask;		//闹钟响铃掩码 bit0~bit6,代表周日~周6.   
	u8  ringsta;		//闹铃状态及铃声标志.
						//[7]:0,无闹铃;1,闹铃中;
						//[6:3]:保留
						//[2:0]:闹钟铃声类型
	u8  hour;	   		//闹铃小时
	u8  min;			//闹铃分钟	 
	u8 saveflag;		//保存标志,0X0A,保存过了;其他,还从未保存	   
}_alarm_obj;

extern _alarm_obj alarm;//闹钟结构体
extern u8*const calendar_week_table[GUI_LANGUAGE_NUM][7];//外部定义的星期表

void calendar_alarm_init(_alarm_obj *alarmx,_calendar_obj *calendarx);
void calendar_alarm_ring(u8 type);
void calendar_get_time(_calendar_obj *calendarx);
void calendar_get_date(_calendar_obj *calendarx);
void calendar_date_refresh(void);
void calendar_read_para(_alarm_obj * alarm);
void calendar_save_para(_alarm_obj * alarm);
u8 calendar_alarm_msg(u16 x,u16 y);
void calendar_circle_clock_drawpanel(u16 x,u16 y,u16 size,u16 d);
void calendar_circle_clock_showtime(u16 x,u16 y,u16 size,u16 d,u8 hour,u8 min,u8 sec);
u8 calendar_play(void);
					    				   
#endif












