#include "remote.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK 阿波罗STM32F407开发板
//红外遥控解码驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2017/4/15
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

TIM_HandleTypeDef TIM1_Handler;      //定时器1句柄

//红外遥控初始化
//设置IO以及TIM1_CH1的输入捕获
//TIM1挂载APB2上
void Remote_Init(void)
{  
    TIM_IC_InitTypeDef TIM1_CH1Config;  
    
    TIM1_Handler.Instance=TIM1;                          //通用定时器1
    TIM1_Handler.Init.Prescaler=179;                     //预分频器,1M的计数频率,1us加1.
    TIM1_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //向上计数器
    TIM1_Handler.Init.Period=10000;                      //自动装载值
    TIM1_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_IC_Init(&TIM1_Handler);
    
    //初始化TIM1输入捕获参数
    TIM1_CH1Config.ICPolarity=TIM_ICPOLARITY_RISING;    //上升沿捕获
    TIM1_CH1Config.ICSelection=TIM_ICSELECTION_DIRECTTI;//映射到TI1上
    TIM1_CH1Config.ICPrescaler=TIM_ICPSC_DIV1;          //配置输入分频，不分频
    TIM1_CH1Config.ICFilter=0x03;                       //IC1F=0003 8个定时器时钟周期滤波
    HAL_TIM_IC_ConfigChannel(&TIM1_Handler,&TIM1_CH1Config,TIM_CHANNEL_1);//配置TIM1通道1
    HAL_TIM_IC_Start_IT(&TIM1_Handler,TIM_CHANNEL_1);   //开始捕获TIM1的通道1
    __HAL_TIM_ENABLE_IT(&TIM1_Handler,TIM_IT_UPDATE);   //使能更新中断
}

//定时器1底层驱动，时钟使能，引脚配置
//此函数会被HAL_TIM_IC_Init()调用
//htim:定时器1句柄
void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_TIM1_CLK_ENABLE();            //使能TIM1时钟
    __HAL_RCC_GPIOA_CLK_ENABLE();			//开启GPIOA时钟
	
    GPIO_Initure.Pin=GPIO_PIN_8;            //PA8
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;      //复用推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
    GPIO_Initure.Alternate=GPIO_AF1_TIM1;   //PA8复用为TIM1通道1
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);

    HAL_NVIC_SetPriority(TIM1_CC_IRQn,1,3); //设置中断优先级，抢占优先级1，子优先级3
    HAL_NVIC_EnableIRQ(TIM1_CC_IRQn);       //开启ITM1中断

    HAL_NVIC_SetPriority(TIM1_UP_TIM10_IRQn,1,2); //设置中断优先级，抢占优先级1，子优先级2
    HAL_NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);       //开启ITM1中断   
}

//遥控器接收状态
//[7]:收到了引导码标志
//[6]:得到了一个按键的所有信息
//[5]:保留	
//[4]:标记上升沿是否已经被捕获								   
//[3:0]:溢出计时器
u8 	RmtSta=0;	  	  
u16 Dval;		//下降沿时计数器的值
u32 RmtRec=0;	//红外接收到的数据	   		    
u8  RmtCnt=0;	//按键按下的次数	 

//定时器1更新（溢出）中断
void TIM1_UP_TIM10_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&TIM1_Handler);//定时器共用处理函数
} 

//定时器1输入捕获中断服务程序	 
void TIM1_CC_IRQHandler(void)
{ 	
	HAL_TIM_IRQHandler(&TIM1_Handler);//定时器共用处理函数
}

//定时器更新（溢出）中断回调函数
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
 if(htim->Instance==TIM1){
 		if(RmtSta&0x80)//上次有数据被接收到了
		{	
			RmtSta&=~0X10;						//取消上升沿已经被捕获标记
			if((RmtSta&0X0F)==0X00)RmtSta|=1<<6;//标记已经完成一次按键的键值信息采集
			if((RmtSta&0X0F)<14)RmtSta++;
			else
			{
				RmtSta&=~(1<<7);//清空引导标识
				RmtSta&=0XF0;	//清空计数器	
			}						 	   	
		}	
 
 }
}

//定时器输入捕获中断回调函数
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)//捕获中断发生时执行
{
 if(htim->Instance==TIM1)
{
 	if(RDATA)//上升沿捕获
	{
		TIM_RESET_CAPTUREPOLARITY(&TIM1_Handler,TIM_CHANNEL_1);   //一定要先清除原来的设置！！
        TIM_SET_CAPTUREPOLARITY(&TIM1_Handler,TIM_CHANNEL_1,TIM_ICPOLARITY_FALLING);//CC1P=1 设置为下降沿捕获
        __HAL_TIM_SET_COUNTER(&TIM1_Handler,0);  //清空定时器值   	  
		  	RmtSta|=0X10;					//标记上升沿已经被捕获
	}else //下降沿捕获
	{
        Dval=HAL_TIM_ReadCapturedValue(&TIM1_Handler,TIM_CHANNEL_1);//读取CCR1也可以清CC1IF标志位
        TIM_RESET_CAPTUREPOLARITY(&TIM1_Handler,TIM_CHANNEL_1);   //一定要先清除原来的设置！！
        TIM_SET_CAPTUREPOLARITY(&TIM1_Handler,TIM_CHANNEL_1,TIM_ICPOLARITY_RISING);//配置TIM5通道1上升沿捕获
		if(RmtSta&0X10)					//完成一次高电平捕获 
		{
			if(RmtSta&0X80)//接收到了引导码
			{
						
				if(Dval>300&&Dval<800)			//560为标准值,560us
				{
					RmtRec<<=1;	//左移一位.
					RmtRec|=0;	//接收到0	   
				}else if(Dval>1400&&Dval<1800)	//1680为标准值,1680us
				{
					RmtRec<<=1;	//左移一位.
					RmtRec|=1;	//接收到1
				}else if(Dval>2200&&Dval<2600)	//得到按键键值增加的信息 2500为标准值2.5ms
				{
					RmtCnt++; 		//按键次数增加1次
					RmtSta&=0XF0;	//清空计时器		
				}
				}else if(Dval>4200&&Dval<4700)		//4500为标准值4.5ms
				{
					RmtSta|=1<<7;	//标记成功接收到了引导码
					RmtCnt=0;		//清除按键次数计数器
				}						 
			}
		RmtSta&=~(1<<4);
		}				 		     	    
	}
}

//处理红外键盘
//返回值:
//	 0,没有任何按键按下
//其他,按下的按键键值.
u8 Remote_Scan(void)
{        
	u8 sta=0;       
  u8 t1,t2;  
	if(RmtSta&(1<<6))//得到一个按键的所有信息了
	{ 
	    t1=RmtRec>>24;			//得到地址码
	    t2=(RmtRec>>16)&0xff;	//得到地址反码 
 	    if((t1==(u8)~t2)&&t1==REMOTE_ID)//检验遥控识别码(ID)及地址 
	    { 
	        t1=RmtRec>>8;
	        t2=RmtRec; 	
	        if(t1==(u8)~t2)sta=t1;//键值正确	 
		}   
		if((sta==0)||((RmtSta&0X80)==0))//按键数据错误/遥控已经没有按下了
		{
		 	RmtSta&=~(1<<6);//清除接收到有效按键标识
			RmtCnt=0;		//清除按键次数计数器
		}
	}  
    return sta;
}
