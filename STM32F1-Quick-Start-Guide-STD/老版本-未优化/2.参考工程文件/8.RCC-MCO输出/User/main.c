#include "stm32f10x.h"
#include "clkconfig.h"
#include "mcooutput.h"

int main(void)
{
	// 使用HSE时：SYSCLK = 8M * RCC_PLLMul_x(x=2~16)，最高128M
	// 使用HSI时：SYSCLK = 4M * RCC_PLLMul_x(x=2~16)，最高64M
	HSI_SetSYSCLK(RCC_PLLMul_16); // 这里我们选择HSI作为系统时钟源，SYSCLK = 4M * 16 = 64M
	MCO_GPIO_Init();
	// 设置MCO引脚输出的时钟频率
	// MCO引脚输出的频率可以是HSE、HSI、SYSCLK、PLLCLK等，这里我们选择SYSCLK作为输出源，也就是HSI啦
	RCC_MCOConfig(RCC_MCO_SYSCLK);
	while(1)
	{
	}	
}
