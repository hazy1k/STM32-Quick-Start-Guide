#include "general_tim.h"

// 初始化GPIO，配置与定时器相关的引脚
static void TIM_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  // 使能与定时器相关的GPIO时钟
  RCC_APB2PeriphClockCmd(GTIM_MASTER_GPIO_CLK | GTIM_SLAVE1_GPIO_CLK | GTIM_SLAVE2_GPIO_CLK, ENABLE);
  // 使能AFIO时钟，用于处理引脚复用
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
  // 配置TIM2的引脚重映射，将TIM2_CH3映射到PB10引脚
  GPIO_PinRemapConfig(GPIO_FullRemap_TIM2, ENABLE); // 重映射TIM2引脚 TIM2_CH3 -> PB10
  // 配置TIM3的引脚重映射，将TIM3_CH1映射到PC6引脚
  GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE); // 重映射TIM3引脚 TIM3_CH1 -> PC6
  // 配置从定时器1（TIM3）的GPIO引脚，PC6
  GPIO_InitStructure.GPIO_Pin = TIM_SLAVE1_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 复用推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // 设置引脚速度
  GPIO_Init(TIM_SLAVE1_PORT, &GPIO_InitStructure); // 初始化TIM3_CH1（PC6）
  // 配置从定时器2（TIM4）的GPIO引脚，PB6
  GPIO_InitStructure.GPIO_Pin = TIM_SLAVE2_PIN;
  GPIO_Init(TIM_SLAVE2_PORT, &GPIO_InitStructure); // 初始化TIM4_CH2（PB6）
  // 配置主定时器（TIM2）的GPIO引脚，PB10
  GPIO_InitStructure.GPIO_Pin = TIM_MASTER_PIN;
  GPIO_Init(TIM_MASTER_PORT, &GPIO_InitStructure); // 初始化TIM2_CH3（PB10）
}

static void TIM_Mode_Init(void)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  // 使能主定时器和从定时器的时钟
  RCC_APB1PeriphClockCmd(TIM_MASTER_CLK | TIM_SLAVE1_CLK | TIM_SLAVE2_CLK, ENABLE);
  // 配置主定时器时基参数
  TIM_TimeBaseStructure.TIM_Period = 72; // 设置定时器的自动重载值（周期为72）
  TIM_TimeBaseStructure.TIM_Prescaler = 0; // 不使用预分频器
  TIM_TimeBaseStructure.TIM_ClockDivision = 0; // 时钟分频系数为0
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // 计数向上计数
  TIM_TimeBaseInit(TIM_MASTER, &TIM_TimeBaseStructure); // 初始化主定时器
  // 配置从定时器1时基参数（周期设置为4）
  TIM_TimeBaseStructure.TIM_Period = 4; // 设置周期为4
  TIM_TimeBaseInit(TIM_SLAVE1, &TIM_TimeBaseStructure); // 初始化从定时器1
  // 配置从定时器2时基参数（周期设置为4）
  TIM_TimeBaseStructure.TIM_Period = 4; // 设置周期为4
  TIM_TimeBaseInit(TIM_SLAVE2, &TIM_TimeBaseStructure); // 初始化从定时器2
  // 配置主定时器为PWM模式
  TIM_OCInitTypeDef TIM_OCInitStructure;
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; // 设置为PWM模式1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // 使能输出
  TIM_OCInitStructure.TIM_Pulse = 5; // 设置PWM占空比为5
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; // 输出极性为高
  TIM_OC3Init(TIM_MASTER, &TIM_OCInitStructure); // 配置主定时器的PWM输出通道3
  // 配置主定时器为主从模式
  TIM_SelectMasterSlaveMode(TIM_MASTER, TIM_MasterSlaveMode_Enable); // 使能主从模式
  TIM_SelectInputTrigger(TIM_MASTER, TIM_TRGOSource_Update); // 设置输出触发源为更新事件
  // 配置从定时器1为PWM模式
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; // 设置为PWM模式1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // 使能输出
  TIM_OCInitStructure.TIM_Pulse = 4; // 设置PWM占空比为4
  TIM_OC1Init(TIM_SLAVE1, &TIM_OCInitStructure); // 配置从定时器1的PWM输出通道1
  // 配置从定时器1为主从模式
  TIM_SelectMasterSlaveMode(TIM_SLAVE1, TIM_MasterSlaveMode_Enable);
  // 配置从定时器1为门控同步模式
  TIM_SelectSlaveMode(TIM_SLAVE1, TIM_SlaveMode_Gated); // 设置为门控同步模式
  TIM_SelectInputTrigger(TIM_SLAVE1, TIM_TS_ITR1); // 设置触发源为主定时器的更新事件
  // 配置从定时器2为门控同步模式
  TIM_SelectSlaveMode(TIM_SLAVE2, TIM_SlaveMode_Gated); // 设置为门控同步模式
  TIM_SelectInputTrigger(TIM_SLAVE2, TIM_TS_ITR2); // 设置触发源为另一个输入触发源
  // 启动主定时器和从定时器
  TIM_Cmd(TIM_MASTER, ENABLE); // 启动主定时器
  TIM_Cmd(TIM_SLAVE1, ENABLE); // 启动从定时器1
  TIM_Cmd(TIM_SLAVE2, ENABLE); // 启动从定时器2
}

// 初始化定时器相关的硬件
void TIMx_Init(void)
{
  TIM_GPIO_Init(); // 调用GPIO初始化函数
  TIM_Mode_Init(); // 调用定时器模式初始化函数
}
