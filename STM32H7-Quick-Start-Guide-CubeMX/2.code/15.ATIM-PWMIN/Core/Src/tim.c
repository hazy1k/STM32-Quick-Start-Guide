#include "tim.h"
#include "stdio.h"
#include "usart.h"

// 定义全局变量用于PWM测量
volatile uint32_t IC_Value1 = 0;
volatile uint32_t IC_Value2 = 0;
volatile uint32_t IC_Value3 = 0;
volatile uint32_t DutyCycle = 0;
volatile uint32_t Frequency = 0;
volatile uint8_t capture_state = 0;  // 0:等待上升沿, 1:等待下降沿, 2:等待第二个上升沿

// 添加调试计数器
volatile uint32_t capture_count = 0;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim12;

/* TIM1 init function */
void MX_TIM1_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 479; // 480MHz / (479+1) = 1MHz
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 0xFFFF; // 最大计数值
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;

  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_TIM_IC_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }

  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0; // 减少滤波，提高响应速度
  if (HAL_TIM_IC_ConfigChannel(&htim1, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/* TIM12 init function */
void MX_TIM12_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  htim12.Instance = TIM12;
  htim12.Init.Prescaler = 479; // 480MHz / (479+1) = 1MHz
  htim12.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim12.Init.Period = 99;     // 1MHz / 100 = 10kHz
  htim12.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  // htim12.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;

  if (HAL_TIM_Base_Init(&htim12) != HAL_OK)
  {
    Error_Handler();
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim12, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_TIM_PWM_Init(&htim12) != HAL_OK)
  {
    Error_Handler();
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim12, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }

  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 50; // 50% 占空比
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE; // 禁用快速模式
  if (HAL_TIM_PWM_ConfigChannel(&htim12, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_TIM_MspPostInit(&htim12);
}

// 简化后的回调函数
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM1)
  {
    capture_count++;

    switch(capture_state)
    {
      case 0: // 第一个上升沿
        IC_Value1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
        capture_state = 1;
        // 改变极性为下降沿捕获
        __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_FALLING);
        break;

      case 1: // 下降沿
        IC_Value2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
        capture_state = 2;
        // 改变极性为上升沿捕获
        __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);
        break;

      case 2: // 第二个上升沿
        IC_Value3 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);

        // 计算周期和占空比
        uint32_t period, high_time;

        if (IC_Value3 > IC_Value1)
        {
          period = IC_Value3 - IC_Value1;
        }
        else
        {
          period = (0xFFFF - IC_Value1) + IC_Value3;
        }

        if (IC_Value2 > IC_Value1)
        {
          high_time = IC_Value2 - IC_Value1;
        }
        else
        {
          high_time = (0xFFFF - IC_Value1) + IC_Value2;
        }

        // 计算频率 (定时器时钟为1MHz)
        if (period > 0)
        {
          Frequency = 1000000 / period;
          // 计算占空比百分比
          float duty_cycle_percent = (float)high_time / period * 100;

          // 打印PWM信息
          printf("Capture #%lu: Freq=%lu Hz, Duty=%.2f%%\r\n",
                 capture_count, Frequency, duty_cycle_percent);
        }

        capture_state = 0; // 重置状态
        break;
    }
  }
}

// 启动PWM输入捕获
void Start_PWM_Input_Capture(void)
{
  // 先停止可能正在运行的捕获
  HAL_TIM_IC_Stop_IT(&htim1, TIM_CHANNEL_1);

  // 重置状态
  capture_state = 0;
  capture_count = 0;
  IC_Value1 = 0;
  IC_Value2 = 0;
  IC_Value3 = 0;

  // 设置为上升沿捕获
  __HAL_TIM_SET_CAPTUREPOLARITY(&htim1, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);

  // 清除可能挂起的中断标志
  __HAL_TIM_CLEAR_FLAG(&htim1, TIM_FLAG_CC1);

  // 启动输入捕获中断
  HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_1);

  printf("PWM Input Capture Started\r\n");
}

