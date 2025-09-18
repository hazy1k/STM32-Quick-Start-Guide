#include "timer.h"
#include "led.h"

// 前置知识
/*
    arr:自动重装载值，计数器到达该值时，计数器自动重装载，计数器重新计数。
    psc:预分频值，用来设置时钟源的频率，以此来控制计数器的频率。
    定时器溢出时间计算公式：
    Time= ((arr+1)*(psc+1))/fclk us ,这里的fclk是定时器工作频率，单位为Hz。
    我们这个工程使用的是定时器3，定时器挂在APB1上，时钟位HCLK/2
*/
TIM_HandleTypeDef TIM14_Handler; // 定时器TIM14句柄
TIM_OC_InitTypeDef TIM14_CH1Handler; // 定时器TIM14通道1句柄

// TIM14 PWM初始化 
void TIM14_PWM_Init(u16 arr, u16 psc)
{  
    TIM14_Handler.Instance = TIM14;          	   
    TIM14_Handler.Init.Prescaler = psc;            
    TIM14_Handler.Init.CounterMode = TIM_COUNTERMODE_UP;
    TIM14_Handler.Init.Period = arr;               
    TIM14_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_PWM_Init(&TIM14_Handler); 
    TIM14_CH1Handler.OCMode=TIM_OCMODE_PWM1; // 模式选择PWM1
    TIM14_CH1Handler.Pulse = arr/2;          // 设置比较值,此值用来确定占空比，默认比较值为自动重装载值的一半,即占空比为50%
    TIM14_CH1Handler.OCPolarity = TIM_OCPOLARITY_LOW; // 输出比较极性为低 
    HAL_TIM_PWM_ConfigChannel(&TIM14_Handler,&TIM14_CH1Handler,TIM_CHANNEL_1); // 配置TIM14通道1
    HAL_TIM_PWM_Start(&TIM14_Handler,TIM_CHANNEL_1); // 开启PWM通道1
}

// 定时器底层驱动，时钟使能，引脚配置
// 此函数会被HAL_TIM_PWM_Init()调用
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
	GPIO_InitTypeDef GPIO_Initure;
	__HAL_RCC_TIM14_CLK_ENABLE();			
	__HAL_RCC_GPIOF_CLK_ENABLE();			
	GPIO_Initure.Pin = GPIO_PIN_9;          
	GPIO_Initure.Mode = GPIO_MODE_AF_PP;  	
	GPIO_Initure.Pull = GPIO_PULLUP;        
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;   
	GPIO_Initure.Alternate = GPIO_AF9_TIM14; // PF9复用为TIM14_CH1
	HAL_GPIO_Init(GPIOF,&GPIO_Initure);
}

// 设置TIM通道4的占空比
void TIM_SetTIM14Compare1(u32 compare)
{
	TIM14->CCR1 = compare; // 设置占空比 
}
