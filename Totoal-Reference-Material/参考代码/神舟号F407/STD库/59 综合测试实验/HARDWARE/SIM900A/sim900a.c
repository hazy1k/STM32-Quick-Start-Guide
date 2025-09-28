#include "sim900a.h" 
#include "delay.h"	
#include "led.h"     
#include "w25qxx.h"  
#include "malloc.h"
#include "string.h"    
#include "text.h"		
#include "usart3.h" 
#include "ff.h" 
#include "ucos_ii.h" 

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
 

__sim900dev sim900dev;	//sim900控制器


//sim900a发送命令后,检测接收到的应答
//str:期待的应答结果
//返回值:0,没有得到期待的应答结果
//    其他,期待应答结果的位置(str的位置)
u8* sim900a_check_cmd(u8 *str)
{
	char *strx=0;
	if(USART3_RX_STA&0X8000)		//接收到一次数据了
	{ 
		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//添加结束符
		strx=strstr((const char*)USART3_RX_BUF,(const char*)str);
	} 
	return (u8*)strx;
}
//向sim900a发送命令
//cmd:发送的命令字符串(不需要添加回车了),当cmd<0XFF的时候,发送数字(比如发送0X1A),大于的时候发送字符串.
//ack:期待的应答结果,如果为空,则表示不需要等待应答
//waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果)
//       1,收到非预期结果
//       2,没收到任何回复
u8 sim900a_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0;  
	USART3_RX_STA=0;
	sim900dev.cmdon=1;//进入指令等待状态
	if((u32)cmd<=0XFF)
	{   
		while((USART3->SR&0X40)==0);//等待上一次数据发送完成  
		USART3->DR=(u32)cmd;
	}else u3_printf("%s\r\n",cmd);//发送命令
	if(ack&&waittime)		//需要等待应答
	{
		while(--waittime)	//等待倒计时
		{
			delay_ms(10);
			if(USART3_RX_STA&0X8000)//是否接收到期待的应答结果
			{
				if(sim900a_check_cmd(ack))res=0;//收到期待的结果了
				else res=1;//不是期待的结果
				break; 
			} 
		}
		if(waittime==0)res=2; 
	}
	return res;
}
//命令处理完时调用,与sim900a_send_cmd成对使用/多个与sim900a_send_cmd后调用.
void sim900a_cmd_over(void)
{
	USART3_RX_STA=0;
	sim900dev.cmdon=0;//退出指令等待状态
}
//将1个字符转换为16进制数字
//chr:字符,0~9/A~F/a~F
//返回值:chr对应的16进制数值
u8 sim900a_chr2hex(u8 chr)
{
	if(chr>='0'&&chr<='9')return chr-'0';
	if(chr>='A'&&chr<='F')return (chr-'A'+10);
	if(chr>='a'&&chr<='f')return (chr-'a'+10); 
	return 0;
}
//将1个16进制数字转换为字符
//hex:16进制数字,0~15;
//返回值:字符
u8 sim900a_hex2chr(u8 hex)
{
	if(hex<=9)return hex+'0';
	if(hex>=10&&hex<=15)return (hex-10+'A'); 
	return '0';
}
//unicode gbk 转换函数
//src:输入字符串
//dst:输出(uni2gbk时为gbk内码,gbk2uni时,为unicode字符串)
//mode:0,unicode到gbk转换;
//     1,gbk到unicode转换;
void sim900a_unigbk_exchange(u8 *src,u8 *dst,u8 mode)
{
    OS_CPU_SR cpu_sr=0;
	u16 temp; 
	u8 buf[2];
	OS_ENTER_CRITICAL();//进入临界区(无法被中断打断)  
	if(mode)//gbk 2 unicode
	{
		while(*src!=0)
		{
			if(*src<0X81)	//非汉字
			{
				temp=(u16)ff_convert((WCHAR)*src,1);
				src++;
			}else 			//汉字,占2个字节
			{
				buf[1]=*src++;
				buf[0]=*src++;    
				temp=(u16)ff_convert((WCHAR)*(u16*)buf,1); 
			}
			*dst++=sim900a_hex2chr((temp>>12)&0X0F);
			*dst++=sim900a_hex2chr((temp>>8)&0X0F);
			*dst++=sim900a_hex2chr((temp>>4)&0X0F);
			*dst++=sim900a_hex2chr(temp&0X0F);
		}
	}else	//unicode 2 gbk
	{ 
		while(*src!=0)
		{
			buf[1]=sim900a_chr2hex(*src++)*16;
			buf[1]+=sim900a_chr2hex(*src++);
			buf[0]=sim900a_chr2hex(*src++)*16;
			buf[0]+=sim900a_chr2hex(*src++);
 			temp=(u16)ff_convert((WCHAR)*(u16*)buf,0);
			if(temp<0X80){*dst=temp;dst++;}
			else {*(u16*)dst=swap16(temp);dst+=2;}
		} 
	}
	*dst=0;//添加结束符
	OS_EXIT_CRITICAL();	//退出临界区(可以被中断打断)	
} 

