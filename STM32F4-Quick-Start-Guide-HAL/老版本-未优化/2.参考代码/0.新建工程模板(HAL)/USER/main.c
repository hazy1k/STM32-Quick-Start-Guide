#include "sys.h"
#include "delay.h"
#include "usart.h"

void Delay(__IO uint32_t nCount);

void Delay(__IO uint32_t nCount)
{
  while(nCount--){}
}

int main(void)
{

	GPIO_InitTypeDef GPIO_Initure;
     
	HAL_Init();                    // 初始化HAL库    
	Stm32_Clock_Init(336,8,2,7);   // 设置时钟,168Mhz

    __HAL_RCC_GPIOF_CLK_ENABLE(); // 开启GPIOF时钟
	
    GPIO_Initure.Pin = GPIO_PIN_9|GPIO_PIN_10; 	//PF9,10
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;  	//推挽输出
    GPIO_Initure.Pull = GPIO_PULLUP;          	//上拉
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;    	 	//高速
    HAL_GPIO_Init(GPIOF, &GPIO_Initure);

	while(1)
	{
		HAL_GPIO_WritePin(GPIOF,GPIO_PIN_9,GPIO_PIN_SET); // PF9置1 
		HAL_GPIO_WritePin(GPIOF,GPIO_PIN_10,GPIO_PIN_SET);// PF10置1  			
		Delay(0x7FFFFF);
		HAL_GPIO_WritePin(GPIOF,GPIO_PIN_9,GPIO_PIN_RESET);	// PF9置0
		HAL_GPIO_WritePin(GPIOF,GPIO_PIN_10,GPIO_PIN_RESET);// PF10置0  
		Delay(0x7FFFFF);
	}
}