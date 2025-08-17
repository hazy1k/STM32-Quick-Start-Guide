#include <bsp_init.h>

void bsp_init(void)
{
    /* ϵͳ��ʼ�� */
    HAL_Init();                         // ��ʼ��HAL��
    sys_stm32_clock_init(336, 8, 2, 7); // ����ʱ��,168Mhz 
    usart_init(115200);                 // ���ڳ�ʼ��
    delay_init(168);                    // ��ʱ��ʼ�� 
    /* �����ʼ�� */
    bsp_led_init();                     // ��ʼ��LED 
    bsp_key_init();
    LCD_Init();
    sram_init();
	serial_invoker_init(84);
}
