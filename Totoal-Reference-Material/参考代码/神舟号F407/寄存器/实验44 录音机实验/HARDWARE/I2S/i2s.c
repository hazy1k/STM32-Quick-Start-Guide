#include "i2s.h"  
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//I2S 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/24
//版本：V1.1
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
//********************************************************************************
//V1.1 20140606
//新增I2S2ext_Init、I2S2ext_RX_DMA_Init、I2S_Rec_Start和I2S_Rec_Stop等函数
//V1.2 20141220  
//修正I2S2_SampleRate_Set函数ODD位设置的bug
////////////////////////////////////////////////////////////////////////////////// 	
 

//I2S2初始化
//std:I2S标准,00,飞利浦标准;01,MSB对齐标准(右对齐);10,LSB对齐标准(左对齐);11,PCM标准
//mode:I2S工作模式,00,从机发送;01,从机接收;10,主机发送;11,主机接收
//cpol:0,时钟低电平有效;1,时钟高电平有效
//datalen:数据长度,0,16位标准;1,16位扩展(frame=32bit);2,24位;3,32位.
void I2S2_Init(u8 std,u8 mode,u8 cpol,u8 datalen)
{ 
	RCC->APB1ENR|=1<<14;	//使能SPI2时钟
	RCC->APB1RSTR|=1<<14;	//复位SPI2
	RCC->APB1RSTR&=~(1<<14);//结束复位  
	SPI2->I2SCFGR=0;		//全部设置为0
	SPI2->I2SPR=0X02;		//分频寄存器为默认值
	SPI2->I2SCFGR|=1<<11;	//选择:I2S模式
	SPI2->I2SCFGR|=(u16)mode<<8;//I2S工作模式设置
	SPI2->I2SCFGR|=std<<4;		//I2S标准设置 
	SPI2->I2SCFGR|=cpol<<3;		//空闲时钟电平设置
	if(datalen)					//非标准16位长度
	{
		SPI2->I2SCFGR|=1<<0;	//Channel长度为32位 
		datalen-=1;		
	}else SPI2->I2SCFGR|=0<<0;	//Channel长度为16位  
	SPI2->I2SCFGR|=datalen<<1;	//I2S标准设置 
	SPI2->CR2|=1<<1;			//SPI2 TX DMA请求使能.
	SPI2->I2SCFGR|=1<<10;		//SPI2 I2S EN使能.	
} 
//I2S2ext配置,用于全双工模式配置,需要时,在I2S2_Init之后调用
//std:I2S标准,00,飞利浦标准;01,MSB对齐标准(右对齐);10,LSB对齐标准(左对齐);11,PCM标准
//mode:I2S工作模式,00,从机发送;01,从机接收;I2S2ext仅用于从机模式
//cpol:0,时钟低电平有效;1,时钟高电平有效
//datalen:数据长度,0,16位标准;1,16位扩展(frame=32bit);2,24位;3,32位.
void I2S2ext_Init(u8 std,u8 mode,u8 cpol,u8 datalen)
{  
	I2S2ext->I2SCFGR=0;				//全部设置为0
	I2S2ext->I2SPR=0X02;			//分频寄存器为默认值
	I2S2ext->I2SCFGR|=1<<11;		//选择:I2S模式
	I2S2ext->I2SCFGR|=(u16)mode<<8;	//I2S工作模式设置
	I2S2ext->I2SCFGR|=std<<4;		//I2S标准设置 
	I2S2ext->I2SCFGR|=cpol<<3;		//空闲时钟电平设置
	if(datalen)						//非标准16位长度
	{
		I2S2ext->I2SCFGR|=1<<0;		//Channel长度为32位 
		datalen-=1;		
	}else I2S2ext->I2SCFGR|=0<<0;	//Channel长度为16位  
	I2S2ext->I2SCFGR|=datalen<<1;	//I2S标准设置 
	I2S2ext->CR2|=1<<0;				//I2S2ext RX DMA请求使能.
	I2S2ext->I2SCFGR|=1<<10;		//I2S2ext I2S EN使能.	
} 

