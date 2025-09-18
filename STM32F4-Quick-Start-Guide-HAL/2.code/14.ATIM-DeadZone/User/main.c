#include <bsp_init.h>
#include <atim.h>

int main(void) 
{
    bsp_init();
    printf("\r\n===== PWM Complementary Output Demo =====\r\n");
    // PWM初始化: 1kHz波形
    ATIM_PWM_Init(1000-1, 168-1); 
    // 设置死区时间: 占空比30%，死区时间100
    ATIM_PWM_SetDeadZone(300, 100);
    printf("\r\nSystem running...\r\n");
    while(1) {
        LED_TOGGLE(LED0_GPIO_Pin);
        printf("[SYS] Heartbeat\r\n");
        delay_ms(1000);
    }
}
