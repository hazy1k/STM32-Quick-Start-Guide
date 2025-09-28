#ifndef __SMSPLAY_H
#define __SMSPLAY_H 
#include "common.h"  
#include "sim900a.h"  
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//APP-短信测试 代码	   
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
#define SMS_RMEMO_MAXLEN		2000		//最多可以输入2K字节内容

#define SMS_PNUM_BACK_COLOR		0X42F0		//手机号码(联系人/收件人)区域背景色
#define SMS_PNUM_NAME_COLOR		WHITE		//联系人/收件人字体颜色
#define SMS_PNUM_NUM_COLOR		WHITE		//号码颜色

#define SMS_COM_RIM_COLOR		0X9D36		//通用分割线

#define SMS_RMSG_BACK_COLOR		0XBE3B		//短信接收时间区域背景色
#define SMS_RMSG_FONT_COLOR		BLACK		//时间字体颜色

#define SMS_RMEMO_BACK_COLOR	WHITE		//接收的短信MEMO控件背景色
#define SMS_RMEMO_FONT_COLOR	BLACK		//接收的短信字体颜色

#define SMS_SAREA_BACK_COLOR	0XA599  	//发送区域背景色
#define SMS_SMEMO_BACK_COLOR	WHITE		//发送短信MEMO控件背景色
#define SMS_SMEMO_FONT_COLOR	BLACK		//发送短信字体颜色


//短信链表节点
typedef  struct __sms_node 
{	
	u8* pnum;	//电话号码指针
	u8* msg;	//短信内容指针
	u8* time;	//接收时间指针
	u8 *item;	//用于展示的指针=pnum+msg+time
	u8 index;	//该短信在SIM卡的位置
	u8 readed;	//0,未读短信;1,已读短信.
	struct __sms_node *next;//指向下一个节点
}sms_node; 

sms_node* sms_node_creat(void);

void sms_node_free(sms_node * pnode,u8 mode);
u8 sms_node_insert(sms_node* head,sms_node *pnew,u16 i);
u8 sms_node_delete(sms_node* head,u16 i);
sms_node* sms_node_getnode(sms_node* head,u16 i);
u16 sms_node_getsize(sms_node* head);
void sms_node_destroy(sms_node* head);

void sms_remind_msg(u8 mode);
u8 sms_read_sms(sms_node *pnode,u16 index,u8 mode);
u8 sms_delete_sms(sms_node **head,u16 index);
u8 sms_get_smsnum(u16 *num,u16 *max);
u8 sms_msg_select(u16 *sel,u16 *top,u8 * caption,sms_node* head);
u16 sms_read_all_sms(sms_node *head,u16 *maxnum);
void sms_send_sms(sms_node* pnode,u8 mode);
u8 sms_play(void);
#endif



















