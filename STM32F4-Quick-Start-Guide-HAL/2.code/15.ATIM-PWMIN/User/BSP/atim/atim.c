#include <atim.h>

TIM_HandleTypeDef g_tim8_pwmin_handle;

/* 状态变量 0,无，1，有 */
uint8_t PWMIN_STA = 0;// PWM输入状态
uint16_t PWMIN_PSC = 0;// PWM输入分频系数
uint32_t PWMIN_Highvlaue = 0; // PWM输入高电平时间
uint32_t PWMIN_CycleTime = 0; // PWM输入周期时间

// 初始化定时器8通道1为PWM输入模式
void tim8_pwmin_init(void)
{
    GPIO_InitTypeDef gpio_init;
    TIM_SlaveConfigTypeDef slave_config;
    TIM_IC_InitTypeDef ic_config;
    // 启用时钟
    __HAL_RCC_TIM8_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    // 配置GPIO-PC6
    gpio_init.Pin = GPIO_PIN_6;
    gpio_init.Mode = GPIO_MODE_AF_PP;
    gpio_init.Pull = GPIO_PULLDOWN;
    gpio_init.Speed = GPIO_SPEED_FREQ_HIGH;
    gpio_init.Alternate = GPIO_AF3_TIM8;
    HAL_GPIO_Init(GPIOC, &gpio_init);
    // 配置定时器基础参数
    g_tim8_pwmin_handle.Instance = TIM8;
    g_tim8_pwmin_handle.Init.Prescaler = 0;
    g_tim8_pwmin_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    g_tim8_pwmin_handle.Init.Period = 0xFFFF;
    g_tim8_pwmin_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_IC_Init(&g_tim8_pwmin_handle);
    // 从模式配置：复位模式
    slave_config.SlaveMode = TIM_SLAVEMODE_RESET;
    slave_config.InputTrigger = TIM_TS_TI1FP1;
    slave_config.TriggerPolarity = TIM_TRIGGERPOLARITY_RISING;
    slave_config.TriggerFilter = 0;
    HAL_TIM_SlaveConfigSynchro(&g_tim8_pwmin_handle, &slave_config);
    // 通道1配置：直接捕获上升沿
    ic_config.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
    ic_config.ICSelection = TIM_ICSELECTION_DIRECTTI;
    ic_config.ICPrescaler = TIM_ICPSC_DIV1;
    ic_config.ICFilter = 0;
    HAL_TIM_IC_ConfigChannel(&g_tim8_pwmin_handle, &ic_config, TIM_CHANNEL_1);
    // 通道2配置：间接捕获下降沿
    ic_config.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
    ic_config.ICSelection = TIM_ICSELECTION_INDIRECTTI;
    HAL_TIM_IC_ConfigChannel(&g_tim8_pwmin_handle, &ic_config, TIM_CHANNEL_2);
    // 配置NVIC中断
    HAL_NVIC_SetPriority(TIM8_CC_IRQn, 1, 3);
    HAL_NVIC_EnableIRQ(TIM8_CC_IRQn);
    HAL_NVIC_SetPriority(TIM8_UP_TIM13_IRQn, 1, 3);
    HAL_NVIC_EnableIRQ(TIM8_UP_TIM13_IRQn);
    // 启动捕获
    __HAL_TIM_ENABLE_IT(&g_tim8_pwmin_handle, TIM_IT_UPDATE);
    HAL_TIM_IC_Start_IT(&g_tim8_pwmin_handle, TIM_CHANNEL_1);
    HAL_TIM_IC_Start_IT(&g_tim8_pwmin_handle, TIM_CHANNEL_2);
}

// 重启PWM捕获
void restart_pwmin_capture(void)
{
    __disable_irq();
    
    // 重置状态
    PWMIN_STA = 0; // 重置PWM输入状态
    PWMIN_PSC = 0; 
    __HAL_TIM_SET_PRESCALER(&g_tim8_pwmin_handle, 0); // 以最大的计数频率采集
    __HAL_TIM_SET_COUNTER(&g_tim8_pwmin_handle, 0); // 重置计数器
    __HAL_TIM_ENABLE_IT(&g_tim8_pwmin_handle, TIM_IT_CC1|TIM_IT_UPDATE); // 重新使能捕获中断和更新中断
    __HAL_TIM_ENABLE(&g_tim8_pwmin_handle);
    __HAL_TIM_CLEAR_FLAG(&g_tim8_pwmin_handle, TIM_FLAG_CC1|TIM_FLAG_CC2|TIM_FLAG_UPDATE); // 清除捕获中断和更新中断标志
    __enable_irq();
}

