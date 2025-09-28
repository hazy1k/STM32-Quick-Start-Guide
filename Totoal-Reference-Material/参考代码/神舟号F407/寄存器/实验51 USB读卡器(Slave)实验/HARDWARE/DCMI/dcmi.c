#include "sys.h"
#include "dcmi.h" 
#include "lcd.h" 
#include "led.h" 
#include "ov2640.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//DCMI 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/14
//版本：V1.1
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved	
//********************************************************************************
//V1.1 20140516
//修改DCMI_DMA_Init函数，支持双缓冲模式
//////////////////////////////////////////////////////////////////////////////////	 

u8 ov_frame=0;  						//帧率
extern void jpeg_data_process(void);	//JPEG数据处理函数
//DCMI中断服务函数
void DCMI_IRQHandler(void)
{  
	if(DCMI->MISR&0X01)//捕获到一帧图像
	{
		jpeg_data_process();//jpeg数据处理
		DCMI->ICR|=1<<0;	//清除帧中断
		LED1=!LED1;
		ov_frame++;  
	}										 
} 
//DCMI DMA配置
//mem0addr:存储器地址0  将要存储摄像头数据的内存地址(也可以是外设地址)
//mem1addr:存储器地址1  当只使用mem0addr的时候,该值必须为0
//memsize:存储器长度    0~65535
//memblen:存储器位宽    0,8位,1,16位,2,32位
//meminc:存储器增长方式,0,不增长;1,增长
void DCMI_DMA_Init(u32 mem0addr,u32 mem1addr,u16 memsize,u8 memblen,u8 meminc)
{ 
	RCC->AHB1ENR|=1<<22;		//DMA2时钟使能 
 	while(DMA2_Stream1->CR&0X01);//等待DMA2_Stream1可配置 
	DMA2->LIFCR|=0X3D<<6*1;		//清空通道1上所有中断标志
	DMA2_Stream1->FCR=0X0000021;//设置为默认值	
	
	DMA2_Stream1->PAR=(u32)&DCMI->DR;//外设地址为:DCMI->DR
	DMA2_Stream1->M0AR=mem0addr;	//mem0addr作为目标地址0
	DMA2_Stream1->M1AR=mem1addr;	//mem1addr作为目标地址1
	DMA2_Stream1->NDTR=memsize;		//传输长度为memsize
	DMA2_Stream1->CR=0;				//先全部复位CR寄存器值  
	DMA2_Stream1->CR|=0<<6;			//外设到存储器模式 
	DMA2_Stream1->CR|=1<<8;			//循环模式
	DMA2_Stream1->CR|=0<<9;			//外设非增量模式
	DMA2_Stream1->CR|=meminc<<10;	//存储器增量模式
	DMA2_Stream1->CR|=2<<11;		//外设数据长度:32位
	DMA2_Stream1->CR|=memblen<<13;	//存储器位宽,8/16/32bit
	DMA2_Stream1->CR|=2<<16;	//高优先级 
	DMA2_Stream1->CR|=0<<21;	//外设突发单次传输
	DMA2_Stream1->CR|=0<<23;	//存储器突发单次传输
	DMA2_Stream1->CR|=1<<25;	//通道1 DCMI通道  
	if(mem1addr)//双缓冲的时候,才需要开启
	{
		DMA2_Stream1->CR|=1<<18;				//双缓冲模式
		DMA2_Stream1->CR|=1<<4;					//开启传输完成中断
		MY_NVIC_Init(0,0,DMA2_Stream1_IRQn,2);	//抢占1，子优先级3，组2  
	}
}   
void (*dcmi_rx_callback)(void);//DCMI DMA接收回调函数
//DMA2_Stream1中断服务函数(仅双缓冲模式会用到)
void DMA2_Stream1_IRQHandler(void)
{        
	if(DMA2->LISR&(1<<11))	//DMA2_Steam1,传输完成标志
	{ 
		DMA2->LIFCR|=1<<11;	//清除传输完成中断
      	dcmi_rx_callback();	//执行摄像头接收回调函数,读取数据等操作在这里面处理  
	}    											 
}  
//DCMI初始化
void DCMI_Init(void)
{
	//设置IO 
	RCC->AHB1ENR|=1<<0;		//使能外设PORTA时钟
	RCC->AHB1ENR|=1<<1;		//使能外设PORTB时钟
 	RCC->AHB1ENR|=1<<2;		//使能外设PORTC时钟
  	RCC->AHB1ENR|=1<<4;		//使能外设PORTE时钟     
	RCC->AHB2ENR|=1<<0;		//能DCMI时钟 

	GPIO_Set(GPIOA,PIN4|PIN6,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);	//PA4/6   复用功能输出
	GPIO_Set(GPIOB,PIN6|PIN7,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);	//PB6/7   复用功能输出
	GPIO_Set(GPIOC,PIN6|PIN7|PIN8|PIN9|PIN11,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);//PC6/7/8/9/11 复用功能输出
	GPIO_Set(GPIOE,PIN5|PIN6,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);	//PE5/6  复用功能输出 
		
	GPIO_AF_Set(GPIOA,4,13);	//PA4,AF13  DCMI_HSYNC
	GPIO_AF_Set(GPIOA,6,13);	//PA6,AF13  DCMI_PCLK  
	GPIO_AF_Set(GPIOB,7,13);	//PB7,AF13  DCMI_VSYNC 
 	GPIO_AF_Set(GPIOC,6,13);	//PC6,AF13  DCMI_D0
 	GPIO_AF_Set(GPIOC,7,13);	//PC7,AF13  DCMI_D1
 	GPIO_AF_Set(GPIOC,8,13);	//PC8,AF13  DCMI_D2
 	GPIO_AF_Set(GPIOC,9,13);	//PC9,AF13  DCMI_D3
 	GPIO_AF_Set(GPIOC,11,13);	//PC11,AF13 DCMI_D4 
	GPIO_AF_Set(GPIOB,6,13);	//PB6,AF13  DCMI_D5
	GPIO_AF_Set(GPIOE,5,13);	//PE5,AF13  DCMI_D6
	GPIO_AF_Set(GPIOE,6,13);	//PE6,AF13  DCMI_D7
	//清除原来的设置
	DCMI->CR=0x0;	
	DCMI->IER=0x0;
	DCMI->ICR=0x1F;
	DCMI->ESCR=0x0;
	DCMI->ESUR=0x0;
	DCMI->CWSTRTR=0x0;
	DCMI->CWSIZER=0x0;
	
	DCMI->CR|=0<<1;		//连续模式
	DCMI->CR|=0<<2;		//全帧捕获
	DCMI->CR|=0<<4;		//硬件同步HSYNC,VSYNC
	DCMI->CR|=1<<5;		//PCLK 上升沿有效
	DCMI->CR|=0<<6;		//HSYNC 低电平有效
	DCMI->CR|=0<<7;		//VSYNC 低电平有效
	DCMI->CR|=0<<8;		//捕获所有的帧
	DCMI->CR|=0<<10; 	//8位数据格式  
	DCMI->IER|=1<<0; 	//开启帧中断 
	DCMI->CR|=1<<14; 	//DCMI使能
	MY_NVIC_Init(0,0,DCMI_IRQn,2);	//抢占1，子优先级2，组2 
} 
//DCMI,启动传输
void DCMI_Start(void)
{  
  	LCD_SetCursor(0,0);  
	LCD_WriteRAM_Prepare();		//开始写入GRAM
	DMA2_Stream1->CR|=1<<0;		//开启DMA2,Stream1 
	DCMI->CR|=1<<0; 			//DCMI捕获使能  
}
//DCMI,关闭传输
void DCMI_Stop(void)
{
	DCMI->CR&=~(1<<0); 			//DCMI捕获关闭   
	while(DCMI->CR&0X01);		//等待传输结束 
	DMA2_Stream1->CR&=~(1<<0);	//关闭DMA2,Stream1  	
} 
////////////////////////////////////////////////////////////////////////////////
//以下两个函数,供usmart调用,用于调试代码

//DCMI设置显示窗口
//sx,sy;LCD的起始坐标
//width,height:LCD显示范围.
void DCMI_Set_Window(u16 sx,u16 sy,u16 width,u16 height)
{
	DCMI_Stop(); 
	LCD_Clear(WHITE);
	LCD_Set_Window(sx,sy,width,height);
	OV2640_OutSize_Set(width,height);
  	LCD_SetCursor(0,0);  
	LCD_WriteRAM_Prepare();		//开始写入GRAM
	DMA2_Stream1->CR|=1<<0;		//开启DMA2,Stream1 
	DCMI->CR|=1<<0; 			//DCMI捕获使能  
}
   
//通过usmart调试,辅助测试用.
//pclk/hsync/vsync:三个信号的有限电平设置
void DCMI_CR_Set(u8 pclk,u8 hsync,u8 vsync)
{
	DCMI->CR=0;
	DCMI->CR|=pclk<<5;		//PCLK 有效边沿设置
	DCMI->CR|=hsync<<6;		//HSYNC 有效电平设置
	DCMI->CR|=vsync<<7;		//VSYNC 有效电平设置	
	DCMI->CR|=1<<14; 		//DCMI使能
	DCMI->CR|=1<<0; 		//DCMI捕获使能   
}