// 启动PWM输出
void Start_PWM_Output(void)
{
  HAL_TIM_PWM_Start(&htim12, TIM_CHANNEL_1);
  printf("PWM Output Started: 10kHz, 50%% duty\r\n");
}

// 停止PWM输出
void Stop_PWM_Output(void)
{
  HAL_TIM_PWM_Stop(&htim12, TIM_CHANNEL_1);
}

// 设置PWM输出频率和占空比
void Set_PWM_Output(uint32_t frequency, float duty_cycle)
{
  // 停止PWM输出
  Stop_PWM_Output();

  // 计算新的ARR和CCR值
  // 定时器时钟为1MHz (480MHz/(479+1))
  uint32_t arr_value = 1000000 / frequency - 1;
  uint32_t ccr_value = (arr_value + 1) * duty_cycle / 100;

  // 设置新的ARR和CCR值
  __HAL_TIM_SET_AUTORELOAD(&htim12, arr_value);
  __HAL_TIM_SET_COMPARE(&htim12, TIM_CHANNEL_1, ccr_value);

  // 重新启动PWM输出
  HAL_TIM_PWM_Start(&htim12, TIM_CHANNEL_1);

  printf("PWM Output Set: %lu Hz, %.1f%% duty\r\n", frequency, duty_cycle);
}

// 添加定时器更新中断处理
void TIM1_UP_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&htim1);

  // 检查是否发生溢出
  if (__HAL_TIM_GET_FLAG(&htim1, TIM_FLAG_UPDATE) != RESET)
  {
    __HAL_TIM_CLEAR_FLAG(&htim1, TIM_FLAG_UPDATE);

    // 如果发生溢出，重置捕获状态
    if (capture_state != 0)
    {
      capture_state = 0;
      __HAL_TIM_SET_CAPTUREPOLARITY(&htim1, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);
      printf("Timer overflow detected, resetting capture state\r\n");
    }
  }
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	if(tim_baseHandle->Instance==TIM1)
	{
		__HAL_RCC_TIM1_CLK_ENABLE();
		__HAL_RCC_GPIOE_CLK_ENABLE();
		GPIO_InitStruct.Pin = PWM_CH1IN_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
		HAL_GPIO_Init(PWM_CH1IN_GPIO_Port, &GPIO_InitStruct);
		HAL_NVIC_SetPriority(TIM1_UP_IRQn, 1, 3);
		HAL_NVIC_EnableIRQ(TIM1_UP_IRQn);
		HAL_NVIC_SetPriority(TIM1_CC_IRQn, 1, 3);
		HAL_NVIC_EnableIRQ(TIM1_CC_IRQn);
	}
	else if(tim_baseHandle->Instance==TIM12)
	{
		__HAL_RCC_TIM12_CLK_ENABLE();
	}
}

void HAL_TIM_MspPostInit(TIM_HandleTypeDef* timHandle)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	if(timHandle->Instance==TIM12)
	{
		__HAL_RCC_GPIOB_CLK_ENABLE();
		GPIO_InitStruct.Pin = PWM_CH1_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF2_TIM12;
		HAL_GPIO_Init(PWM_CH1_GPIO_Port, &GPIO_InitStruct);
	}
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle)
{
	if(tim_baseHandle->Instance==TIM1)
	{
		__HAL_RCC_TIM1_CLK_DISABLE();
		HAL_GPIO_DeInit(PWM_CH1IN_GPIO_Port, PWM_CH1IN_Pin);
		HAL_NVIC_DisableIRQ(TIM1_UP_IRQn);
		HAL_NVIC_DisableIRQ(TIM1_CC_IRQn);
	}
	else if(tim_baseHandle->Instance==TIM12)
	{
		__HAL_RCC_TIM12_CLK_DISABLE();
	}
}