// PWM输入模式中断处理函数
static void tim8_pwmin_irq_handler(void)
{
    static uint8_t sflag = 0; // 启动PWM输入检测标志
    if(PWMIN_STA)
    {
        PWMIN_PSC = 0;
        __HAL_TIM_CLEAR_FLAG(&g_tim8_pwmin_handle, TIM_FLAG_CC1|TIM_FLAG_CC2|TIM_FLAG_UPDATE);
        __HAL_TIM_SET_COUNTER(&g_tim8_pwmin_handle, 0);
        return;
    }
    if(__HAL_TIM_GET_FLAG(&g_tim8_pwmin_handle, TIM_FLAG_UPDATE)) // 更新中断
    {
        __HAL_TIM_CLEAR_FLAG(&g_tim8_pwmin_handle, TIM_FLAG_UPDATE); // 清除更新中断标志
        if(__HAL_TIM_GET_FLAG(&g_tim8_pwmin_handle, TIM_FLAG_CC1) == 0) // 如果没有发生捕获中断，且捕获未完成
        {
            sflag = 0;
            if(PWMIN_PSC == 0)
            {
                PWMIN_PSC++;
            }
            else
            {
                if(PWMIN_PSC == 65535) // 已经最大，可能是无输入状态
                {
                    PWMIN_PSC = 0;
                }
                else if(PWMIN_PSC > 32767) // 限制
                {
                    PWMIN_PSC = 65535;
                }
                else
                {
                    PWMIN_PSC += PWMIN_PSC;
                }
            }
            __HAL_TIM_SET_PRESCALER(&g_tim8_pwmin_handle, PWMIN_PSC); // 设置分频系数
            __HAL_TIM_SET_COUNTER(&g_tim8_pwmin_handle, 0); // 重置计数器
            __HAL_TIM_CLEAR_IT(&g_tim8_pwmin_handle, TIM_IT_CC1|TIM_IT_CC2|TIM_IT_UPDATE); // 清除捕获中断和更新中断标志
            return;
        }
    }
    if(sflag == 0) // 第一次采集到捕获中断
    {
        if(__HAL_TIM_GET_FLAG(&g_tim8_pwmin_handle, TIM_FLAG_CC1)) // 捕获中断1
        {
            sflag = 1;
        }
        __HAL_TIM_CLEAR_FLAG(&g_tim8_pwmin_handle, TIM_FLAG_CC1|TIM_FLAG_CC2|TIM_FLAG_UPDATE);
        return;
    }
    if(PWMIN_STA == 0) // 还没有成功捕获
    {
        if(__HAL_TIM_GET_FLAG(&g_tim8_pwmin_handle, TIM_FLAG_CC1))
        {
            PWMIN_Highvlaue = HAL_TIM_ReadCapturedValue(&g_tim8_pwmin_handle, TIM_CHANNEL_2)+1; // 读取高电平时间
            PWMIN_CycleTime = HAL_TIM_ReadCapturedValue(&g_tim8_pwmin_handle, TIM_CHANNEL_1)+1; // 读取周期时间
            if(PWMIN_Highvlaue < PWMIN_CycleTime) // 周期时间必定大于高电平时间
            {
                PWMIN_STA = 1; // 成功捕获到一次PWM输入
                PWMIN_PSC = TIM8->PSC; // 记录分频系数
                if(PWMIN_PSC == 0)
                {
                    PWMIN_Highvlaue++;
                    PWMIN_CycleTime++;
                }
                sflag = 0; // 捕获完成，准备下一次捕获
                /* 停止捕获 */
                TIM8->CR1 &= ~(1<<0);
                __HAL_TIM_DISABLE_IT(&g_tim8_pwmin_handle, TIM_IT_CC1|TIM_IT_CC2|TIM_IT_UPDATE); // 停止捕获中断和更新中断
                __HAL_TIM_CLEAR_FLAG(&g_tim8_pwmin_handle, TIM_FLAG_CC1|TIM_FLAG_CC2|TIM_FLAG_UPDATE); // 清除捕获中断和更新中断标志
            }
            else
            {
                restart_pwmin_capture(); // 周期时间小于高电平时间，可能是无输入状态，重新捕获
            }
        }
    }
    __HAL_TIM_CLEAR_FLAG(&g_tim8_pwmin_handle, TIM_FLAG_CC1|TIM_FLAG_CC2|TIM_FLAG_UPDATE); // 清除捕获中断和更新中断标志
}

void TIM8_CC_IRQHandler(void)
{
    tim8_pwmin_irq_handler();
}

void TIM8_UP_TIM13_IRQHandler(void)
{
    tim8_pwmin_irq_handler();
}

/* TIM14_CH1作为PWM输出 */
TIM_HandleTypeDef g_tim14_pwmout_handle;

void tim14_pwmout_init(uint16_t arr, uint16_t psc)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    TIM_OC_InitTypeDef oc_init;
    
    // 启用时钟
    __HAL_RCC_TIM14_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    
    // 配置GPIO
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_TIM14;  
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);  

    // 配置定时器
    g_tim14_pwmout_handle.Instance = TIM14;
    g_tim14_pwmout_handle.Init.Prescaler = psc;
    g_tim14_pwmout_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    g_tim14_pwmout_handle.Init.Period = arr;
    g_tim14_pwmout_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_PWM_Init(&g_tim14_pwmout_handle);
    
    // 配置PWM通道
    oc_init.OCMode = TIM_OCMODE_PWM1;
    oc_init.Pulse = arr / 2;  // 默认50%占空比
    oc_init.OCPolarity = TIM_OCPOLARITY_HIGH;
    oc_init.OCFastMode = TIM_OCFAST_DISABLE;
    HAL_TIM_PWM_ConfigChannel(&g_tim14_pwmout_handle, &oc_init, TIM_CHANNEL_1);
    
    // 启动PWM
    HAL_TIM_PWM_Start(&g_tim14_pwmout_handle, TIM_CHANNEL_1);
}
