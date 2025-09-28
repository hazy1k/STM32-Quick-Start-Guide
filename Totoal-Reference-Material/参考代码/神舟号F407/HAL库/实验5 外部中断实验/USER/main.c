#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "exti.h"
/************************************************
 ALIENTEK 探索者STM32F407开发板 实验5
 外部中断-HAL库函数版
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/

int main(void)
{
    HAL_Init();                    	//初始化HAL库    
    Stm32_Clock_Init(336,8,2,7);  	//设置时钟,168Mhz
	delay_init(168);               	//初始化延时函数
	uart_init(115200);              //初始化USART
	LED_Init();						//初始化LED	
    KEY_Init();                     //初始化按键
	EXTI_Init();                    //外部中断初始化
	
    while(1)
    {	
		printf("OK\r\n");           //打印OK提示程序运行
        delay_ms(1000);             //每隔1s打印一次 
    }
}

