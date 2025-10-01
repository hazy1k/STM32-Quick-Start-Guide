/**
 ****************************************************************************************************
 * @file        atim.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.2
 * @date        2020-03-19
 * @brief       高级定时器 驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 STM32H750开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.0 20200319
 * 第一次发布
 * V1.1 20200320
 * 1, 增加atim_timx_cplm_pwm_init函数
 * 2, 增加atim_timx_cplm_pwm_gpio_init函数
 * V1.2 20200321
 * 1, 增加atim_timx_pwmin_chy_process函数
 * 2, 增加atim_timx_pwmin_chy_init函数
 * 3, 增加atim_timx_pwmin_chy_restart等函数
 *
 ****************************************************************************************************
 */

#include "./BSP/TIMER/atim.h"
#include "./BSP/LED/led.h"


TIM_HandleTypeDef g_timx_comp_pwm_handle;                              /* 定时器x句柄 */

/**
 * @brief       高级定时器TIMX 输出比较模式 初始化函数（使用输出比较模式）
 * @note
 *              配置高级定时器TIMX 4路输出比较模式PWM输出,实现50%占空比,不同相位控制
 *              注意,本例程输出比较模式,每2个计数周期才能完成一个PWM输出,因此输出频率减半
 *              另外,我们还可以开启中断在中断里面修改CCRx,从而实现不同频率/不同相位的控制
 *              但是我们不推荐这么使用,因为这可能导致非常频繁的中断,从而占用大量CPU资源
 *
 *              高级定时器的时钟来自APB1,当D2PPRE2≥2分频的时候
 *              高级定时器的时钟为APB2时钟的2倍, 而APB2为120M, 所以定时器时钟 = 240Mhz
 *              定时器溢出时间计算方法: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=定时器工作频率,单位:Mhz
 *
 * @param       arr: 自动重装值。
 * @param       psc: 时钟预分频数
 * @retval      无
 */
void atim_timx_comp_pwm_init(uint16_t arr, uint16_t psc)
{
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};
    TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

    atim_timx_comp_pwm_gpio_init();
    ATIM_TIMX_COMP_CLK_ENABLE();

    g_timx_comp_pwm_handle.Instance = ATIM_TIMX_COMP;                       /* 定时器1 */
    g_timx_comp_pwm_handle.Init.Prescaler = psc  ;                          /* 定时器分频 */
    g_timx_comp_pwm_handle.Init.CounterMode = TIM_COUNTERMODE_UP;           /* 向上计数模式 */
    g_timx_comp_pwm_handle.Init.Period = arr;                               /* 自动重装载值 */
    g_timx_comp_pwm_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;     /* 时钟分频因子 */
    g_timx_comp_pwm_handle.Init.RepetitionCounter = 0;
    g_timx_comp_pwm_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    HAL_TIM_Base_Init(&g_timx_comp_pwm_handle);

    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    HAL_TIM_ConfigClockSource(&g_timx_comp_pwm_handle, &sClockSourceConfig);

    HAL_TIM_OC_Init(&g_timx_comp_pwm_handle);                               /*输出比较初始化TIM1 */

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    HAL_TIMEx_MasterConfigSynchronization(&g_timx_comp_pwm_handle, &sMasterConfig);

    sConfigOC.OCMode = TIM_OCMODE_TOGGLE;
    sConfigOC.Pulse = 250;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
    sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    HAL_TIM_OC_ConfigChannel(&g_timx_comp_pwm_handle, &sConfigOC, TIM_CHANNEL_1);
    __HAL_TIM_ENABLE_OCxPRELOAD(&g_timx_comp_pwm_handle, TIM_CHANNEL_1);
    
    sConfigOC.Pulse = 500;
    HAL_TIM_OC_ConfigChannel(&g_timx_comp_pwm_handle, &sConfigOC, TIM_CHANNEL_2);
    __HAL_TIM_ENABLE_OCxPRELOAD(&g_timx_comp_pwm_handle, TIM_CHANNEL_2);
    
    sConfigOC.Pulse = 750;
    sConfigOC.OCIdleState = TIM_OCIDLESTATE_SET;
    HAL_TIM_OC_ConfigChannel(&g_timx_comp_pwm_handle, &sConfigOC, TIM_CHANNEL_3);
    __HAL_TIM_ENABLE_OCxPRELOAD(&g_timx_comp_pwm_handle, TIM_CHANNEL_3);
    
    sConfigOC.Pulse = 1000;
    sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
    HAL_TIM_OC_ConfigChannel(&g_timx_comp_pwm_handle, &sConfigOC, TIM_CHANNEL_4);
    __HAL_TIM_ENABLE_OCxPRELOAD(&g_timx_comp_pwm_handle, TIM_CHANNEL_4);

    sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
    sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
    sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
    sBreakDeadTimeConfig.DeadTime = 0;
    sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
    sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
    sBreakDeadTimeConfig.BreakFilter = 0;
    sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
    sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
    sBreakDeadTimeConfig.Break2Filter = 0;
    sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
    HAL_TIMEx_ConfigBreakDeadTime(&g_timx_comp_pwm_handle, &sBreakDeadTimeConfig);
}

