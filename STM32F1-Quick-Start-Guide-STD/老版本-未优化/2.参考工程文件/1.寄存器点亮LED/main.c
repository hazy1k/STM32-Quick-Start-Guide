#include "stm32f10x.h"

int main (void)
{	
	// 开启GPIOB端口时钟
	RCC_APB2ENR |= (1<<3);
	// 清空控制PB0的端口位
	GPIOB_CRL &= ~(0x0F << (4*0));
	// 配置PB0位通用推挽输出
	GPIOB_CRL |= (1<<4*0);
	// 使PB0输出为低电平
	GPIOB_ODR |= (0<<0);
	while(1)
	{
		
	}
}

void SystemInit(void)
{
	// 函数体为空，目的是为了骗过编译器不报错
}
