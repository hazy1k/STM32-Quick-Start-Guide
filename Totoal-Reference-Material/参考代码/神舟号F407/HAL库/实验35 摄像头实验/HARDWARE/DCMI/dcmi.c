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
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 

DCMI_HandleTypeDef  DCMI_Handler;           //DCMI句柄
DMA_HandleTypeDef   DMADMCI_Handler;        //DMA句柄

u8 ov_frame=0;  							//帧率
extern void jpeg_data_process(void);		//JPEG数据处理函数

//DCMI中断服务函数
void DCMI_IRQHandler(void)
{
    HAL_DCMI_IRQHandler(&DCMI_Handler);
}

//捕获到一帧图像处理函数
//hdcmi:DCMI句柄
void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef *hdcmi)
{
	__HAL_DCMI_CLEAR_FLAG(&DCMI_Handler,DCMI_FLAG_FRAMERI);//清除帧中断
	jpeg_data_process();//jpeg数据处理
	LED1=!LED1;
	ov_frame++; 
    //重新使能帧中断,因为HAL_DCMI_IRQHandler()函数会关闭帧中断
    __HAL_DCMI_ENABLE_IT(&DCMI_Handler,DCMI_IT_FRAME);
}

//DCMI DMA配置
//memaddr:存储器地址  将要存储摄像头数据的内存地址(也可以是外设地址)
//memblen:存储器位宽,可以为:DMA_MDATAALIGN_BYTE/DMA_MDATAALIGN_HALFWORD/DMA_MDATAALIGN_WORD
//meminc:存储器增长方式,可以为:DMA_MINC_ENABLE/DMA_MINC_DISABLE
void DCMI_DMA_Init(u32 memaddr,u16 memsize,u32 memblen,u32 meminc)
{ 
    __HAL_RCC_DMA2_CLK_ENABLE();                                    //使能DMA2时钟
    __HAL_LINKDMA(&DCMI_Handler,DMA_Handle,DMADMCI_Handler);        //将DMA与DCMI联系起来
	
    DMADMCI_Handler.Instance=DMA2_Stream1;                          //DMA2数据流1                     
    DMADMCI_Handler.Init.Channel=DMA_CHANNEL_1;                     //通道1
    DMADMCI_Handler.Init.Direction=DMA_PERIPH_TO_MEMORY;            //外设到存储器
    DMADMCI_Handler.Init.PeriphInc=DMA_PINC_DISABLE;                //外设非增量模式
    DMADMCI_Handler.Init.MemInc=meminc;                             //存储器增量模式
    DMADMCI_Handler.Init.PeriphDataAlignment=DMA_PDATAALIGN_WORD;   //外设数据长度:32位
    DMADMCI_Handler.Init.MemDataAlignment=memblen;                  //存储器数据长度:8/16/32位
    DMADMCI_Handler.Init.Mode=DMA_CIRCULAR;                         //使用循环模式 
    DMADMCI_Handler.Init.Priority=DMA_PRIORITY_HIGH;                //高优先级
    DMADMCI_Handler.Init.FIFOMode=DMA_FIFOMODE_ENABLE;              //使能FIFO
    DMADMCI_Handler.Init.FIFOThreshold=DMA_FIFO_THRESHOLD_HALFFULL; //使用1/2的FIFO 
    DMADMCI_Handler.Init.MemBurst=DMA_MBURST_SINGLE;                //存储器突发传输
    DMADMCI_Handler.Init.PeriphBurst=DMA_PBURST_SINGLE;             //外设突发单次传输 
    HAL_DMA_DeInit(&DMADMCI_Handler);                               //先清除以前的设置
    HAL_DMA_Init(&DMADMCI_Handler);	                                //初始化DMA
    
    //在开启DMA之前先使用__HAL_UNLOCK()解锁一次DMA,因为HAL_DMA_Statrt()HAL_DMAEx_MultiBufferStart()
    //这两个函数一开始要先使用__HAL_LOCK()锁定DMA,而函数__HAL_LOCK()会判断当前的DMA状态是否为锁定状态，如果是
    //锁定状态的话就直接返回HAL_BUSY，这样会导致函数HAL_DMA_Statrt()和HAL_DMAEx_MultiBufferStart()后续的DMA配置
    //程序直接被跳过！DMA也就不能正常工作，为了避免这种现象，所以在启动DMA之前先调用__HAL_UNLOC()先解锁一次DMA。
    __HAL_UNLOCK(&DMADMCI_Handler);

	HAL_DMA_Start(&DMADMCI_Handler,(u32)&DCMI->DR,memaddr,memsize);
}

