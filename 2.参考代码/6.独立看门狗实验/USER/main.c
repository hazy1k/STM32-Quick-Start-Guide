#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "iwdg.h"

int main(void)
{
	HAL_Init();
	Stm32_Clock_Init(336,8,2,7);
	delay_init(168);
	uart_init(115200);
	LED_Init();
	KEY_Init();
	delay_ms(100);
	// ��ʼ��IWGD��Ԥ��Ƶ64������ֵ512
	// ���㹫ʽ��Time = (64 * 512) / LSI
	IWDG_Init(IWDG_PRESCALER_64, 512); // ���ó�ʱʱ��Ϊ1S 
	LED0_ON();
	while(1)
	{
		if(KEY_Scan(0) == KEY_UP_PRES) // ������������£�����ι��
		{
			IWDG_Feed(); // ι��
		}
		delay_ms(10); // ��ʱ10ms
	}
}
