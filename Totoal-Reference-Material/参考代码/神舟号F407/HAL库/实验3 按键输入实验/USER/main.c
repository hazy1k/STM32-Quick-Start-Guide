#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
/************************************************
 ALIENTEK 探索者STM32F407开发板 实验3
 按键输入实验-HAL库函数版
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/

int main(void)
{
	u8 key;
	
    HAL_Init();                    	//初始化HAL库    
    Stm32_Clock_Init(336,8,2,7);  	//设置时钟,168Mhz
	delay_init(168);               	//初始化延时函数
	LED_Init();						//初始化LED	
    KEY_Init();                     //初始化按键
	
    while(1)
    {
        key=KEY_Scan(0);            //按键扫描
		switch(key)
		{				 
			case WKUP_PRES:			//控制LED0,LED1互斥点亮
				LED1=!LED1;
				LED0=!LED1;
				break;
			case KEY2_PRES:			//控制LED0翻转
				LED0=!LED0;
				break;
			case KEY1_PRES:			//控制LED1翻转	 
				LED1=!LED1;
				break;
			case KEY0_PRES:			//同时控制LED0,LED1翻转 
				LED0=!LED0;
				LED1=!LED1;
				break;
		}
        delay_ms(10);
	}
}

