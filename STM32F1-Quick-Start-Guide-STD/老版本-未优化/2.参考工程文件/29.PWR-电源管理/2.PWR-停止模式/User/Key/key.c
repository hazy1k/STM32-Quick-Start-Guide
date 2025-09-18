#include "key.h"

void Key_NVIC_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	NVIC_InitStructure.NVIC_IRQChannel = KEY1_EXTI_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = KEY2_EXTI_IRQn;
	NVIC_Init(&NVIC_InitStructure);
}
void Key_GPIOandEXTI_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	RCC_APB2PeriphClockCmd(KEY1_GPIO_CLK, ENABLE);
	/* KEY1 引脚配置 */
	GPIO_InitStructure.GPIO_Pin = KEY1_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 输入浮空模式
	GPIO_Init(KEY1_GPIO_PORT, &GPIO_InitStructure);
	GPIO_EXTILineConfig(KEY1_EXTI_PORTSOURCE, KEY1_EXTI_PINSOURCE);
	EXTI_InitStructure.EXTI_Line = KEY1_EXTI_LINE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; // 上升沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	/* KEY2 引脚配置 */
	GPIO_InitStructure.GPIO_Pin = KEY2_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(KEY2_GPIO_PORT, &GPIO_InitStructure);
	GPIO_EXTILineConfig(KEY2_EXTI_PORTSOURCE, KEY2_EXTI_PINSOURCE);
	EXTI_InitStructure.EXTI_Line = KEY2_EXTI_LINE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; // 下降沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
}

void KEY_EXTI_Init(void)
{
	Key_NVIC_Init();
	Key_GPIOandEXTI_Init();
}
