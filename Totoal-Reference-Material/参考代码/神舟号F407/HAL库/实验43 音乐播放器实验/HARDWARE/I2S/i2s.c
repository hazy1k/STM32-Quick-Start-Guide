#include "i2s.h"  
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//I2S 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2017/5/3
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
//******************************************************************************** 
//V1.1 20141220  
//修正I2S2_SampleRate_Set函数ODD位设置的bug 	
////////////////////////////////////////////////////////////////////////////////// 	
 
I2S_HandleTypeDef I2S2_Handler;			//I2S2句柄
DMA_HandleTypeDef I2S2_TXDMA_Handler;   //I2S2发送DMA句柄

//I2S2初始化
//I2S_Standard:I2S标准,可以设置：I2S_STANDARD_PHILIPS/I2S_STANDARD_MSB/
//				       I2S_STANDARD_LSB/I2S_STANDARD_PCM_SHORT/I2S_STANDARD_PCM_LONG
//I2S_Mode:I2S工作模式,可以设置：I2S_MODE_SLAVE_TX/I2S_MODE_SLAVE_RX/I2S_MODE_MASTER_TX/I2S_MODE_MASTER_RX
//I2S_Clock_Polarity:时钟电平，可以设置为:I2S_CPOL_LOW/I2S_CPOL_HIGH
//I2S_DataFormat:数据长度,可以设置：I2S_DATAFORMAT_16B/I2S_DATAFORMAT_16B_EXTENDED/I2S_DATAFORMAT_24B/I2S_DATAFORMAT_32B
void I2S2_Init(u32 I2S_Standard,u32 I2S_Mode,u32 I2S_Clock_Polarity,u32 I2S_DataFormat)
{
	I2S2_Handler.Instance=SPI2;
	I2S2_Handler.Init.Mode=I2S_Mode;					//IIS模式
	I2S2_Handler.Init.Standard=I2S_Standard;			//IIS标准
	I2S2_Handler.Init.DataFormat=I2S_DataFormat;		//IIS数据长度
	I2S2_Handler.Init.MCLKOutput=I2S_MCLKOUTPUT_ENABLE;	//主时钟输出使能
	I2S2_Handler.Init.AudioFreq=I2S_AUDIOFREQ_DEFAULT;	//IIS频率设置
	I2S2_Handler.Init.CPOL=I2S_Clock_Polarity;			//空闲状态时钟电平
	I2S2_Handler.Init.ClockSource=I2S_CLOCK_PLL;		//IIS时钟源为PLL
	HAL_I2S_Init(&I2S2_Handler); 
	
	SPI2->CR2|=1<<1;									//SPI2 TX DMA请求使能.
	__HAL_I2S_ENABLE(&I2S2_Handler);					//使能I2S2	
}

//I2S底层驱动，时钟使能，引脚配置，DMA配置
//此函数会被HAL_I2S_Init()调用
//hi2s:I2S句柄
void HAL_I2S_MspInit(I2S_HandleTypeDef *hi2s)
{
	GPIO_InitTypeDef GPIO_Initure;
    
	__HAL_RCC_SPI2_CLK_ENABLE();        		//使能SPI2/I2S2时钟
	__HAL_RCC_GPIOB_CLK_ENABLE();               //使能GPIOB时钟
	__HAL_RCC_GPIOC_CLK_ENABLE();               //使能GPIOC时钟
	
    //初始化PB12/13
    GPIO_Initure.Pin=GPIO_PIN_12|GPIO_PIN_13;  
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;          //推挽复用
    GPIO_Initure.Pull=GPIO_PULLUP;              //上拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;         //高速
    GPIO_Initure.Alternate=GPIO_AF5_SPI2;       //复用为SPI/I2S  
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);         //初始化
	
	//初始化PC2/PC3/PC6
	GPIO_Initure.Pin=GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_6; 
	HAL_GPIO_Init(GPIOC,&GPIO_Initure);         //初始化	
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

//开启I2S的DMA功能,HAL库没有提供此函数
//因此我们需要自己操作寄存器编写一个
void I2S_DMA_Enable(void)
{
    u32 tempreg=0;
    tempreg=SPI2->CR2;    	//先读出以前的设置			
	tempreg|=1<<1;			//使能DMA
	SPI2->CR2=tempreg;		//写入CR1寄存器中
}

