#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "timer.h"
/************************************************
 ALIENTEK 探索者STM32F407开发板 实验9
 PWM输出实验-HAL库函数版 
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/

int main(void)
{
	u8 dir=1;
    u16 led0pwmval=0; 
	
    HAL_Init();                   	//初始化HAL库    
    Stm32_Clock_Init(336,8,2,7);  	//设置时钟,168Mhz
	delay_init(168);               	//初始化延时函数
	uart_init(115200);             	//初始化USART
	LED_Init();						//初始化LED	
	TIM3_Init(5000-1,8400-1);       //定时器3初始化，周期为500ms
	TIM14_PWM_Init(500-1,84-1);    	//84M/84=1M的计数频率，自动重装载为500，那么PWM频率为1M/500=2kHZ
	
    while(1)
    {
		delay_ms(10);	 	
		if(dir)led0pwmval++;				//dir==1 led0pwmval递增
		else led0pwmval--;					//dir==0 led0pwmval递减 
		if(led0pwmval>300)dir=0;			//led0pwmval到达300后，方向为递减
		if(led0pwmval==0)dir=1;				//led0pwmval递减到0后，方向改为递增
		TIM_SetTIM14Compare1(led0pwmval);	//修改比较值，修改占空比
    }
}

