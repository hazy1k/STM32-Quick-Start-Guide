#include <bsp_init.h>

void bsp_init(void)
{
    /* 系统初始化 */
    HAL_Init();                         // 初始化HAL库
    sys_stm32_clock_init(336, 8, 2, 7); // 设置时钟,168Mhz 
    usart_init(115200);                 // 串口初始化
    delay_init(168);                    // 延时初始化 
    /* 外设初始化 */
    bsp_led_init();                     // 初始化LED 
    bsp_key_init();
    LCD_Init();
    sram_init();
	serial_invoker_init(84);
}
