#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "timer.h"
/************************************************
 ALIENTEK 探索者STM32F407开发板 实验8
 定时器中断实验-HAL库函数版 
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/

int main(void)
{
    HAL_Init();                   	//初始化HAL库    
    Stm32_Clock_Init(336,8,2,7);  	//设置时钟,168Mhz
	delay_init(168);               	//初始化延时函数
	uart_init(115200);             	//初始化USART
	LED_Init();						//初始化LED	
    KEY_Init();                    	//初始化按键
    TIM3_Init(5000-1,8400-1);       //定时器3初始化，定时器时钟为84M，分频系数为8400-1，
                                    //所以定时器3的频率为84M/8400=10K，自动重装载为5000-1，那么定时器周期就是500ms
    while(1)
    {
        LED0=!LED0;                 //LED0翻转
        delay_ms(200);              //延时200ms
    }
}