//采样率计算公式:Fs=I2SxCLK/[256*(2*I2SDIV+ODD)]
//I2SxCLK=(HSE/pllm)*PLLI2SN/PLLI2SR
//一般HSE=8Mhz 
//pllm:在Sys_Clock_Set设置的时候确定，一般是8
//PLLI2SN:一般是192~432 
//PLLI2SR:2~7
//I2SDIV:2~255
//ODD:0/1
//I2S分频系数表@pllm=8,HSE=8Mhz,即vco输入频率为1Mhz
//表格式:采样率/10,PLLI2SN,PLLI2SR,I2SDIV,ODD
const u16 I2S_PSC_TBL[][5]=
{
	{800 ,256,5,12,1},		//8Khz采样率
	{1102,429,4,19,0},		//11.025Khz采样率 
	{1600,213,2,13,0},		//16Khz采样率
	{2205,429,4, 9,1},		//22.05Khz采样率
	{3200,213,2, 6,1},		//32Khz采样率
	{4410,271,2, 6,0},		//44.1Khz采样率
	{4800,258,3, 3,1},		//48Khz采样率
	{8820,316,2, 3,1},		//88.2Khz采样率
	{9600,344,2, 3,1},  	//96Khz采样率
	{17640,361,2,2,0},  	//176.4Khz采样率 
	{19200,393,2,2,0},  	//192Khz采样率
};  
//设置IIS的采样率(@MCKEN)
//samplerate:采样率,单位:Hz
//返回值:0,设置成功;1,无法设置.
u8 I2S2_SampleRate_Set(u32 samplerate)
{ 
	u8 i=0; 
	u32 tempreg=0;
	samplerate/=10;//缩小10倍   
	for(i=0;i<(sizeof(I2S_PSC_TBL)/10);i++)//看看改采样率是否可以支持
	{
		if(samplerate==I2S_PSC_TBL[i][0])break;
	}
	RCC->CR&=~(1<<26);						//先关闭PLLI2S
	if(i==(sizeof(I2S_PSC_TBL)/10))return 1;//搜遍了也找不到
	tempreg|=(u32)I2S_PSC_TBL[i][1]<<6;		//设置PLLI2SN
	tempreg|=(u32)I2S_PSC_TBL[i][2]<<28;	//设置PLLI2SR
	RCC->PLLI2SCFGR=tempreg;		//设置I2SxCLK的频率(x=2) 
	RCC->CR|=1<<26;					//开启I2S时钟
	while((RCC->CR&1<<27)==0);		//等待I2S时钟开启成功. 
	tempreg=I2S_PSC_TBL[i][3]<<0;	//设置I2SDIV
	tempreg|=I2S_PSC_TBL[i][4]<<8;	//设置ODD位
	tempreg|=1<<9;					//使能MCKOE位,输出MCK
	SPI2->I2SPR=tempreg;			//设置I2SPR寄存器 
	return 0;
}  
//I2S2 TX DMA配置
//设置为双缓冲模式,并开启DMA传输完成中断
//buf0:M0AR地址.
//buf1:M1AR地址.
//num:每次传输数据量
void I2S2_TX_DMA_Init(u8* buf0,u8 *buf1,u16 num)
{  
	RCC->AHB1ENR|=1<<21;		//DMA1时钟使能   
	while(DMA1_Stream4->CR&0X01);//等待DMA1_Stream4可配置 
	DMA1->HIFCR|=0X3D<<6*0;		//清空通道4上所有中断标志
	DMA1_Stream4->FCR=0X0000021;//设置为默认值	
	
	DMA1_Stream4->PAR=(u32)&SPI2->DR;		//外设地址为:SPI2->DR
	DMA1_Stream4->M0AR=(u32)buf0;//暂不设置
	DMA1_Stream4->M1AR=(u32)buf1;//暂不设置
	DMA1_Stream4->NDTR=num;		//暂时设置长度为1
	DMA1_Stream4->CR=0;			//先全部复位CR寄存器值  
	DMA1_Stream4->CR|=1<<6;		//存储器到外设模式 
	DMA1_Stream4->CR|=1<<8;		//循环模式
	DMA1_Stream4->CR|=0<<9;		//外设非增量模式
	DMA1_Stream4->CR|=1<<10;	//存储器增量模式
	DMA1_Stream4->CR|=1<<11;	//外设数据长度:16位
	DMA1_Stream4->CR|=1<<13;	//存储器数据长度:16位
	DMA1_Stream4->CR|=2<<16;	//高优先级
	DMA1_Stream4->CR|=1<<18;	//双缓冲模式
	DMA1_Stream4->CR|=0<<21;	//外设突发单次传输
	DMA1_Stream4->CR|=0<<23;	//存储器突发单次传输
	DMA1_Stream4->CR|=0<<25;	//选择通道0 SPI2_TX通道 

	DMA1_Stream4->FCR&=~(1<<2);	//不使用FIFO模式
	DMA1_Stream4->FCR&=~(3<<0);	//无FIFO 设置
	
	DMA1_Stream4->CR|=1<<4;		//开启传输完成中断
	MY_NVIC_Init(0,0,DMA1_Stream4_IRQn,2);	//抢占1，子优先级0，组2  
} 
//I2S2ext RX DMA配置
//设置为双缓冲模式,并开启DMA传输完成中断
//buf0:M0AR地址.
//buf1:M1AR地址.
//num:每次传输数据量
void I2S2ext_RX_DMA_Init(u8* buf0,u8 *buf1,u16 num)
{  
	RCC->AHB1ENR|=1<<21;		//DMA1时钟使能  
	while(DMA1_Stream3->CR&0X01);//等待DMA1_Stream3可配置 
	DMA1->LIFCR|=0X3D<<22;		//清空通道3上所有中断标志
	DMA1_Stream3->FCR=0X0000021;//设置为默认值	
	
	DMA1_Stream3->PAR=(u32)&I2S2ext->DR;		//外设地址为:I2S2ext->DR
	DMA1_Stream3->M0AR=(u32)buf0;//暂不设置
	DMA1_Stream3->M1AR=(u32)buf1;//暂不设置
	DMA1_Stream3->NDTR=num;		//设置DMA传输长度
	DMA1_Stream3->CR=0;			//先全部复位CR寄存器值  
	DMA1_Stream3->CR|=0<<6;		//外设到存储器模式 
	DMA1_Stream3->CR|=1<<8;		//循环模式
	DMA1_Stream3->CR|=0<<9;		//外设非增量模式
	DMA1_Stream3->CR|=1<<10;	//存储器增量模式
	DMA1_Stream3->CR|=1<<11;	//外设数据长度:16位
	DMA1_Stream3->CR|=1<<13;	//存储器数据长度:16位
	DMA1_Stream3->CR|=1<<16;	//中等优先级
	DMA1_Stream3->CR|=1<<18;	//双缓冲模式
	DMA1_Stream3->CR|=0<<21;	//外设突发单次传输
	DMA1_Stream3->CR|=0<<23;	//存储器突发单次传输
	DMA1_Stream3->CR|=3<<25;	//选择通道3 I2S2ext_RX通道 

	DMA1_Stream3->FCR&=~(1<<2);	//不使用FIFO模式
	DMA1_Stream3->FCR&=~(3<<0);	//无FIFO 设置
	
	DMA1_Stream3->CR|=1<<4;		//开启传输完成中断
	MY_NVIC_Init(0,1,DMA1_Stream3_IRQn,2);	//抢占1，子优先级1，组2  
} 