//DCMI初始化
void DCMI_Init(void)
{
    DCMI_Handler.Instance=DCMI;
    DCMI_Handler.Init.SynchroMode=DCMI_SYNCHRO_HARDWARE;    //硬件同步HSYNC,VSYNC
    DCMI_Handler.Init.PCKPolarity=DCMI_PCKPOLARITY_RISING;  //PCLK 上升沿有效
    DCMI_Handler.Init.VSPolarity=DCMI_VSPOLARITY_LOW;       //VSYNC 低电平有效
    DCMI_Handler.Init.HSPolarity=DCMI_HSPOLARITY_LOW;       //HSYNC 低电平有效
    DCMI_Handler.Init.CaptureRate=DCMI_CR_ALL_FRAME;        //全帧捕获
    DCMI_Handler.Init.ExtendedDataMode=DCMI_EXTEND_DATA_8B; //8位数据格式 
    HAL_DCMI_Init(&DCMI_Handler);                           //初始化DCMI

	//关闭所有中断，函数HAL_DCMI_Init()会默认打开很多中断，开启这些中断
	//以后我们就需要对这些中断做相应的处理，否则的话就会导致各种各样的问题，
	//但是这些中断很多都不需要，所以这里将其全部关闭掉，也就是将IER寄存器清零。
	//关闭完所有中断以后再根据自己的实际需求来使能相应的中断。
	DCMI->IER=0x0;										
	
	__HAL_DCMI_ENABLE_IT(&DCMI_Handler,DCMI_IT_FRAME);		//开启帧中断
	__HAL_DCMI_ENABLE(&DCMI_Handler);						//使能DCMI
	
}

//DCMI底层驱动，引脚配置，时钟使能，中断配置
//此函数会被HAL_DCMI_Init()调用
//hdcmi:DCMI句柄
void HAL_DCMI_MspInit(DCMI_HandleTypeDef* hdcmi)
{
	 GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_DCMI_CLK_ENABLE();                //使能DCMI时钟

    __HAL_RCC_GPIOA_CLK_ENABLE();               //使能GPIOA时钟
    __HAL_RCC_GPIOB_CLK_ENABLE();               //使能GPIOB时钟
    __HAL_RCC_GPIOC_CLK_ENABLE();               //使能GPIOC时钟
    __HAL_RCC_GPIOE_CLK_ENABLE();               //使能GPIOE时钟
    
    //PA4/6初始化设置
    GPIO_Initure.Pin=GPIO_PIN_4|GPIO_PIN_6;  
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;          //推挽复用
    GPIO_Initure.Pull=GPIO_PULLUP;              //上拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;         //高速
    GPIO_Initure.Alternate=GPIO_AF13_DCMI;      //复用为DCMI   
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);         //初始化
    
    //PB6,7
    GPIO_Initure.Pin=GPIO_PIN_6|GPIO_PIN_7;  
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);         //初始化
    
    //PC6,7,8,9,11
    GPIO_Initure.Pin=GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|\
                     GPIO_PIN_9|GPIO_PIN_11;  
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);         //初始化
    
	//PE5,6 
    GPIO_Initure.Pin=GPIO_PIN_5|GPIO_PIN_6; 
    HAL_GPIO_Init(GPIOE,&GPIO_Initure);         //初始化
    
    HAL_NVIC_SetPriority(DCMI_IRQn,0,0);        //抢占优先级0，子优先级0
    HAL_NVIC_EnableIRQ(DCMI_IRQn);              //使能DCMI中断
}

//DCMI,启动传输
void DCMI_Start(void)
{  
  	LCD_SetCursor(0,0);  
	LCD_WriteRAM_Prepare();					//开始写入GRAM
	__HAL_DMA_ENABLE(&DMADMCI_Handler); 	//使能DMA
	DCMI->CR|=DCMI_CR_CAPTURE;    			//DCMI捕获使能 
}

//DCMI,关闭传输
void DCMI_Stop(void)
{
	DCMI->CR&=~(DCMI_CR_CAPTURE);  			//关闭捕获  
	while(DCMI->CR&0X01);					//等待传输结束 
	__HAL_DMA_DISABLE(&DMADMCI_Handler);	//关闭DMA 	
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








