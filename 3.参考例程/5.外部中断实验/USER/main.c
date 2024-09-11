#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "exti.h"

int main(void)
{
    HAL_Init();                  // ��ʼ��HAL��    
    Stm32_Clock_Init(336,8,2,7); // ����ʱ��,168Mhz
	delay_init(168);             // ��ʼ����ʱ����
	uart_init(115200);           // ��ʼ��USART
	LED_Init();				     // ��ʼ��LED	
    KEY_Init();                  // ��ʼ������
	EXTI_Init();                 // �ⲿ�жϳ�ʼ��
	
    while(1)
    {	
		printf("OK\r\n"); // ��ӡOK��ʾ��������
        delay_ms(1000);   // ÿ��1s��ӡһ�� 
    }
}