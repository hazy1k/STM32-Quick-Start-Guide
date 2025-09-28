#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//定时器 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/4
//版本：V1.6
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved
//********************************************************************************
//修改说明
//V1.1 20140504
//新增TIM14_PWM_Init函数,用于PWM输出
//V1.2 20140504
//新增TIM5_CH1_Cap_Init函数,用于输入捕获
//V1.3 20140506
//新增TIM9_CH2_PWM_Init函数,用于PWM DAC实验
//V1.4 20140723
//新增TIM6_Int_Init函数
//V1.5 20140726
//新增TIM12_CH2_PWM_Init函数
//V1.6 20140807
//新增TIM7_Int_Init函数
////////////////////////////////////////////////////////////////////////////////// 	 

//通过改变TIM14->CCR1的值来改变占空比，从而控制LED0的亮度
#define LED0_PWM_VAL TIM14->CCR1    
//TIM9 CH2作为PWM DAC的输出通道 
#define PWM_DAC_VAL  	TIM9->CCR2  

//LCD PWM背光设置 
#define LCD_BLPWM_VAL   TIM12->CCR2 		

void TIM3_Int_Init(u16 arr,u16 psc);
void TIM14_PWM_Init(u32 arr,u32 psc);
void TIM5_CH1_Cap_Init(u32 arr,u16 psc);
void TIM9_CH2_PWM_Init(u16 arr,u16 psc);
void TIM6_Int_Init(u16 arr,u16 psc);	
void TIM7_Int_Init(u16 arr,u16 psc);
void TIM12_CH2_PWM_Init(u16 arr,u16 psc);
#endif























