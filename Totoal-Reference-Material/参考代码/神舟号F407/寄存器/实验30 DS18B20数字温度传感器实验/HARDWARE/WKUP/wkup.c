#include "wkup.h"
#include "led.h"
#include "delay.h"	
#include "rtc.h"
#include "usart.h"																	   
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
	 
//系统进入待机模式
void Sys_Enter_Standby(void)
{		
	u32 tempreg;			//零时存储寄存器值用
	//关闭所有外设(根据实际情况写) 
 	RCC->AHB1RSTR|=0X01FE;	//复位除GPIOA以外的所有IO口	
  	while(WKUP_KD);			//等待WK_UP按键松开(在有RTC中断时,必须等WK_UP松开再进入待机)
 	RCC->AHB1RSTR|=1<<0;	//复位GPIOA
	
	//STM32F4,当开启了RTC相关中断后,必须先关闭RTC中断,再清中断标志位,然后重新设置
	//RTC中断,再进入待机模式才可以正常唤醒,否则会有问题.
	RCC->APB1ENR|=1<<28;	//使能电源时钟 
	PWR->CR|=1<<8;			//后备区域访问使能(RTC+SRAM) 
	//关闭RTC寄存器写保护
	RTC->WPR=0xCA;
	RTC->WPR=0x53; 
	tempreg=RTC->CR&(0X0F<<12);//记录原来的RTC中断设置
	RTC->CR&=~(0XF<<12);	//关闭RTC所有中断
	RTC->ISR&=~(0X3F<<8);	//清除所有RTC中断标志.  
	PWR->CR|=1<<2;      	//清除Wake-up 标志
	RTC->CR|=tempreg;		//重新设置RTC中断 
	RTC->WPR=0xFF;			//使能RTC寄存器写保护  
	Sys_Standby();	//进入待机模式
}
//检测WKUP脚的信号
//返回值1:连续按下3s以上
//      0:错误的触发	
u8 Check_WKUP(void) 
{
	u8 t=0;
	u8 tx=0;//记录松开的次数
	LED0=0; //亮灯DS0 
	while(1)
	{
		if(WKUP_KD)//已经按下了
		{
			t++;
			tx=0;
		}else 
		{
			tx++; 
			if(tx>3)//超过90ms内没有WKUP信号
			{
				LED0=1;
				return 0;//错误的按键,按下次数不够
			}
		}
		delay_ms(30);
		if(t>=100)//按下超过3秒钟
		{
			LED0=0;	  //点亮DS0 
			return 1; //按下3s以上了
		}
	}
}  
//中断,检测到PA0脚的一个上升沿.	  
//中断线0线上的中断检测
void EXTI0_IRQHandler(void)
{ 		  
	EXTI->PR=1<<0;  //清除LINE10上的中断标志位  
	if(Check_WKUP())//关机?
	{		  
		Sys_Enter_Standby();  
	}
} 
//PA0 WKUP唤醒初始化
void WKUP_Init(void)
{	  		  
	RCC->AHB1ENR|=1<<0;    		//使能PORTA时钟	   
	GPIO_Set(GPIOA,PIN0,GPIO_MODE_IN,0,0,GPIO_PUPD_PD);	//PA0设置 
	//(检查是否是正常开)机    		 
    if(Check_WKUP()==0)
	{
 		Sys_Enter_Standby();	//不是开机,进入待机模式  
	}
	Ex_NVIC_Config(GPIO_A,0,RTIR);	//PA0上升沿触发	    										   		   
	MY_NVIC_Init(2,2,EXTI0_IRQn,2);	//抢占2，子优先级2，组2
}
















