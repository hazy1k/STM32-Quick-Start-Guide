#include "sys.h"
#include "delay.h"  
#include "led.h"  
#include "key.h" 
#include "wdg.h"
//ALIENTEK 探索者STM32F407开发板 实验6
//独立看门狗实验  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司
  
int main(void)
{  
	Stm32_Clock_Init(336,8,2,7);//设置时钟,168Mhz 
	delay_init(168);		//延时初始化   
	LED_Init();		  		//初始化与LED连接的硬件接口 
	KEY_Init();		  		//初始化按键
 	delay_ms(100);			//延时100ms再初始化看门狗,LED0的变化"可见"
	IWDG_Init(4,500);    	//预分频数为64,重载值为500,溢出时间为1s	   
	LED0=0;				 	//点亮LED0
	while(1)
	{ 
		if(KEY_Scan(0)==WKUP_PRES)//如果WK_UP按下,则喂狗
		{
			IWDG_Feed();//喂狗
		}
		delay_ms(10);
	};
}

















