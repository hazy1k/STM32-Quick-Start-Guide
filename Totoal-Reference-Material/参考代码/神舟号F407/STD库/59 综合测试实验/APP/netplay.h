#ifndef __NETPLAY_H
#define __NETPLAY_H
#include "common.h"  
#include "lwip_comm.h"  
#include "lwip/api.h"
#include "lwip/tcp.h"
#include "lwip/memp.h"
#include "httpd.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//APP-网络测试 代码	   
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

#define NET_RMEMO_MAXLEN		10000		//RMEMO最大接收的字符数.最大不能超过65535
#define NET_SMEMO_MAXLEN		400			//RMEMO最大允许输入400个字符.



#define NET_IP_BACK_COLOR		0X42F0		//IP区域背景颜色
#define NET_COM_RIM_COLOR		0X7BCF		//分割线颜色
#define NET_MSG_FONT_COLOR		0X4A49		//提示消息字体颜色
#define NET_MSG_BACK_COLOR		0XBE3B		//提示消息背景颜色
#define NET_MEMO_BACK_COLOR		0XA599		//2个memo区域背景颜色






void net_load_ui(void);
void net_msg_show(u16 y,u16 height,u8 fsize,u32 tx,u32 rx,u8 prot,u8 flag); 
void net_edit_colorset(_edit_obj *ipx,_edit_obj *portx,u8 prot,u8 connsta); 
u16 net_get_port(u8 *str);
u32 net_get_ip(u8 *str); 
void net_tcpserver_remove_timewait(void); 
void net_disconnect(struct netconn *netconn1,struct netconn *netconn2);
u8 net_test(void);
u8 net_play(void);
#endif















 



