extern void sms_remind_msg(u8 mode);
extern void phone_incall_task_creat(void);
//电话呼入/检测到短信 检测
void sim900a_cmsgin_check(void)
{
	u8 *p1,*p2; 
	u8 num;
	if(sim900dev.cmdon==0&&sim900dev.mode==0)//非指令等待状态,.拨号/短信模式,才检测数据
	{
		if(USART3_RX_STA&0X8000)//收到数据了
		{
			if(sim900a_check_cmd("+CLIP:"))//接收到来电?
			{
				p1=sim900a_check_cmd("+CLIP:");
				p1+=8;
				p2=(u8*)strstr((const char *)p1,"\"");
				p2[0]=0;//添加结束符 
				strcpy((char*)sim900dev.incallnum,(char*)p1);//拷贝号码
				sim900dev.mode=3;			//标记来电了
				phone_incall_task_creat();	//创建来电任务
			}
			if(sim900a_check_cmd("+CMGS:"))//短信发送成功
			{
				sms_remind_msg(1);//提示发送信息成功
			}
			if(sim900a_check_cmd("+CMTI:"))//收到新消息
			{
				if(sim900dev.newmsg<SIM900_MAX_NEWMSG)
				{
					p1=(u8*)strstr((const char*)(USART3_RX_BUF),",");   
					p2=(u8*)strstr((const char*)(p1+1),"\r\n");
					if((p2-p1)==2)num=p1[1]-'0';//1位
					else if((p2-p1)==3)num=(p1[1]-'0')*10+p1[2]-'0';//2位
					else if((p2-p1)==4)num=(p1[1]-'0')*100+(p1[2]-'0')*10+p1[2]-'0';//3位 
					sim900dev.newmsgindex[sim900dev.newmsg]=num;
					sim900dev.newmsg++;
				}
				sms_remind_msg(0);//提示收到新消息
			}			
			USART3_RX_STA=0;
			printf("rev:%s\r\n",USART3_RX_BUF);	
		}
	}
}
//sim900a状态监测
void sim900a_status_check(void)
{
	u8 *p1; 
	if(sim900dev.cmdon==0&&sim900dev.mode==0&&USART3_RX_STA==0)//非指令等待状态.拨号/短信模式/且没有收到任何数据,才允许查询
	{
		if(sim900a_send_cmd("AT+CSQ","OK",25)==0)//查询信号质量,顺便检测GSM模块状态
		{
			p1=(u8*)strstr((const char*)(USART3_RX_BUF),":"); 
			p1+=2;
			sim900dev.csq=(p1[0]-'0')*10+p1[1]-'0';//信号质量
			if(sim900dev.csq>30)sim900dev.csq=30;		
			sim900dev.status|=1<<7;	//查询GSM模块是否在位?
		}else 
		{ 
			sim900dev.csq=0;	
			sim900dev.status=0;	//重新查找
		} 
		if((sim900dev.status&0XC0)==0X80)//CPIN状态,未获取?
		{ 
			sim900a_send_cmd("ATE0","OK",100);//不回显(必须关闭,否则接收数据可能异常)
			if(sim900a_send_cmd("AT+CPIN?","OK",25)==0)sim900dev.status|=1<<6;//SIM卡在位
			else sim900dev.status&=~(1<<6);//SIM卡错误 
		} 
		if((sim900dev.status&0XE0)==0XC0)//运营商名字,未获取?
		{ 
			if(sim900a_send_cmd("AT+COPS?","OK",25)==0)//查询运营商名字
			{ 
				p1=(u8*)strstr((const char*)(USART3_RX_BUF),"MOBILE");//查找MOBILE,看看是不是中国移动?
				if(p1)sim900dev.status&=~(1<<4); //中国移动 
				else 
				{
					p1=(u8*)strstr((const char*)(USART3_RX_BUF),"UNICOM");//查找UNICOM,看看是不是中国联通?
					if(p1)sim900dev.status|=1<<4;	//中国联通 
				}
				if(p1)
				{
					sim900dev.status|=1<<5;	//得到运营商名字了 
					//phone部分通用设置
					sim900a_send_cmd("AT+CLIP=1","OK",100);	//设置来电显示 
					sim900a_send_cmd("AT+COLP=1","OK",100);	//设置被叫号码显示
					//sms部分通用设置
					sim900a_send_cmd("AT+CMGF=1","OK",100);			//设置文本模式 
					sim900a_send_cmd("AT+CSCS=\"UCS2\"","OK",100);	//设置TE字符集为UCS2 
					sim900a_send_cmd("AT+CSMP=17,0,2,25","OK",100);	//设置短消息文本模式参数 
				}
			}else sim900dev.status&=~(1<<5);	//未获得运营商名字
		}
		sim900a_cmd_over();//处理完毕
	}
}























