#include "spblcd.h"
#include "spb.h"
#include "dma.h"
#include "lcd.h"
#include "delay.h"
#include "malloc.h"
#include "usart.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//SPBLCD 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/7/20
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	


u16 *sramlcdbuf;							//SRAM LCD BUFFER,背景图片显存区 

//在指定位置画点.
//x,y:坐标
//color:颜色.
void slcd_draw_point(u16 x,u16 y,u16 color)
{	 
	sramlcdbuf[y+x*spbdev.spbheight+spbdev.frame*spbdev.spbheight*spbdev.spbwidth]=color;
}
//读取指定位置点的颜色值
//x,y:坐标
//返回值:颜色
u16 slcd_read_point(u16 x,u16 y)
{
	return sramlcdbuf[y+x*spbdev.spbheight+spbdev.frame*spbdev.spbheight*spbdev.spbwidth];
} 
//填充颜色
//x,y:起始坐标
//width，height：宽度和高度
//*color：颜色数组
void slcd_fill_color(u16 x,u16 y,u16 width,u16 height,u16 *color)
{   
	u16 i,j; 
 	for(i=0;i<height;i++)
	{
		for(j=0;j<width;j++)
		{
			slcd_draw_point(x+j,y+i,*color++);
		}	
	}	
} 
//SRAM --> LCD_RAM dma配置
//16位,外部SRAM传输到LCD_RAM. 
void slcd_dma_init(void)
{  
	DMA_InitTypeDef  DMA_InitStructure;
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);//DMA2时钟使能 
 
	while (DMA_GetCmdStatus(DMA2_Stream0) != DISABLE){}//等待DMA2_Stream1可配置 	
	
	DMA_DeInit(DMA2_Stream0); 
	  /* 配置 DMA Stream */
  DMA_InitStructure.DMA_Channel = DMA_Channel_0;  //通道0
  DMA_InitStructure.DMA_Memory0BaseAddr = (u32)&LCD->LCD_RAM;;//DMA 存储器0地址
	DMA_InitStructure.DMA_PeripheralBaseAddr=0;
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToMemory;//存储器到存储器模式
  DMA_InitStructure.DMA_BufferSize = 0;//数据传输量 
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;//外设增量模式
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;//存储器非增量模式
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//外设数据长度:16位
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//存储器数据长度 16位
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;// 使用普通模式 
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;//高优先级
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable; //FIFO模式不开启       
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//外设突发单次传输
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//存储器突发单次传输
  DMA_Init(DMA2_Stream0, &DMA_InitStructure);//初始化DMA Stream

} 

//开启一次SPI到LCD的DMA的传输
//x:起始传输地址编号(0~480)
void slcd_dma_enable(u32 x)
{	  
	u32 lcdsize=spbdev.spbwidth*spbdev.spbheight;
	u32 dmatransfered=0;
	while(lcdsize)
	{ 
		DMA_Cmd(DMA2_Stream0,DISABLE);//关闭DMA2,Stream0
		while (DMA_GetCmdStatus(DMA2_Stream0) != DISABLE){}//等待DMA2_Stream0可配置 	
		DMA_ClearFlag(DMA2_Stream0,DMA_FLAG_TCIF0);//清除传输完成中断
		if(lcdsize>SLCD_DMA_MAX_TRANS)
		{
			lcdsize-=SLCD_DMA_MAX_TRANS;
			DMA_SetCurrDataCounter(DMA2_Stream0,SLCD_DMA_MAX_TRANS);//设置传输长度
		}else
		{
			DMA_SetCurrDataCounter(DMA2_Stream0,lcdsize);//设置传输长度
			lcdsize=0;
		}	
		DMA2_Stream0->PAR=(u32)(sramlcdbuf+x*spbdev.spbheight+dmatransfered);	
		dmatransfered+=SLCD_DMA_MAX_TRANS;		
		DMA_Cmd(DMA2_Stream0,ENABLE);//开启DMA2,Stream0
		while(DMA_GetFlagStatus(DMA2_Stream0,DMA_FLAG_TCIF0)!=SET);//等待传输完成 
	} 
	DMA_Cmd(DMA2_Stream0,DISABLE);//关闭DMA2,Stream0
}
//显示一帧,即启动一次spi到lcd的显示.
//x:坐标偏移量
void slcd_frame_show(u32 x)
{  
	LCD_Scan_Dir(U2D_L2R);		//设置扫描方向  
	if(lcddev.id==0X9341||lcddev.id==0X5310||lcddev.id==0X5510||lcddev.id==0X6804||lcddev.id==0X9481||lcddev.id==0X7789||lcddev.id==0X9486||lcddev.id==0X9488||lcddev.id==0X7796)
	{
		LCD_Set_Window(spbdev.stabarheight,0,spbdev.spbheight,spbdev.spbwidth);
		if(lcddev.id==0X9486||lcddev.id==0X9488)
			LCD_SetCursor9486_9488(spbdev.stabarheight, 0);  //特殊处理的
		else
			LCD_SetCursor(spbdev.stabarheight,0);	//设置光标位置 
	}else
	{
		LCD_Set_Window(0,spbdev.stabarheight,spbdev.spbwidth,spbdev.spbheight);
		if(lcddev.id!=0X1963)LCD_SetCursor(0,spbdev.stabarheight);	//设置光标位置 		
	}
	LCD_WriteRAM_Prepare();     //开始写入GRAM	
	slcd_dma_enable(x);
	LCD_Scan_Dir(DFT_SCAN_DIR);	//恢复默认方向
	LCD_Set_Window(0,0,lcddev.width,lcddev.height);//恢复默认窗口大小
}
 