/**
 * @brief       定时器输出比较模式通道引脚初始化函数
 * @param       无
 * @note        此函数会被atim_timx_comp_pwm_init()函数调用
 * @retval      无
 */
void atim_timx_comp_pwm_gpio_init(void)
{
    GPIO_InitTypeDef gpio_init_struct = {0};
    
    ATIM_TIMX_COMP_CH1_GPIO_CLK_ENABLE();
    ATIM_TIMX_COMP_CH2_GPIO_CLK_ENABLE();
    ATIM_TIMX_COMP_CH3_GPIO_CLK_ENABLE();
    ATIM_TIMX_COMP_CH4_GPIO_CLK_ENABLE();
    
    gpio_init_struct.Pin = ATIM_TIMX_COMP_CH1_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_AF_PP;
    gpio_init_struct.Pull = GPIO_NOPULL;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    gpio_init_struct.Alternate = ATIM_TIMX_COMP_CH1_GPIO_AF;
    HAL_GPIO_Init(ATIM_TIMX_COMP_CH1_GPIO_PORT, &gpio_init_struct);
    
    gpio_init_struct.Pin = ATIM_TIMX_COMP_CH2_GPIO_PIN;
    gpio_init_struct.Alternate = ATIM_TIMX_COMP_CH2_GPIO_AF;
    HAL_GPIO_Init(ATIM_TIMX_COMP_CH2_GPIO_PORT, &gpio_init_struct);
    
    gpio_init_struct.Pin = ATIM_TIMX_COMP_CH3_GPIO_PIN;
    gpio_init_struct.Alternate = ATIM_TIMX_COMP_CH3_GPIO_AF;
    HAL_GPIO_Init(ATIM_TIMX_COMP_CH3_GPIO_PORT, &gpio_init_struct);
    
    gpio_init_struct.Pin = ATIM_TIMX_COMP_CH4_GPIO_PIN;
    gpio_init_struct.Alternate = ATIM_TIMX_COMP_CH4_GPIO_AF;
    HAL_GPIO_Init(ATIM_TIMX_COMP_CH4_GPIO_PORT, &gpio_init_struct);
}

/*******************************互补输出带死区控制程序**************************************/

TIM_HandleTypeDef g_timx_cplm_pwm_handle;                              /* 定时器x句柄 */

/**
 * @brief       高级定时器TIMX 互补输出 初始化函数（使用PWM模式1）
 * @note
 *              配置高级定时器TIMX 互补输出, 一路OCy 一路OCyN, 并且可以设置死区时间
 *
 *              高级定时器的时钟来自APB1,当D2PPRE2≥2分频的时候
 *              高级定时器的时钟为APB2时钟的2倍, 而APB2为120M, 所以定时器时钟 = 240Mhz
 *              定时器溢出时间计算方法: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=定时器工作频率,单位:Mhz
 *
 * @param       arr: 自动重装值。
 * @param       psc: 时钟预分频数
 * @retval      无
 */
