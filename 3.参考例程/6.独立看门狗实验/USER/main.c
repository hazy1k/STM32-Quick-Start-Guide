#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "iwdg.h"

int main(void)
{
    HAL_Init();                    	// ��ʼ��HAL��    
    Stm32_Clock_Init(336,8,2,7);  	// ����ʱ��,168Mhz
	delay_init(168);               	// ��ʼ����ʱ����
	uart_init(115200);              // ��ʼ��USART
	LED_Init();						// ��ʼ��LED	
    KEY_Init();                     // ��ʼ������
	delay_ms(100);                  // ��ʱ100ms�ٳ�ʼ�����Ź�,LED0�ı仯"�ɼ�"
    IWDG_Init(IWDG_PRESCALER_64, 500);  // ���Ź���ʼ��,Ԥ��Ƶ��Ϊ64,����ֵΪ500,���ʱ��Ϊ1s	
    LED0 = 0; // LED0 Ϩ��
	
    while(1)
    {	
        if(KEY_Scan(0) == WKUP_PRES) // ���WK_UP���£�ι��
        {
            IWDG_Feed();    	     // ι��
        }
        delay_ms(10); 
    }
}