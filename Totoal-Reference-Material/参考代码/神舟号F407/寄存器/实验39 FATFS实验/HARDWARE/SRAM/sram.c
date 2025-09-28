#include "sram.h"	  
#include "usart.h"	     
#include "XMRAM.h"	
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//外部SRAM 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/14
//版本：V1.1
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
//********************************************************************************
//V1.1 20140729
//1,修改FSMC_SRAM_Init函数,防止复位后初始化打乱SRAM数据.
////////////////////////////////////////////////////////////////////////////////// 	 

//使用NOR/SRAM的 Bank1.sector3,地址位HADDR[27,26]=10 
//对XM8A51216,地址线范围为A0~A18

#define Bank1_SRAM3_ADDR    ((u32)(0x68000000))	
 					   
//初始化外部SRAM
void FSMC_SRAM_Init(void)
{
	XmRamInit();                //初始化XMRAM 20191024
	delay_us(100);
	
	RCC->AHB1ENR|=0XF<<3;    	//使能PD,PE,PF,PG时钟  
	RCC->AHB3ENR|=1<<0;     	//使能FSMC时钟  
	 
 	GPIO_AF_Set(GPIOG,10,12);	//PG10,AF12(CS放到最前面,防止复位后CS非法变低,破坏原有数据) 
	
	GPIO_Set(GPIOD,(3<<0)|(3<<4)|(0XFF<<8),GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);	//PD0,1,4,5,8~15 AF OUT
 	GPIO_Set(GPIOE,(3<<0)|(0X1FF<<7),GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);			//PE0,1,7~15,AF OUT
	GPIO_Set(GPIOF,(0X3F<<0)|(0XF<<12),GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU); 	 	//PF0~5,12~15
	GPIO_Set(GPIOG,(0X3F<<0)|PIN10,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);	 		//PG0~5,10
 
	
	GPIO_AF_Set(GPIOD,0,12);	//PD0,AF12
 	GPIO_AF_Set(GPIOD,1,12);	//PD1,AF12
 	GPIO_AF_Set(GPIOD,4,12);	//PD4,AF12
 	GPIO_AF_Set(GPIOD,5,12);	//PD5,AF12 
 	GPIO_AF_Set(GPIOD,8,12);	//PD8,AF12
 	GPIO_AF_Set(GPIOD,9,12);	//PD9,AF12
 	GPIO_AF_Set(GPIOD,10,12);	//PD10,AF12 
 	GPIO_AF_Set(GPIOD,11,12);	//PD11,AF12 
 	GPIO_AF_Set(GPIOD,12,12);	//PD12,AF12 
 	GPIO_AF_Set(GPIOD,13,12);	//PD13,AF12 
 	GPIO_AF_Set(GPIOD,14,12);	//PD14,AF12
 	GPIO_AF_Set(GPIOD,15,12);	//PD15,AF12
	
 	GPIO_AF_Set(GPIOE,0,12);	//PE0,AF12
 	GPIO_AF_Set(GPIOE,1,12);	//PE1,AF12
 	GPIO_AF_Set(GPIOE,7,12);	//PE7,AF12
 	GPIO_AF_Set(GPIOE,8,12);	//PE8,AF12
 	GPIO_AF_Set(GPIOE,9,12);	//PE9,AF12
 	GPIO_AF_Set(GPIOE,10,12);	//PE10,AF12
 	GPIO_AF_Set(GPIOE,11,12);	//PE11,AF12
 	GPIO_AF_Set(GPIOE,12,12);	//PE12,AF12
 	GPIO_AF_Set(GPIOE,13,12);	//PE13,AF12
 	GPIO_AF_Set(GPIOE,14,12);	//PE14,AF12
 	GPIO_AF_Set(GPIOE,15,12);	//PE15,AF12
	
 	GPIO_AF_Set(GPIOF,0,12);	//PF0,AF12
 	GPIO_AF_Set(GPIOF,1,12);	//PF1,AF12
 	GPIO_AF_Set(GPIOF,2,12);	//PF2,AF12
 	GPIO_AF_Set(GPIOF,3,12);	//PF3,AF12
 	GPIO_AF_Set(GPIOF,4,12);	//PF4,AF12
 	GPIO_AF_Set(GPIOF,5,12);	//PF5,AF12
 	GPIO_AF_Set(GPIOF,12,12);	//PF12,AF12
 	GPIO_AF_Set(GPIOF,13,12);	//PF13,AF12
 	GPIO_AF_Set(GPIOF,14,12);	//PF14,AF12
 	GPIO_AF_Set(GPIOF,15,12);	//PF15,AF12
	
 	GPIO_AF_Set(GPIOG,0,12);	//PG0,AF12
 	GPIO_AF_Set(GPIOG,1,12);	//PG1,AF12
 	GPIO_AF_Set(GPIOG,2,12);	//PG2,AF12
 	GPIO_AF_Set(GPIOG,3,12);	//PG3,AF12
 	GPIO_AF_Set(GPIOG,4,12);	//PG4,AF12
 	GPIO_AF_Set(GPIOG,5,12);	//PG5,AF12 	
	
	//寄存器清零
	//bank1有NE1~4,每一个有一个BCR+TCR，所以总共八个寄存器。
	//这里我们使用NE3 ，也就对应BTCR[4],[5]。				    
	FSMC_Bank1->BTCR[4]=0X00000000;
	FSMC_Bank1->BTCR[5]=0X00000000;
	FSMC_Bank1E->BWTR[4]=0X00000000;
	//操作BCR寄存器	使用异步模式,模式A(读写共用一个时序寄存器)
	//BTCR[偶数]:BCR寄存器;BTCR[奇数]:BTR寄存器
	FSMC_Bank1->BTCR[4]|=1<<12;//存储器写使能
	FSMC_Bank1->BTCR[4]|=1<<4; //存储器数据宽度为16bit 	    
	//操作BTR寄存器			（HCLK=168M, 1个HCLK=6ns			    
	FSMC_Bank1->BTCR[5]|=12<<8; //数据保持时间（DATAST）为3个HCLK 6*3=18ns	 	 
	FSMC_Bank1->BTCR[5]|=0<<4; //地址保持时间（ADDHLD）未用到	  	 
	FSMC_Bank1->BTCR[5]|=0<<0; //地址建立时间（ADDSET）为1个HCLK 12ns	 

	//闪存写时序寄存器  
	FSMC_Bank1E->BWTR[4]=0x0FFFFFFF;//默认值
	//使能BANK1区域3
	FSMC_Bank1->BTCR[4]|=1<<0; 	
} 														  
//在指定地址(WriteAddr+Bank1_SRAM3_ADDR)开始,连续写入n个字节.
//pBuffer:字节指针
//WriteAddr:要写入的地址
//n:要写入的字节数
void FSMC_SRAM_WriteBuffer(u8* pBuffer,u32 WriteAddr,u32 n)
{
	for(;n!=0;n--)  
	{										    
		*(vu8*)(Bank1_SRAM3_ADDR+WriteAddr)=*pBuffer;	  
		WriteAddr++;
		pBuffer++;
	}   
}																			    
//在指定地址((WriteAddr+Bank1_SRAM3_ADDR))开始,连续读出n个字节.
//pBuffer:字节指针
//ReadAddr:要读出的起始地址
//n:要写入的字节数
void FSMC_SRAM_ReadBuffer(u8* pBuffer,u32 ReadAddr,u32 n)
{
	for(;n!=0;n--)  
	{											    
		*pBuffer++=*(vu8*)(Bank1_SRAM3_ADDR+ReadAddr);    
		ReadAddr++;
	}  
} 
////////////////////////////////////////////////////////////////////////////////////////
//测试函数
//在指定地址写入1个字节
//addr:地址(0~SRAM大小)
//data:要写入的数据
void fsmc_sram_test_write(u32 addr,u8 data)
{			   
	FSMC_SRAM_WriteBuffer(&data,addr,1);//写入1个字节
}
//读取1个字节
//addr:要读取的地址(0~SRAM大小)
//返回值:读取到的数据
u8 fsmc_sram_test_read(u32 addr)
{
	u8 data;
	FSMC_SRAM_ReadBuffer(&data,addr,1);
	return data;
}
