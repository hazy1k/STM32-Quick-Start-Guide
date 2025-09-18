#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "usmart.h"
#include "dma.h"

#define SEND_BUF_SIZE 8200	//发送数据长度,最好等于sizeof(TEXT_TO_SEND)+2的整数倍.

u8 SendBuff[SEND_BUF_SIZE]; //发送数据缓冲区
const u8 TEXT_TO_SEND[]={"ALIENTEK Explorer STM32F4 DMA 串口实验"}; 

int main(void)
{
	u16 i;
	u8 t=0;
	u8 j,mask=0;
	float pro=0; 
	
    HAL_Init();                 // 初始化HAL库    
    Stm32_Clock_Init(336,8,2,7);// 设置时钟,168Mhz
	delay_init(168);            // 初始化延时函数
	uart_init(115200);          // 初始化USART
	usmart_dev.init(84); 		// 初始化USMART
	LED_Init();					// 初始化LED	
	KEY_Init();				    // 初始化KEY
 	LCD_Init();           		// 初始化LCD
    MYDMA_Config(DMA2_Stream7,DMA_CHANNEL_4); // 初始化DMA，使用DMA2_Stream7通道4
    POINT_COLOR=RED; 
	LCD_ShowString(30,50,200,16,16,"Explorer STM32F4");	
	LCD_ShowString(30,70,200,16,16,"DMA TEST");	 
	LCD_ShowString(30,130,200,16,16,"KEY0:Start"); 
	POINT_COLOR=BLUE;//设置字体为蓝色 
    //显示提示信息	
	j=sizeof(TEXT_TO_SEND);	   
	for(i=0;i<SEND_BUF_SIZE;i++)  // 填充ASCII字符集数据
    {
		if(t>=j)//加入换行符
		{
			if(mask)
			{
				SendBuff[i]=0x0a;
				t=0;
			}else 
			{
				SendBuff[i]=0x0d;
				mask++;
			}	
		}else//复制TEXT_TO_SEND语句
		{
			mask=0;
			SendBuff[i]=TEXT_TO_SEND[t];
			t++;
		}   	   
    }   
	POINT_COLOR=BLUE;//设置字体为蓝色	  
	i=0;
       while(1)
	{
        t=KEY_Scan(0);
		if(t==KEY0_PRES)  //KEY0按下
		{
			printf("\r\nDMA DATA:\r\n"); 	    
			LCD_ShowString(30,150,200,16,16,"Start Transimit....");
			LCD_ShowString(30,170,200,16,16,"   %") ;     //显示百分号      
			HAL_UART_Transmit_DMA(&UART1_Handler,SendBuff,SEND_BUF_SIZE);//启动传输
			//使能串口1的DMA发送 //等待DMA传输完成，此时我们来做另外一些事，点灯
		    //实际应用中，传输数据期间，可以执行另外的任务
		    while(1)
		    {
                if(__HAL_DMA_GET_FLAG(&UART1TxDMA_Handler,DMA_FLAG_TCIF3_7))//等待DMA2_Steam7传输完成
                {
                    __HAL_DMA_CLEAR_FLAG(&UART1TxDMA_Handler,DMA_FLAG_TCIF3_7);//清除DMA2_Steam7传输完成标志
                    HAL_UART_DMAStop(&UART1_Handler);      //传输完成以后关闭串口DMA
					break; 
                }
				pro=__HAL_DMA_GET_COUNTER(&UART1TxDMA_Handler);//得到当前还剩余多少个数据
				pro=1-pro/SEND_BUF_SIZE;    //得到百分比	  
				pro*=100;      			    //扩大100倍
				LCD_ShowNum(30,170,pro,3,16);	    
		    }
			LCD_ShowNum(30,170,100,3,16);//显示100%	  
		    LCD_ShowString(30,150,200,16,16,"Transimit Finished!");//提示传送完成
		}
		i++;
		delay_ms(10);
		if(i==20)
		{
			LED0=!LED0;//提示系统正在运行	
			i=0;
		}		   
	}
}
