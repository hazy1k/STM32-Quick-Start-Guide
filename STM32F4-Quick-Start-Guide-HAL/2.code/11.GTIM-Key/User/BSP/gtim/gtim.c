#include "gtim.h"
#include "bsp_init.h"
#include <stdio.h>

TIM_HandleTypeDef TIM2_Handler;          // 定时器2句柄
volatile uint32_t g_overflow_count = 0;   // 计数器溢出次数（原子访问）

/**
  * @brief  定时器2输入捕获初始化
  * @param  psc 预分频值 (0-65535)
  * @note   配置PA0作为TIM2_CH1输入，工作在外部触发模式
  */
void TIM2_Mode_Init(uint16_t psc)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    TIM_SlaveConfigTypeDef sSlaveConfig = {0};
    
    // 1. 使能时钟
    __HAL_RCC_TIM2_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    // 2. 配置GPIO (PA0: TIM2_CH1)
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;       // 复用推挽
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;         // 下拉电阻
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; // 高速模式
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;    // 复用为TIM2
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    // 3. 配置定时器基础参数
    TIM2_Handler.Instance = TIM2;
    TIM2_Handler.Init.Prescaler = psc;            // 预分频器
    TIM2_Handler.Init.CounterMode = TIM_COUNTERMODE_UP; // 向上计数
    TIM2_Handler.Init.Period = 0xFFFF;            // 自动重载值 (65535)
    TIM2_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1; // 时钟分频
    HAL_TIM_IC_Init(&TIM2_Handler);
    
    // 4. 配置从模式：外部时钟模式1
    sSlaveConfig.SlaveMode = TIM_SLAVEMODE_EXTERNAL1; // 外部时钟模式
    sSlaveConfig.InputTrigger = TIM_TS_TI1FP1;    // 触发源：TIM2_CH1
    sSlaveConfig.TriggerPolarity = TIM_TRIGGERPOLARITY_RISING; // 上升沿触发
    sSlaveConfig.TriggerPrescaler = TIM_TRIGGERPRESCALER_DIV1;
    sSlaveConfig.TriggerFilter = 0;               // 无滤波
    if (HAL_TIM_SlaveConfigSynchronization(&TIM2_Handler, &sSlaveConfig) != HAL_OK)
    {
        while(1);
    }
    
    // 5. 配置中断
    HAL_NVIC_SetPriority(TIM2_IRQn, 1, 3);       // 中断优先级
    HAL_NVIC_EnableIRQ(TIM2_IRQn);               // 使能中断
    __HAL_TIM_ENABLE_IT(&TIM2_Handler, TIM_IT_UPDATE); // 使能更新中断
    
    // 6. 启动输入捕获通道
    if (HAL_TIM_IC_Start(&TIM2_Handler, TIM_CHANNEL_1) != HAL_OK)
    {
        while(1);
    }
}

/**
  * @brief  获取当前脉冲计数值（原子操作）
  * @retval 累计脉冲计数值
  */
uint32_t TIM2_CH1_GetCount(void)
{
    uint32_t count, overflow;
    
    // 原子操作：读取时禁止中断
    __disable_irq();
    overflow = g_overflow_count;
    count = __HAL_TIM_GET_COUNTER(&TIM2_Handler);
    __enable_irq();
    
    return (overflow * 0x10000) + count; // 组合溢出值和当前计数值
}

/**
  * @brief  重启脉冲计数器
  */
void TIM2_CH1_Restart(void)
{
    // 原子操作：重置时禁止中断
    __disable_irq();
    __HAL_TIM_DISABLE(&TIM2_Handler);     // 关闭定时器
    g_overflow_count = 0;                 // 重置溢出计数
    __HAL_TIM_SET_COUNTER(&TIM2_Handler, 0); // 计数器归零
    __HAL_TIM_ENABLE(&TIM2_Handler);      // 重新使能定时器
    __enable_irq();
}

/**
  * @brief  定时器2中断服务函数
  * @note   处理计数器溢出事件
  */
void TIM2_IRQHandler(void)
{
    if (__HAL_TIM_GET_FLAG(&TIM2_Handler, TIM_FLAG_UPDATE) != RESET)
    {
        if (__HAL_TIM_GET_IT_SOURCE(&TIM2_Handler, TIM_IT_UPDATE) != RESET)
        {
            g_overflow_count++;           // 溢出计数增加
            __HAL_TIM_CLEAR_IT(&TIM2_Handler, TIM_IT_UPDATE); // 清除中断标志
        }
    }
}
