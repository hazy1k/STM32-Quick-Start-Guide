#include "sys.h"
#include "delay.h"  
#include "usart.h"  
#include "led.h"
#include "timer.h"
//ALIENTEK 探索者STM32F407开发板 实验10
//输入捕获实验  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司

extern u8  TIM5CH1_CAPTURE_STA;		//输入捕获状态		    				
extern u32	TIM5CH1_CAPTURE_VAL;	//输入捕获值  
int main(void)
{  
	long long temp=0;  
	Stm32_Clock_Init(336,8,2,7);//设置时钟,168Mhz 
	delay_init(168);			//延时初始化   
	uart_init(84,115200);		//初始化串口波特率为115200
 	TIM14_PWM_Init(500-1,84-1);	//1Mhz的计数频率,2Khz的PWM.     
 	TIM5_CH1_Cap_Init(0XFFFFFFFF,84-1);//以1Mhz的频率计数 
   	while(1)
	{
 		delay_ms(10);
		LED0_PWM_VAL++;
		if(LED0_PWM_VAL==300)LED0_PWM_VAL=0;	 		 
 		if(TIM5CH1_CAPTURE_STA&0X80)//成功捕获到了一次高电平
		{
			temp=TIM5CH1_CAPTURE_STA&0X3F; 
			temp*=0XFFFFFFFF;		 		//溢出时间总和
			temp+=TIM5CH1_CAPTURE_VAL;		//得到总的高电平时间
			printf("HIGH:%lld us\r\n",temp);//打印总的高点平时间
			TIM5CH1_CAPTURE_STA=0;			//开启下一次捕获
		}
	}
}

















