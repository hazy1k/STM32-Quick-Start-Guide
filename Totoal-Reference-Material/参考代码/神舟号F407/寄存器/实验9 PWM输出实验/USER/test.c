#include "sys.h"
#include "delay.h"  
#include "led.h"
#include "timer.h"
//ALIENTEK 探索者STM32F407开发板 实验9
//PWM输出实验  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司
  
int main(void)
{  
	u16 led0pwmval=0;    
	u8 dir=1;
	Stm32_Clock_Init(336,8,2,7);//设置时钟,168Mhz 
	delay_init(168);			//延时初始化   
 	TIM14_PWM_Init(500-1,84-1);	//1Mhz的计数频率,2Khz的PWM.     
   	while(1)
	{
 		delay_ms(10);	 
		if(dir)led0pwmval++;
		else led0pwmval--;	 
 		if(led0pwmval>300)dir=0;
		if(led0pwmval==0)dir=1;	   					 
		LED0_PWM_VAL=led0pwmval;	   
	}
}

