//设置SAIA的采样率(@MCKEN)
//samplerate:采样率,单位:Hz
//返回值:0,设置成功;1,无法设置.
u8 I2S2_SampleRate_Set(u32 samplerate)
{   
    u8 i=0; 
	u32 tempreg=0;
    RCC_PeriphCLKInitTypeDef RCCI2S2_ClkInitSture;  
	
	for(i=0;i<(sizeof(I2S_PSC_TBL)/10);i++)//看看改采样率是否可以支持
	{
		if((samplerate/10)==I2S_PSC_TBL[i][0])break;
	}
    if(i==(sizeof(I2S_PSC_TBL)/10))return 1;//搜遍了也找不到
	
    RCCI2S2_ClkInitSture.PeriphClockSelection=RCC_PERIPHCLK_I2S;	//外设时钟源选择 
    RCCI2S2_ClkInitSture.PLLI2S.PLLI2SN=(u32)I2S_PSC_TBL[i][1];    	//设置PLLI2SN
    RCCI2S2_ClkInitSture.PLLI2S.PLLI2SR=(u32)I2S_PSC_TBL[i][2];    	//设置PLLI2SR
    HAL_RCCEx_PeriphCLKConfig(&RCCI2S2_ClkInitSture);             	//设置时钟
	
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
    __HAL_RCC_DMA1_CLK_ENABLE();                                    		//使能DMA1时钟
    __HAL_LINKDMA(&I2S2_Handler,hdmatx,I2S2_TXDMA_Handler);         		//将DMA与I2S联系起来
	
    I2S2_TXDMA_Handler.Instance=DMA1_Stream4;                       		//DMA1数据流4                     
    I2S2_TXDMA_Handler.Init.Channel=DMA_CHANNEL_0;                  		//通道0
    I2S2_TXDMA_Handler.Init.Direction=DMA_MEMORY_TO_PERIPH;         		//存储器到外设模式
    I2S2_TXDMA_Handler.Init.PeriphInc=DMA_PINC_DISABLE;             		//外设非增量模式
    I2S2_TXDMA_Handler.Init.MemInc=DMA_MINC_ENABLE;                 		//存储器增量模式
    I2S2_TXDMA_Handler.Init.PeriphDataAlignment=DMA_PDATAALIGN_HALFWORD;   	//外设数据长度:16位
    I2S2_TXDMA_Handler.Init.MemDataAlignment=DMA_MDATAALIGN_HALFWORD;    	//存储器数据长度:16位
    I2S2_TXDMA_Handler.Init.Mode=DMA_CIRCULAR;                      		//使用循环模式 
    I2S2_TXDMA_Handler.Init.Priority=DMA_PRIORITY_HIGH;             		//高优先级
    I2S2_TXDMA_Handler.Init.FIFOMode=DMA_FIFOMODE_DISABLE;          		//不使用FIFO
    I2S2_TXDMA_Handler.Init.MemBurst=DMA_MBURST_SINGLE;             		//存储器单次突发传输
    I2S2_TXDMA_Handler.Init.PeriphBurst=DMA_PBURST_SINGLE;          		//外设突发单次传输 
    HAL_DMA_DeInit(&I2S2_TXDMA_Handler);                            		//先清除以前的设置
    HAL_DMA_Init(&I2S2_TXDMA_Handler);	                            		//初始化DMA

    HAL_DMAEx_MultiBufferStart(&I2S2_TXDMA_Handler,(u32)buf0,(u32)&SPI2->DR,(u32)buf1,num);//开启双缓冲
    __HAL_DMA_DISABLE(&I2S2_TXDMA_Handler);                         		//先关闭DMA 
    delay_us(10);                                                   		//10us延时，防止-O2优化出问题 	
    __HAL_DMA_ENABLE_IT(&I2S2_TXDMA_Handler,DMA_IT_TC);             		//开启传输完成中断
    __HAL_DMA_CLEAR_FLAG(&I2S2_TXDMA_Handler,DMA_FLAG_TCIF0_4);     		//清除DMA传输完成中断标志位
    HAL_NVIC_SetPriority(DMA1_Stream4_IRQn,0,0);                    		//DMA中断优先级
    HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);
} 

//I2S DMA回调函数指针
void (*i2s_tx_callback)(void);	//TX回调函数 
//DMA1_Stream4中断服务函数
void DMA1_Stream4_IRQHandler(void)
{  
    if(__HAL_DMA_GET_FLAG(&I2S2_TXDMA_Handler,DMA_FLAG_TCIF0_4)!=RESET) //DMA传输完成
    {
        __HAL_DMA_CLEAR_FLAG(&I2S2_TXDMA_Handler,DMA_FLAG_TCIF0_4);     //清除DMA传输完成中断标志位
        i2s_tx_callback();	//执行回调函数,读取数据等操作在这里面处理  
    } 
} 

//I2S开始播放
void I2S_Play_Start(void)
{   	
	__HAL_DMA_ENABLE(&I2S2_TXDMA_Handler);//开启DMA TX传输  			
}

//关闭I2S播放
void I2S_Play_Stop(void)
{ 
	__HAL_DMA_DISABLE(&I2S2_TXDMA_Handler);//结束播放;		 	 
} 







