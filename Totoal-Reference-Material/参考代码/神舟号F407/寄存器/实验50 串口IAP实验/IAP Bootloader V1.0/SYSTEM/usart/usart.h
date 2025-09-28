#ifndef __USART_H
#define __USART_H 
#include "sys.h"
#include "stdio.h"	  
//////////////////////////////////////////////////////////////////////////////////	   
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//串口1初始化 
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/2
//版本：V1.2
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved
//********************************************************************************
//修改说明
//V1.1 20150411
//修改OS_CRITICAL_METHOD宏判断为：SYSTEM_SUPPORT_OS
//V1.2 20150804
//修改串口接收架构,用于实现串口IAP升级
////////////////////////////////////////////////////////////////////////////////// 

#define USART_REC_LEN  			120*1024 //定义最大接收字节数 120K
#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收
	  	
extern u8  USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART_RX_STA;         		//接收状态标记	
extern u32 USART_RX_CNT;				//接收的字节数	

void uart_init(u32 pclk2,u32 bound); 
#endif	   
















