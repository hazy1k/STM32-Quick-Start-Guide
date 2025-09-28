#include "timer.h"
#include "led.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//定时器 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/4
//版本：V1.1
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved
//********************************************************************************
//修改说明
//V1.1 20140504
//新增TIM14_PWM_Init函数,用于PWM输出
////////////////////////////////////////////////////////////////////////////////// 	 


//定时器3中断服务程序	 
void TIM3_IRQHandler(void)
{ 		    		  			    
	if(TIM3->SR&0X0001)//溢出中断
	{
		LED1=!LED1;			    				   				     	    	
	}				   
	TIM3->SR&=~(1<<0);//清除中断标志位 	    
}
//通用定时器3中断初始化
//这里时钟选择为APB1的2倍，而APB1为42M
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器3!
void TIM3_Int_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<1;	//TIM3时钟使能    
 	TIM3->ARR=arr;  	//设定计数器自动重装值 
	TIM3->PSC=psc;  	//预分频器	  
	TIM3->DIER|=1<<0;   //允许更新中断	  
	TIM3->CR1|=0x01;    //使能定时器3
  	MY_NVIC_Init(1,3,TIM3_IRQn,2);	//抢占1，子优先级3，组2									 
}

//TIM14 PWM部分初始化 
//PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
void TIM14_PWM_Init(u32 arr,u32 psc)
{		 					 
	//此部分需手动修改IO口设置
	RCC->APB1ENR|=1<<8; 	//TIM14时钟使能    
	RCC->AHB1ENR|=1<<5;   	//使能PORTF时钟	
	GPIO_Set(GPIOF,PIN9,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);//复用功能,上拉输出
	GPIO_AF_Set(GPIOF,9,9);	//PF9,AF9 
	
	TIM14->ARR=arr;			//设定计数器自动重装值 
	TIM14->PSC=psc;			//预分频器不分频 
	TIM14->CCMR1|=6<<4;  	//CH1 PWM1模式		 
	TIM14->CCMR1|=1<<3; 	//CH1 预装载使能	   
	TIM14->CCER|=1<<0;   	//OC1 输出使能	
	TIM14->CCER|=1<<1;   	//OC1 低电平有效	   
	TIM14->CR1|=1<<7;   	//ARPE使能 
	TIM14->CR1|=1<<0;    	//使能定时器14 											  
}  













