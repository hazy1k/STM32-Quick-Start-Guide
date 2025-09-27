/**
  ******************************************************************************
  * @file    bsp_proximity_switch.c
  * @author  fire
  * @version V1.0
  * @date    2020-xx-xx
  * @brief   接近开关中断应用bsp
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F103 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./switch/bsp_proximity_switch.h"
#include "./led/bsp_led.h"
#include "./usart/bsp_debug_usart.h"

/* 接近开关数组，可用来增减接近开关的数量 */
ProximitySwitch_TypeDef proximity_switch[4] = 
{
  {SWITCH1_INT_GPIO_PORT, SWITCH1_INT_GPIO_PIN, GPIO_MODE_IT_FALLING, SWITCH1_INT_EXTI_IRQ},
  {SWITCH2_INT_GPIO_PORT, SWITCH2_INT_GPIO_PIN, GPIO_MODE_IT_FALLING, SWITCH2_INT_EXTI_IRQ},
};

 /**
  * @brief  配置接近开关IO口，并设置中断优先级
  * @param  无
  * @retval 无
  */
void ProximitySwitch_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure; 
  
  /*开启按键GPIO口的时钟*/
  SWITCH1_INT_GPIO_CLK_ENABLE();
  SWITCH2_INT_GPIO_CLK_ENABLE();

  uint8_t size = sizeof(proximity_switch)/sizeof(ProximitySwitch_TypeDef);

  for(uint8_t i = 0; i < size; i++)
  {
    /* 选择接近开关的引脚 */ 
    GPIO_InitStructure.Pin = proximity_switch[i].pin;
    /* 设置引脚为下降沿触发模式 */ 
    GPIO_InitStructure.Mode = proximity_switch[i].gpio_mode;
    /* 设置引脚不上拉也不下拉 */
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    /* 使用上面的结构体初始化引脚 */
    HAL_GPIO_Init(proximity_switch[i].port, &GPIO_InitStructure);
    
    /* 配置 EXTI 中断源 到引脚、配置中断优先级*/
    HAL_NVIC_SetPriority(proximity_switch[i].IRQn, 1, 1);
    /* 使能中断 */
    HAL_NVIC_EnableIRQ(proximity_switch[i].IRQn);
  }
}

/**
  * @brief  外部中断回调函数
  * @param  GPIO_Pin：触发外部中断的IO引脚
  * @retval 无
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  switch(GPIO_Pin)
  {
    case SWITCH1_INT_GPIO_PIN:
      LED1_TOGGLE;
      printf("接近开关1以触发\r\n");
      break;
    case SWITCH2_INT_GPIO_PIN:
      LED2_TOGGLE;
      printf("接近开关2以触发\r\n");
      break;
    default:
      break;
  }
}
/*********************************************END OF FILE**********************/
