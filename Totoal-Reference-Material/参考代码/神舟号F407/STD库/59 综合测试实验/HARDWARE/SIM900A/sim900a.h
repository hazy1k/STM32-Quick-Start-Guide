#ifndef __SIM900A_H__
#define __SIM900A_H__	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	   
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//SIM900A 初始化 
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/8/1
//版本：V1.1
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved
//********************************************************************************
//修改说明
//V1.1 20140810
//1,新增__sim900dev结构体
//2,修改sim900a_send_cmd函数,新增非预期结果返回
////////////////////////////////////////////////////////////////////////////////// 	
 
#define SIM900_MAX_NEWMSG	10		//最大10条新消息

typedef struct 
{							  
 	u8 status;		//SIM900A状态
					//bit7:0,没有找到模块;1,找到模块了
					//bit6:0,SIM卡不正常;1,SIM卡正常
					//bit5:0,未获得运营商名字;1,已获得运营商名字
					//bit4:0,中国移动;1,中国联通
					//[3:0]:保留
	
	u8 mode;		//当前工作模式
					//0,号码输入模式/短信模式
					//1,拨号中
					//2,通话中
					//3,来电响应中
	
	vu8 cmdon;		//标记是否有指令在发送等待状态
					//0,没有指令在等待回应
					//1,有指令在等待回应
	
	u8 csq;			//信号质量
	
	vu8 newmsg;		//新消息条数,0,没有新消息;其他,新消息条数
	u8 newmsgindex[SIM900_MAX_NEWMSG];//新短信在SIM卡内的索引,最长记录SIM900_MAX_NEWMSG条新短信
	u8 incallnum[20];//来电号码缓存区,最长20位
}__sim900dev; 

extern __sim900dev sim900dev;	//sim900控制器

 
 
#define swap16(x) (x&0XFF)<<8|(x&0XFF00)>>8		//高低字节交换宏定义

 

u8* sim900a_check_cmd(u8 *str);
u8 sim900a_send_cmd(u8 *cmd,u8 *ack,u16 waittime);
void sim900a_cmd_over(void);
u8 sim900a_chr2hex(u8 chr);
u8 sim900a_hex2chr(u8 hex);
void sim900a_unigbk_exchange(u8 *src,u8 *dst,u8 mode);
void sim900a_cmsgin_check(void);
void sim900a_status_check(void);
#endif





