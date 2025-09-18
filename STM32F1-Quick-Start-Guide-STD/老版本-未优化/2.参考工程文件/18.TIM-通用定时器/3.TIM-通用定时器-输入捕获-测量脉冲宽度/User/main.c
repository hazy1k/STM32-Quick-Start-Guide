#include "stm32f10x.h"
#include "led.h"
#include "usart.h"
#include "GeneralTim.h"  

extern volatile uint32_t Capture_Period;
extern volatile uint32_t Caputure_Value;
extern volatile uint8_t Capture_startflag;
extern volatile uint8_t Capture_endflag;

int main(void)
{
	uint32_t time;
	uint32_t TIM_CNT_CLK = 72000000/(TIMx_Prescaler+1); // 计数器时钟频率1Mhz，预分频系数为71
	LED_Init();
	USART_Config();
	GTim_Init();
	while(1)
	{
		if(Capture_endflag == 1)
		{
			time = Capture_Period*(TIMx_Period+1)+(Caputure_Value+1);
			printf("高电平时间：%d.%d s\r\n", time/TIM_CNT_CLK, time%TIM_CNT_CLK);
			Capture_endflag = 0;
		}
	}
}
