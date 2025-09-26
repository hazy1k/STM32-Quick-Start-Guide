#include "stm32f10x_gpio.h"

/**
* 函数功能：设置引脚为高电平
* 参数说明：GPIOx: 该参数为 GPIO_TypeDef 类型的指针，指向 GPIO 端口的地址
* GPIO_Pin: 选择要设置的 GPIO 端口引脚，可输入宏 GPIO_Pin_0-15，
* 表示 GPIOx 端口的 0-15 号引脚。
*/
void GPIO_SetBits(GPIO_TypeDef *GPIOx,uint16_t GPIO_Pin)
{
	
	/* 设置 GPIOx 端口 BSRR 寄存器的第 GPIO_Pin 位，使其输出高电平 */
  /* 因为 BSRR 寄存器写 0 不影响，宏 GPIO_Pin 只是对应位为 1，其它位均为 0，所以可以直接赋值 */
	
	GPIOx->BSRR |= GPIO_Pin;
}



/**
* 函数功能：设置引脚为低电平
* 参数说明：GPIOx: 该参数为 GPIO_TypeDef 类型的指针，指向 GPIO 端口的地址
* GPIO_Pin: 选择要设置的 GPIO 端口引脚，可输入宏 GPIO_Pin_0-15，
* 表示 GPIOx 端口的 0-15 号引脚。
*/
void GPIO_ResetBits( GPIO_TypeDef *GPIOx,uint16_t GPIO_Pin )
{
	
	/* 设置 GPIOx 端口 BRR 寄存器的第 GPIO_Pin 位, 使其输出低电平 */
  /* 因为 BRR 寄存器写 0 不影响，宏 GPIO_Pin 只是对应位为 1，其它位均为 0，所以可以直接赋值 */
	
	GPIOx->BRR |= GPIO_Pin;
}

