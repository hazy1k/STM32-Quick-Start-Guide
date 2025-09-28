#include "sys.h"
#include "delay.h" 
#include "led.h"
#include "beep.h"
#include "key.h"
//ALIENTEK 探索者STM32F407开发板 实验3
//按键输入实验  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司
 
int main(void)
{ 
	u8 key;
	Stm32_Clock_Init(336,8,2,7);//设置时钟,168Mhz 
	delay_init(168);		//延时初始化 
	LED_Init();		  		//初始化与LED连接的硬件接口 
	BEEP_Init();		 	//初始化蜂鸣器IO
	KEY_Init();         	//初始化与按键连接的硬件接口
	LED0=0;					//先点亮红灯
	while(1)
	{
 		key=KEY_Scan(0); 	//得到键值
	   	if(key)
		{						   
			switch(key)
			{				 
				case WKUP_PRES:	//控制蜂鸣器
					BEEP=!BEEP;
					break;
				case KEY2_PRES:	//控制LED0翻转
					LED0=!LED0;
					break;
				case KEY1_PRES:	//控制LED1翻转	 
					LED1=!LED1;
					break;
				case KEY0_PRES:	//同时控制LED0,LED1翻转 
					LED0=!LED0;
					LED1=!LED1;
					break;
			}
		}else delay_ms(10); 
	}
}

















