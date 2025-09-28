#ifndef __WKUP_H
#define __WKUP_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//待机唤醒 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/6
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////
					    
#define WKUP_KD PAin(0)			//PA0 检测是否外部WK_UP按键按下
	 
u8 Check_WKUP(void);  			//检测WKUP脚的信号
void WKUP_Init(void); 			//PA0 WKUP唤醒初始化
void Sys_Enter_Standby(void);	//系统进入待机模式
#endif


