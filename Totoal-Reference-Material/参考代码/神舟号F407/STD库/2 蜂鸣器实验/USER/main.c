#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "beep.h"

//ALIENTEK 探索者STM32F407开发板 实验2
//蜂鸣器实验-库函数版本
//技术支持：www.openedv.com
//淘宝店铺：http://eboard.taobao.com
//广州市星翼电子科技有限公司    
//作者：正点原子 @ALIENTEK

int main(void)
{ 
 
	delay_init(168);		   //初始化延时函数
	LED_Init();				    //初始化LED端口
	BEEP_Init();         	//初始化蜂鸣器端口
	
	while(1)
	{ 
		GPIO_ResetBits(GPIOF,GPIO_Pin_9); // DS0拉低，亮   等同LED0=0;
		GPIO_ResetBits(GPIOF,GPIO_Pin_8); //BEEP引脚拉低， 等同BEEP=0;
		delay_ms(300);                    //延时300ms
		GPIO_SetBits(GPIOF,GPIO_Pin_9);   // DS0拉高，灭   等同LED0=1;
		GPIO_SetBits(GPIOF,GPIO_Pin_8);   //BEEP引脚拉高， 等同BEEP=1;
		delay_ms(300); 										//延时300ms
	}
}
