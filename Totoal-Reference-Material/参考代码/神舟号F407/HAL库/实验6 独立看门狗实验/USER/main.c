#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "iwdg.h"
/************************************************
 ALIENTEK 探索者STM32F407开发板 实验6
 独立看门狗实验-HAL库函数版
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/

int main(void)
{
    HAL_Init();                    		//初始化HAL库    
    Stm32_Clock_Init(336,8,2,7);  		//设置时钟,168Mhz
	delay_init(168);               		//初始化延时函数
	uart_init(115200);              	//初始化USART
	LED_Init();							//初始化LED	
    KEY_Init();                     	//初始化按键
	delay_ms(100);                  	//延时100ms再初始化看门狗,LED0的变化"可见"
    IWDG_Init(IWDG_PRESCALER_64,500);  	//分频数为64,重载值为500,溢出时间为1s	
    LED0=0;
	
    while(1)
    {	
        if(KEY_Scan(0)==WKUP_PRES)  	//如果WK_UP按下，喂狗
        {
            IWDG_Feed();    			//喂狗
        }
        delay_ms(10); 
    }
}