void atim_timx_cplm_pwm_init(uint16_t arr, uint16_t psc)
{
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};
    TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

    atim_timx_cplm_pwm_gpio_init();
    ATIM_TIMX_CPLM_CLK_ENABLE();

    g_timx_cplm_pwm_handle.Instance = ATIM_TIMX_CPLM;
    g_timx_cplm_pwm_handle.Init.Prescaler = psc;
    g_timx_cplm_pwm_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    g_timx_cplm_pwm_handle.Init.Period = arr;
    g_timx_cplm_pwm_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    g_timx_cplm_pwm_handle.Init.RepetitionCounter = 0;
    g_timx_cplm_pwm_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    HAL_TIM_Base_Init(&g_timx_cplm_pwm_handle);
    
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    HAL_TIM_ConfigClockSource(&g_timx_cplm_pwm_handle, &sClockSourceConfig);
    
    HAL_TIM_PWM_Init(&g_timx_cplm_pwm_handle);
    
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    HAL_TIMEx_MasterConfigSynchronization(&g_timx_cplm_pwm_handle, &sMasterConfig);
    
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 300;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_LOW;
    sConfigOC.OCNPolarity = TIM_OCNPOLARITY_LOW;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
    sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_SET;
    HAL_TIM_PWM_ConfigChannel(&g_timx_cplm_pwm_handle, &sConfigOC, TIM_CHANNEL_1);
    
    sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
    sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
    sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
    sBreakDeadTimeConfig.DeadTime = 100;
    sBreakDeadTimeConfig.BreakState = TIM_BREAK_ENABLE;
    sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_LOW;
    sBreakDeadTimeConfig.BreakFilter = 0;
    sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
    sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_LOW;
    sBreakDeadTimeConfig.Break2Filter = 0;
    sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_ENABLE;
    HAL_TIMEx_ConfigBreakDeadTime(&g_timx_cplm_pwm_handle, &sBreakDeadTimeConfig);

    HAL_TIM_PWM_Start(&g_timx_cplm_pwm_handle, TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Start(&g_timx_cplm_pwm_handle,TIM_CHANNEL_1);
}

/**
 * @brief       定时器互补输出模式通道引脚初始化函数
 * @param       无
 * @note        此函数会被atim_timx_cplm_pwm_init()函数调用
 * @retval      无
 */
void atim_timx_cplm_pwm_gpio_init(void)
{
    GPIO_InitTypeDef gpio_init_struct = {0};

    ATIM_TIMX_CPLM_CHY_GPIO_CLK_ENABLE();   /* 通道X对应IO口时钟使能 */
    ATIM_TIMX_CPLM_CHYN_GPIO_CLK_ENABLE();  /* 通道X互补通道对应IO口时钟使能 */
    ATIM_TIMX_CPLM_BKIN_GPIO_CLK_ENABLE();  /* 通道X刹车输入对应IO口时钟使能 */
  
    gpio_init_struct.Pin = ATIM_TIMX_CPLM_CHY_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_AF_PP;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH ;
    gpio_init_struct.Alternate = ATIM_TIMX_CPLM_CHY_GPIO_AF;
    HAL_GPIO_Init(ATIM_TIMX_CPLM_CHY_GPIO_PORT, &gpio_init_struct);
  
    gpio_init_struct.Pin = ATIM_TIMX_CPLM_CHYN_GPIO_PIN;
    gpio_init_struct.Alternate = ATIM_TIMX_CPLM_CHYN_GPIO_AF;
    HAL_GPIO_Init(ATIM_TIMX_CPLM_CHYN_GPIO_PORT, &gpio_init_struct);
  
    gpio_init_struct.Pin = ATIM_TIMX_CPLM_BKIN_GPIO_PIN;
    gpio_init_struct.Alternate = ATIM_TIMX_CPLM_BKIN_GPIO_AF;
    HAL_GPIO_Init(ATIM_TIMX_CPLM_BKIN_GPIO_PORT, &gpio_init_struct);
}

