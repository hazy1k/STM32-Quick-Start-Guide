#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "timer.h"
/************************************************
 ALIENTEK 探索者STM32F407开发板 实验10
 输入捕获实验-HAL库函数版
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/

extern u8  TIM5CH1_CAPTURE_STA;		//输入捕获状态		    				
extern u32	TIM5CH1_CAPTURE_VAL;	//输入捕获值 

int main(void)
{
	long long temp=0; 
    HAL_Init();                   		//初始化HAL库    
    Stm32_Clock_Init(336,8,2,7);  		//设置时钟,168Mhz
	delay_init(168);               		//初始化延时函数
	uart_init(115200);             		//初始化USART
	LED_Init();							//初始化LED	
	TIM14_PWM_Init(500-1,84-1);    		//84M/84=1M的计数频率，自动重装载为500，那么PWM频率为1M/500=2kHZ
	TIM5_CH1_Cap_Init(0XFFFFFFFF,84-1); //以1MHZ的频率计数
	
    while(1)
    {
        delay_ms(10);
		TIM_SetTIM14Compare1(TIM_GetTIM14Capture1()+1); 
		if(TIM_GetTIM14Capture1()==300)TIM_SetTIM14Compare1(0);        
        if(TIM5CH1_CAPTURE_STA&0X80)        //成功捕获到了一次高电平
		{
			temp=TIM5CH1_CAPTURE_STA&0X3F; 
			temp*=0XFFFFFFFF;		 	    //溢出时间总和
			temp+=TIM5CH1_CAPTURE_VAL;      //得到总的高电平时间
			printf("HIGH:%lld us\r\n",temp);//打印总的高点平时间
			TIM5CH1_CAPTURE_STA=0;          //开启下一次捕获
		}
    }
}

