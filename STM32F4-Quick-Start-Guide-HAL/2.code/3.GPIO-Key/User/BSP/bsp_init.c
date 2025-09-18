#include "bsp_init.h"

void bsp_init(void)
{
    /* 系统初始化 */
    HAL_Init();                         // 初始化HAL库
    sys_stm32_clock_init(336, 8, 2, 7); // 设置时钟,168Mhz 
    delay_init(168);                    // 延时初始化 
    /* 外设初始化 */
    bsp_led_init();                     // 初始化LED 
    bsp_beep_init();                    // 初始化蜂鸣器
    bsp_key_init();                     // 初始化按键   
}