/*******************************高级定时器PWM输入模式程序**************************************/

TIM_HandleTypeDef g_timx_pwmin_chy_handle;                              /* 定时器x句柄 */

/* PWM输入状态(g_timxchy_cap_sta)
 * 0,没有成功捕获.
 * 1,已经成功捕获了
 */
uint8_t g_timxchy_pwmin_sta  = 0;   /* PWM输入状态 */
uint16_t g_timxchy_pwmin_psc  = 0;  /* PWM输入分频系数 */
uint32_t g_timxchy_pwmin_hval = 0 ; /* PWM的高电平脉宽 */
uint32_t g_timxchy_pwmin_cval = 0 ; /* PWM的周期宽度 */

/**
 * @brief       定时器TIMX 通道Y PWM输入模式 初始化函数
 * @note
 *              通用定时器的时钟来自APB1,当D2PPRE1≥2分频的时候
 *              通用定时器的时钟为APB1时钟的2倍, 而APB1为120M, 所以定时器时钟 = 240Mhz
 *              定时器溢出时间计算方法: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=定时器工作频率,单位:Mhz
 *
 *              本函数初始化的时候: 使用psc=0, arr固定为65535. 得到采样时钟频率为240Mhz,精度4.167ns
 *
 * @param       无
 * @retval      无
 */
void atim_timx_pwmin_chy_init(void)
{
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_SlaveConfigTypeDef sSlaveConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_IC_InitTypeDef sConfigIC = {0};


    atim_timx_pwmin_chy_gpio_init();
    ATIM_TIMX_PWMIN_CHY_CLK_ENABLE();
    
    g_timx_pwmin_chy_handle.Instance = ATIM_TIMX_PWMIN;
    g_timx_pwmin_chy_handle.Init.Prescaler = 0;
    g_timx_pwmin_chy_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    g_timx_pwmin_chy_handle.Init.Period = 65535;
    g_timx_pwmin_chy_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    g_timx_pwmin_chy_handle.Init.RepetitionCounter = 0;
    g_timx_pwmin_chy_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    HAL_TIM_Base_Init(&g_timx_pwmin_chy_handle);
    
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    HAL_TIM_ConfigClockSource(&g_timx_pwmin_chy_handle, &sClockSourceConfig);
    
    HAL_TIM_IC_Init(&g_timx_pwmin_chy_handle);

    sSlaveConfig.SlaveMode = TIM_SLAVEMODE_RESET;
    sSlaveConfig.InputTrigger = TIM_TS_TI1FP1;
    sSlaveConfig.TriggerPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
    sSlaveConfig.TriggerFilter = 0;
    HAL_TIM_SlaveConfigSynchronization(&g_timx_pwmin_chy_handle,&sSlaveConfig);

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    HAL_TIMEx_MasterConfigSynchronization(&g_timx_pwmin_chy_handle, &sMasterConfig);
    
    sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
    sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
    sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
    sConfigIC.ICFilter = 0;
    HAL_TIM_IC_ConfigChannel(&g_timx_pwmin_chy_handle, &sConfigIC, TIM_CHANNEL_1);
    
    sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
    sConfigIC.ICSelection = TIM_ICSELECTION_INDIRECTTI;
    HAL_TIM_IC_ConfigChannel(&g_timx_pwmin_chy_handle, &sConfigIC, TIM_CHANNEL_2);
    
    HAL_NVIC_SetPriority(ATIM_TIMX_PWMIN_IRQn,1,3);           /* 设置中断优先级，抢占优先级1，子优先级3 */
    HAL_NVIC_EnableIRQ(ATIM_TIMX_PWMIN_IRQn);                 /* 开启ITMx中断 */
    
    /* TIM1/TIM8 有独立的输入捕获中断服务函数 */
    if (ATIM_TIMX_PWMIN == TIM1 || ATIM_TIMX_PWMIN == TIM8)
    {
        HAL_NVIC_SetPriority(ATIM_TIMX_PWMIN_CC_IRQn,1,3);    /* 设置中断优先级，抢占优先级1，子优先级3 */
        HAL_NVIC_EnableIRQ(ATIM_TIMX_PWMIN_CC_IRQn);          /* 开启ITMx中断 */
    }
    
    HAL_TIM_IC_Start_IT(&g_timx_pwmin_chy_handle,TIM_CHANNEL_1);
    HAL_TIM_IC_Start_IT(&g_timx_pwmin_chy_handle,TIM_CHANNEL_2);
    __HAL_TIM_ENABLE_IT(&g_timx_pwmin_chy_handle, TIM_IT_UPDATE);          /* 使能更新中断 */
}

