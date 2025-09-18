#include "stm32f10x.h"
#include "rcc.h"

// PA8复用为MCO输出
void MCO_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

// 程序来到main函数之前，启动文件：statup_stm32f10x_hd.s已经调用SystemInit()函数把系统时钟初始化成72MHZ
// SystemInit()在system_stm32f10x.c中定义
// 如果用户想修改系统时钟，可自行编写程序修改
// 重新设置系统时钟，根据需要修改，一般设置最高为128MHz
// SYSCLK = 8M * RCC_PLLMul_x, x:[2,3,...16]
int main()
{
	// HSE_Clock_Init(RCC_PLLMul_9); // 8M * 9 = 72M 
	// 如果HSI要作为PLL时钟的来源的话，必须二分频之后才可以，即HSI/2，而PLL倍频因子最大只能是16
	HSI_Clock_Init(RCC_PLLMul_16); // 4M * 16 = 64M

	// 设置MCO引脚输出时钟，用示波器即可在PA8测量到输出的时钟信号
	// MCO引脚输出可以是HSE,HSI,PLLCLK/2,SYSCLK
	RCC_MCOConfig(RCC_MCO_SYSCLK); // 本工程设置为64M
}
