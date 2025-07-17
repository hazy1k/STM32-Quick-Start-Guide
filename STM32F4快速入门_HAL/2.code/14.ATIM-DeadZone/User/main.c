#include <bsp_init.h>
#include <atim.h>

int main(void) 
{
    bsp_init();
    printf("\r\n===== PWM Complementary Output Demo =====\r\n");
    // PWM��ʼ��: 1kHz����
    ATIM_PWM_Init(1000-1, 168-1); 
    // ��������ʱ��: ռ�ձ�30%������ʱ��100
    ATIM_PWM_SetDeadZone(300, 100);
    printf("\r\nSystem running...\r\n");
    while(1) {
        LED_TOGGLE(LED0_GPIO_Pin);
        printf("[SYS] Heartbeat\r\n");
        delay_ms(1000);
    }
}