/**
 * @brief       定时器PWM输入模式通道引脚初始化函数
 * @param       无
 * @note        此函数会被atim_timx_cplm_pwm_init()函数调用
 * @retval      无
 */
void atim_timx_pwmin_chy_gpio_init(void)
{
    GPIO_InitTypeDef gpio_init_struct = {0};

    ATIM_TIMX_PWMIN_CHY_GPIO_CLK_ENABLE();   /* TIMX 通道IO口时钟使能 */
  
    gpio_init_struct.Pin = ATIM_TIMX_PWMIN_CHY_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_AF_PP; 
    gpio_init_struct.Pull = GPIO_PULLDOWN;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH ;
    gpio_init_struct.Alternate = ATIM_TIMX_PWMIN_CHY_GPIO_AF;
    HAL_GPIO_Init(ATIM_TIMX_PWMIN_CHY_GPIO_PORT, &gpio_init_struct);
}

/**
 * @brief       定时器TIMX 更新/溢出 中断服务函数
 *   @note      TIM1/TIM8的这个函数仅用于更新/溢出中断服务,捕获在另外一个函数!
 *              其他普通定时器则更新/溢出/捕获,都在这个函数里面处理!
 * @param       无
 * @retval      无
 */
void ATIM_TIMX_PWMIN_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_timx_pwmin_chy_handle);/* 定时器共用处理函数 */
}

/**
 * @brief       定时器TIMX 输入捕获 中断服务函数
 *   @note      仅TIM1/TIM8有这个函数,其他普通定时器没有这个中断服务函数!
 * @param       无
 * @retval      无
 */
void ATIM_TIMX_PWMIN_CC_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_timx_pwmin_chy_handle);/* 定时器共用处理函数 */
}

/**
 * @brief       定时器TIMX PWM输入模式 重新启动捕获
 * @param       无
 * @retval      无
 */
void atim_timx_pwmin_chy_restart(void)
{
    INTX_DISABLE();                     /* 关闭中断 */

    g_timxchy_pwmin_sta = 0;            /* 清零状态,重新开始检测 */
    g_timxchy_pwmin_hval=0;
    g_timxchy_pwmin_cval=0;

    INTX_ENABLE();                      /* 打开中断 */
}

/**
 * @brief       定时器输入捕获中断处理回调函数
 * @param       htim:定时器句柄指针
 * @note        该函数在HAL_TIM_IRQHandler中会被调用
 *              此函数是定时器共同调用的回调函数，为了防止重定义错误，
 *              我们把gtim.c的HAL_TIM_IC_CaptureCallback()函数先屏蔽掉.
 * @retval      无
 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    
    if (g_timxchy_pwmin_sta == 0)   /* 还没有成功捕获 */
    {
        if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
        {  
            g_timxchy_pwmin_hval = HAL_TIM_ReadCapturedValue(&g_timx_pwmin_chy_handle,TIM_CHANNEL_2)+2; /* 修正系数为2, 加1 */
            g_timxchy_pwmin_cval = HAL_TIM_ReadCapturedValue(&g_timx_pwmin_chy_handle,TIM_CHANNEL_1)+2; /* 修正系数为2, 加1 */
            g_timxchy_pwmin_sta = 1;        /* 标记捕获成功 */
        }
    }
}










