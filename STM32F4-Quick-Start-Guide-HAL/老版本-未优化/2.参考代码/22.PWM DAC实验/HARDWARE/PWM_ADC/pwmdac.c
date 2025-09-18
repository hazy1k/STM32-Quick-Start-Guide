#include "pwmdac.h"

TIM_HandleTypeDef TIM9_Handler;         //定时器9 PWM句柄 
TIM_OC_InitTypeDef TIM9_CH2Handler;	    //定时器9通道2句柄

// PWM DAC初始化(也就是TIM9通道2初始化)
// PWM输出初始化
// arr：自动重装值
// psc：时钟预分频数
void TIM9_CH2_PWM_Init(u16 arr,u16 psc)
{ 
    TIM9_Handler.Instance = TIM9;                      // 定时器9
    TIM9_Handler.Init.Prescaler = psc;                 // 定时器分频
    TIM9_Handler.Init.CounterMode = TIM_COUNTERMODE_UP;// 向上计数模式
    TIM9_Handler.Init.Period = arr;                    // 自动重装载值
    TIM9_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1; // 时钟分频因子
    HAL_TIM_PWM_Init(&TIM9_Handler);                   // 初始化PWM
    TIM9_CH2Handler.OCMode = TIM_OCMODE_PWM1; // 模式选择PWM1
    TIM9_CH2Handler.Pulse = arr/2;            // 设置比较值,此值用来确定占空比，默认比较值为自动重装载值的一半,即占空比为50%
    TIM9_CH2Handler.OCPolarity = TIM_OCPOLARITY_HIGH; // 输出比较极性为高
    HAL_TIM_PWM_ConfigChannel(&TIM9_Handler,&TIM9_CH2Handler,TIM_CHANNEL_2); // 配置TIM9通道2
    HAL_TIM_PWM_Start(&TIM9_Handler,TIM_CHANNEL_2);//开启PWM通道2
}

//定时器底层驱动，时钟使能，引脚配置
//此函数会被HAL_TIM_PWM_Init()调用
//htim:定时器句柄
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
    GPIO_InitTypeDef GPIO_Initure;
	__HAL_RCC_TIM9_CLK_ENABLE();	      // 使能定时器9
	__HAL_RCC_GPIOA_CLK_ENABLE();	      // 开启GPIOA时钟
    GPIO_Initure.Pin=GPIO_PIN_3;          // PA3
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;    // 复用推完输出
    GPIO_Initure.Pull=GPIO_PULLUP;        //上拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;   // 高速
	GPIO_Initure.Alternate= GPIO_AF3_TIM9;// PA3复用为TIM9_CH2
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
}

// 设置TIM通道2的占空比
// TIM_TypeDef:定时器
// compare:比较值
void TIM_SetTIM9Compare2(u32 compare)
{
	TIM9->CCR2 = compare;
}
