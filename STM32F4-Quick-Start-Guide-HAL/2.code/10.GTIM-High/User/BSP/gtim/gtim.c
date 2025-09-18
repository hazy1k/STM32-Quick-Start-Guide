#include "gtim.h"
#include "led.h"

TIM_HandleTypeDef TIM5_Handle;

volatile CapState_t TIM5CH1_CAP = {0}; // volatile确保多任务环境下的可见性

/**
  * @brief  TIM5 输入捕获初始化
  * @param  arr: 自动重装载值
  * @param  psc: 预分频系数
  * @note   配置TIM5通道1为输入捕获模式
  *         时钟频率 = APB1时钟(84MHz) / (psc + 1)
  *         本例中: 84MHz / 84 = 1MHz (1us计数周期)
  */
void TIM5_CH1_Init(uint32_t arr, uint16_t psc)
{
    TIM_IC_InitTypeDef TIM_ICInitStructure;
    
    // 定时器基础配置
    TIM5_Handle.Instance = TIM5;
    TIM5_Handle.Init.Period = arr;
    TIM5_Handle.Init.Prescaler = psc;
    TIM5_Handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    TIM5_Handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_IC_Init(&TIM5_Handle);
    
    // 输入捕获通道配置
    TIM_ICInitStructure.ICPolarity = TIM_ICPOLARITY_RISING;  // 初始上升沿触发
    TIM_ICInitStructure.ICSelection = TIM_ICSELECTION_DIRECTTI; // 直接映射到TI1
    TIM_ICInitStructure.ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInitStructure.ICFilter = 0;                       // 无滤波器
    HAL_TIM_IC_ConfigChannel(&TIM5_Handle, &TIM_ICInitStructure, TIM_CHANNEL_1);
    
    // 启用中断
    __HAL_TIM_ENABLE_IT(&TIM5_Handle, TIM_IT_UPDATE);       // 溢出中断
    HAL_TIM_IC_Start_IT(&TIM5_Handle, TIM_CHANNEL_1);       // 捕获中断
    
    // 清除任何挂起的中断标志
    __HAL_TIM_CLEAR_IT(&TIM5_Handle, TIM_IT_UPDATE);
    __HAL_TIM_CLEAR_IT(&TIM5_Handle, TIM_IT_CC1);
}

/**
  * @brief  TIM5 MSP初始化
  * @param  htim: TIM句柄
  * @note   配置GPIO和NVIC
  */
void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM5)
    {
        GPIO_InitTypeDef GPIO_InitStructure;
        
        // 启用时钟
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_TIM5_CLK_ENABLE();
        
        // 配置PA0为TIM5通道1的复用功能
        GPIO_InitStructure.Pin = GPIO_PIN_0;
        GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;        // 复用推挽输出
        GPIO_InitStructure.Pull = GPIO_PULLDOWN;            
        GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStructure.Alternate = GPIO_AF2_TIM5;     // TIM5复用功能
        HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
        
        // 配置NVIC
        HAL_NVIC_SetPriority(TIM5_IRQn, 1, 0);  // 中等优先级
        HAL_NVIC_EnableIRQ(TIM5_IRQn);
    }
}

/**
  * @brief  TIM5中断服务程序
  */
void TIM5_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TIM5_Handle);
}

/**
  * @brief  输入捕获回调函数
  * @param  htim: TIM句柄
  * @note   状态机:
  *         空闲状态 -> 捕获到上升沿: 计数器清零，改为下降沿捕获
  *         已捕获上升沿 -> 捕获到下降沿: 记录捕获值，完成捕获
  */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) 
    {
        if (TIM5CH1_CAP.status & CAP_COMPLETE) return;  // 已完成，忽略新事件
        
        if (TIM5CH1_CAP.status & CAP_RISING) 
        {
            // 下降沿捕获: 完成测量
            TIM5CH1_CAP.status |= CAP_COMPLETE;         // 标记完成
            TIM5CH1_CAP.cap_val = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
            
            // 重置为上升沿捕获模式
            __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_ICPOLARITY_RISING);
        }
        else 
        {
            // 上升沿捕获: 开始测量
            TIM5CH1_CAP.status = CAP_RISING;            // 设置状态
            TIM5CH1_CAP.cap_val = 0;                    // 清零捕获值
            
            // 重置计数器并切换为下降沿捕获
            __HAL_TIM_SET_COUNTER(htim, 0);
            __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_ICPOLARITY_FALLING);
        }
    }
}

/**
  * @brief  定时器溢出回调函数
  * @param  htim: TIM句柄
  * @note   处理计数器溢出情况
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM5) 
    {
        if ((TIM5CH1_CAP.status & CAP_RISING) && !(TIM5CH1_CAP.status & CAP_COMPLETE)) 
        {
            uint8_t ovf_count = TIM5CH1_CAP.status & CAP_OVF_MASK;
            
            if (ovf_count < CAP_OVF_MASK) 
            {
                // 增加溢出计数
                TIM5CH1_CAP.status = (TIM5CH1_CAP.status & ~CAP_OVF_MASK) | (ovf_count + 1);
            }
            else 
            {
                // 溢出次数达到最大值，强制完成捕获
                TIM5CH1_CAP.status |= CAP_COMPLETE;
                TIM5CH1_CAP.cap_val = 0xFFFF;
            }
        }
    }
}