//I2S DMA回调函数指针
void (*i2s_tx_callback)(void);	//TX回调函数
void (*i2s_rx_callback)(void);	//RX回调函数

//DMA1_Stream4中断服务函数
void DMA1_Stream4_IRQHandler(void)
{      
	if(DMA1->HISR&(1<<5))	//DMA1_Steam4,传输完成标志
	{ 
		DMA1->HIFCR|=1<<5;	//清除传输完成中断
      	i2s_tx_callback();	//执行回调函数,读取数据等操作在这里面处理  
	}   											 
} 
//DMA1_Stream3中断服务函数
void DMA1_Stream3_IRQHandler(void)
{      
	if(DMA1->LISR&(1<<27))	//DMA1_Steam3,传输完成标志
	{ 
		DMA1->LIFCR|=1<<27;	//清除传输完成中断
      	i2s_rx_callback();	//执行回调函数,读取数据等操作在这里面处理  
	}  											 
} 
//I2S开始播放
void I2S_Play_Start(void)
{   	  
	DMA1_Stream4->CR|=1<<0;		//开启DMA TX传输  		
}
//关闭I2S播放
void I2S_Play_Stop(void)
{   	 
	DMA1_Stream4->CR&=~(1<<0);	//结束播放	 	 
}
//I2S开始录音
void I2S_Rec_Start(void)
{   	    
	DMA1_Stream3->CR|=1<<0;		//开启DMA RX传输 		
}
//关闭I2S录音
void I2S_Rec_Stop(void)
{   	  
	DMA1_Stream3->CR&=~(1<<0);	//结束录音		 
}








