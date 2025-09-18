#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"

int main(void)
{ 
    HAL_Init();                 // 初始化HAL库,来源于"stm32f4xx_hal.c" 
    // 设置时钟,168Mhz,来源于"sys.c" 函数参数：plln，pllm，pllp，pllq
    // 部晶振为8M的时候,推荐值:plln=336,pllm=8,pllp=2,pllq=7.
    // 得到:Fvco=8*(336/8)=336Mhz
    //     SYSCLK=336/2=168Mhz
    //     Fusb=336/7=48Mhz
    Stm32_Clock_Init(336,8,2,7); // 系统时钟初始化
	delay_init(168);             // 初始化延时函数,168Mhz系统时钟
	LED_Init();					 // 初始化LED	
	while(1)
	{
        // 亮LED0，灭LED1
        HAL_GPIO_WritePin(LED0_GPIO, LED0_PIN, GPIO_PIN_RESET);  // 点亮LED0
        HAL_GPIO_WritePin(LED1_GPIO, LED1_PIN, GPIO_PIN_SET);    // 熄灭LED1
        delay_ms(500);  // 延时500ms
        // 灭LED0，亮LED1
        HAL_GPIO_WritePin(LED0_GPIO, LED0_PIN, GPIO_PIN_SET);    // 熄灭LED0
        HAL_GPIO_WritePin(LED1_GPIO, LED1_PIN, GPIO_PIN_RESET);  // 点亮LED1
        delay_ms(500);  // 延时500ms             
	}
}
