#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "timer.h"

extern uint8_t TIM5CH1_CapTure_STA;
extern uint32_t TIM5CH1_CapTure_VAL;

int main(void)
{
	long temp = 0;
	HAL_Init();
	Stm32_Clock_Init(336,8,2,7);
	delay_init(168);
	uart_init(115200);
	LED_Init();
	KEY_Init();
	TIM14_PWM_Init(500-1,84-1);
	TIM5_CH1_Cap_Init(0XFFFFFFFF,84-1); //以1MHZ的频率计数
	while(1)
	{
		delay_ms(10);
		TIM_SetTIM14Compare1(TIM_GetTIM14Capture1()+1);
		if(TIM_GetTIM14Capture1() == 300)
		{
			TIM_SetTIM14Compare1(0); // 如果计数值达到300，关闭PWM输出
		}
		if(TIM5CH1_CapTure_STA & 0x80) // 成功捕获
		{
			temp = TIM5CH1_CapTure_STA & 0x3F; // 溢出次数
			temp *= 0xFFFFFFFF; // 溢出时间总和（溢出次数*最大计数值）
			temp += TIM5CH1_CapTure_VAL; // 加上捕获值，得到总高电平时间（溢出时间+捕获值）
			printf("High time: %ld us\r\n", temp);
			TIM5CH1_CapTure_STA = 0; // 清除标志位，准备下次捕获
		}
	}
}
