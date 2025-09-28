#include "sys.h"
#include "delay.h" 
#include "usart.h" 
#include "led.h"
//ALIENTEK 探索者STM32F407开发板 实验4
//串口通信实验  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司
  
int main(void)
{ 
	u8 t;
	u8 len;	
	u16 times=0;   
	Stm32_Clock_Init(336,8,2,7);//设置时钟,168Mhz 
	delay_init(168);		//延时初始化 
	uart_init(84,115200);	//串口初始化为115200
	LED_Init();		  		//初始化与LED连接的硬件接口   
	while(1)
	{
		if(USART_RX_STA&0x8000)
		{					   
			len=USART_RX_STA&0x3fff;//得到此次接收到的数据长度
			printf("\r\n您发送的消息为:\r\n");
			for(t=0;t<len;t++)
			{
				USART1->DR=USART_RX_BUF[t];
				while((USART1->SR&0X40)==0);//等待发送结束
			}
			printf("\r\n\r\n");//插入换行
			USART_RX_STA=0;
		}else
		{
			times++;
			if(times%5000==0)
			{
				printf("\r\nALIENTEK 探索者STM32F407开发板 串口实验\r\n");
				printf("正点原子@ALIENTEK\r\n\r\n\r\n");
			}
			if(times%200==0)printf("请输入数据,以回车键结束\r\n");  
			if(times%30==0)LED0=!LED0;//闪烁LED,提示系统正在运行.
			delay_ms(10);   
		}
	}
}

















