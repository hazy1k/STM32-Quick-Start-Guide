#include "sys.h"
#include "usart.h" 
#include "delay.h" 
//ALIENTEK 探索者STM32F407开发板 实验0
//新建工程实验  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司

int main(void)
{ 
	u8 t=0;
	Stm32_Clock_Init(336,8,2,7);//设置时钟,168Mhz
	delay_init(168);		//初始化延时函数
	uart_init(84,115200);	//串口初始化为115200
	while(1)
	{
		printf("t:%d\r\n",t);
		delay_ms(500);
		t++;
	}
}
