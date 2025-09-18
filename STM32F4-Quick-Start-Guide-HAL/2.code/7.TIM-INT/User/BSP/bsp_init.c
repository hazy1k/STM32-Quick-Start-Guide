#include <bsp_init.h>

void bsp_init(void)
{
    /* 系统初始化 */
    HAL_Init();                         // 初始化HAL库
    sys_stm32_clock_init(336, 8, 2, 7); // 设置时钟,168Mhz 
    usart_init(115200);                  // 串口初始化
    delay_init(168);                    // 延时初始化 
    /* 外设初始化 */
    bsp_led_init();                     // 初始化LED 
    /*
        时器初始化，设置重装值(arr)为5000-1，分频系数(psc)为8400-1
	    这个工程中我们的定时器的工作频率为84MHz/8400=10KHz
	    周期计算Time = ((arr+1)*(psc+1))/fclk = 5000*(8400)/84000000 = 500ms
    */
    BSP_TIM_Init(5000-1, 8400-1);
}
