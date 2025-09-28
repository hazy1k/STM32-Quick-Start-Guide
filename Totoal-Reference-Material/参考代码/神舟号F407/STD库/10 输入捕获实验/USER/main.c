#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "timer.h"


//ALIENTEK 探索者STM32F407开发板 实验10
//输入捕获实验 -库函数版本
//技术支持：www.openedv.com
//淘宝店铺：http://eboard.taobao.com  
//广州市星翼电子科技有限公司  
//作者：正点原子 @ALIENTEK
 
 
 
extern u8  TIM5CH1_CAPTURE_STA;		//输入捕获状态		    				
extern u32	TIM5CH1_CAPTURE_VAL;	//输入捕获值  
  
	
int main(void)
{ 
	long long temp=0;  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);  //初始化延时函数
	uart_init(115200);//初始化串口波特率为115200
	
 	TIM14_PWM_Init(500-1,84-1);       	//84M/84=1Mhz的计数频率计数到500,PWM频率为1M/500=2Khz     
 	TIM5_CH1_Cap_Init(0XFFFFFFFF,84-1); //以1Mhz的频率计数 
   	while(1)
	{
 		delay_ms(10);
		TIM_SetCompare1(TIM14,TIM_GetCapture1(TIM14)+1); 
		if(TIM_GetCapture1(TIM14)==300)TIM_SetCompare1(TIM14,0);			 
 		if(TIM5CH1_CAPTURE_STA&0X80)        //成功捕获到了一次高电平
		{
			temp=TIM5CH1_CAPTURE_STA&0X3F; 
			temp*=0XFFFFFFFF;		 		         //溢出时间总和
			temp+=TIM5CH1_CAPTURE_VAL;		   //得到总的高电平时间
			printf("HIGH:%lld us\r\n",temp); //打印总的高点平时间
			TIM5CH1_CAPTURE_STA=0;			     //开启下一次捕获
		}
	}
}
