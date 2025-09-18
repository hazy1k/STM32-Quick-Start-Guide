#include "exti.h"

// 首先配置NVIC
void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); // 配置优先组
  NVIC_InitStructure.NVIC_IRQChannel = KEY1_EXTI_IRQ; // 配置中断源
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // 配置抢占优先级
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; // 配置子优先级
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; // 使能中断
  NVIC_Init(&NVIC_InitStructure); // 初始化

  // 配置中断源2即KEY2
  NVIC_InitStructure.NVIC_IRQChannel = KEY2_EXTI_IRQ; // 配置中断源
  NVIC_Init(&NVIC_InitStructure); // 沿用上面的配置
}

// EXTI初始化
void EXTI_KeyInit(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;

  RCC_APB2PeriphClockCmd(KEY1_GPIO_CLK|KEY2_GPIO_CLK, ENABLE);
  NVIC_Configuration(); // 配置NVIC
  // 配置GPIO
  GPIO_InitStructure.GPIO_Pin = KEY1_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 浮空输入模式
  GPIO_Init(KEY1_GPIO_PORT, &GPIO_InitStructure);
  // 配置EXTI
  GPIO_EXTILineConfig(KEY1_EXTI_PORTSOURCE, KEY1_EXTI_PINSOURCE);
  EXTI_InitStructure.EXTI_Line = KEY1_EXTI_LINE;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; // 中断模式
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; // 上升沿触发
  EXTI_InitStructure.EXTI_LineCmd = ENABLE; // 使能中断线
  EXTI_Init(&EXTI_InitStructure); // 初始化

  GPIO_InitStructure.GPIO_Pin = KEY2_GPIO_PIN;
  GPIO_Init(KEY2_GPIO_PORT, &GPIO_InitStructure);
  GPIO_EXTILineConfig(KEY2_EXTI_PORTSOURCE, KEY2_EXTI_PINSOURCE);
  EXTI_InitStructure.EXTI_Line = KEY2_EXTI_LINE;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; 
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; // 下升沿触发
  EXTI_InitStructure.EXTI_LineCmd = ENABLE; 
  EXTI_Init(&EXTI_InitStructure);
}
