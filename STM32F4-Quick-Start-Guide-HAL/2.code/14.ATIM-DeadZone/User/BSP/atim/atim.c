/**
  ******************************************************************************
  * @file    atim.c
  * @brief   高级定时器(ATIM)互补PWM输出实现
  * @note    本文件实现ATIM1的互补PWM输出功能，包含：
  *          - PWM初始化
  *          - 死区时间设置
  *          - 刹车功能配置
  *          使用PE8(OCyN)、PE9(OCy)和PE15(BKIN)引脚
  ******************************************************************************
  */
#include <atim.h>
#include <stdio.h>

/* 全局变量定义 */
TIM_HandleTypeDef timx_pwm_handle;                // 定时器句柄
TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig; // 刹车和死区配置结构体

/**
  * @brief  初始化高级定时器1的互补PWM输出
  * @param  arr: 自动重装载值 (决定PWM周期)
  * @param  psc: 预分频值 (决定定时器时钟频率)
  * @note   定时器频率 = 系统时钟 / (psc + 1)
  *         PWM频率 = 定时器频率 / (arr + 1)
  *         配置引脚: PE9(OCy), PE8(OCyN), PE15(BKIN)
  */
void ATIM_PWM_Init(uint16_t arr, uint16_t psc)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_OC_InitTypeDef TIM_OCInitStructure;
    HAL_StatusTypeDef status;
    
    printf("[ATIM] 开始初始化互补PWM输出...\r\n");
    
    /* 步骤1: 使能时钟 */
    printf("[ATIM] 使能TIM1和GPIOE时钟...\r\n");
    __HAL_RCC_TIM1_CLK_ENABLE();      // 使能TIM1时钟
    __HAL_RCC_GPIOE_CLK_ENABLE();     // 使能GPIOE时钟

    /* 步骤2: 配置GPIO引脚 */
    printf("[ATIM] 配置PE8(OCyN), PE9(OCy), PE15(BKIN)为复用推挽输出...\r\n");
    GPIO_InitStructure.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_15;  // PE8, PE9, PE15
    GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;      // 复用推挽输出
    GPIO_InitStructure.Pull = GPIO_PULLDOWN;        // 下拉电阻
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH; // 高速模式
    GPIO_InitStructure.Alternate = GPIO_AF1_TIM1;   // 复用为TIM1功能
    HAL_GPIO_Init(GPIOE, &GPIO_InitStructure);      // 应用配置

    /* 步骤3: 配置定时器基础参数 */
    printf("[ATIM] 配置定时器基础参数(ARR=%u, PSC=%u)...\r\n", arr, psc);
    timx_pwm_handle.Instance = TIM1;                // 使用TIM1
    timx_pwm_handle.Init.Prescaler = psc;           // 预分频值
    timx_pwm_handle.Init.CounterMode = TIM_COUNTERMODE_UP; // 向上计数模式
    timx_pwm_handle.Init.Period = arr;              // 自动重装载值
    timx_pwm_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV4; // 时钟4分频
    timx_pwm_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE; // 自动重载预装载使能
    
    // 应用定时器配置
    status = HAL_TIM_PWM_Init(&timx_pwm_handle);
    if(status != HAL_OK) {
        printf("[错误] 定时器初始化失败: %d\r\n", status);
        return;
    }

    /* 步骤4: 配置PWM通道 */
    printf("[ATIM] 配置PWM通道1参数...\r\n");
    TIM_OCInitStructure.OCMode = TIM_OCMODE_PWM1;   // PWM模式1
    TIM_OCInitStructure.Pulse = 0;                  // 初始占空比为0
    TIM_OCInitStructure.OCPolarity = TIM_OCPOLARITY_HIGH; // 输出极性高
    TIM_OCInitStructure.OCNPolarity = TIM_OCNPOLARITY_HIGH; // 互补输出极性高
    TIM_OCInitStructure.OCIdleState = TIM_OCIDLESTATE_SET; // 空闲状态输出高
    TIM_OCInitStructure.OCNIdleState = TIM_OCNIDLESTATE_SET; // 互补输出空闲状态高
    
    // 应用PWM通道配置
    status = HAL_TIM_PWM_ConfigChannel(&timx_pwm_handle, &TIM_OCInitStructure, TIM_CHANNEL_1);
    if(status != HAL_OK) {
        printf("[错误] PWM通道配置失败: %d\r\n", status);
        return;
    }

    /* 步骤5: 配置刹车和死区时间 */
    printf("[ATIM] 配置刹车和死区参数...\r\n");
    sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;   // 运行模式关闭状态禁用
    sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;  // 空闲模式关闭状态禁用
    sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;        // 无锁定
    sBreakDeadTimeConfig.BreakState = TIM_BREAK_ENABLE;        // 使能刹车输入
    sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH; // 刹车高电平有效
    sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_ENABLE; // 自动输出使能
    sBreakDeadTimeConfig.DeadTime = 0;                         // 初始死区时间为0
    
    // 应用刹车和死区配置
    status = HAL_TIMEx_ConfigBreakDeadTime(&timx_pwm_handle, &sBreakDeadTimeConfig);
    if(status != HAL_OK) {
        printf("[错误] 刹车/死区配置失败: %d\r\n", status);
        return;
    }

    /* 步骤6: 启动PWM输出 */
    printf("[ATIM] 启动PWM主输出和互补输出...\r\n");
    HAL_TIM_PWM_Start(&timx_pwm_handle, TIM_CHANNEL_1);      // 启动主输出
    HAL_TIMEx_PWMN_Start(&timx_pwm_handle, TIM_CHANNEL_1);   // 启动互补输出
    
    printf("[ATIM] PWM初始化成功!\r\n");
}

/**
  * @brief  计算死区时间并打印结果
  * @param  dtg: 死区时间生成值 (DTG寄存器值)
  * @note   根据STM32F4参考手册计算实际死区时间
  *         死区时间取决于DTG值和tDTS(定时器分频后时钟周期)
  */
static void calc_dead_time(uint8_t dtg) 
{
    /* 死区时间计算基础:
     * tDTS = 1 / (定时器输入时钟 / 时钟分频系数)
     * 本例: 系统时钟168MHz, CKD[1:0]=2(4分频) -> 42MHz
     * tDTS = 1 / 42MHz ≈ 23.81ns
     */
    const float tDTS = 23.81f; // 单位: 纳秒(ns)
    float dead_time = 0;       // 计算得到的死区时间
    
    /* 提取DTG的高3位(决定计算模式) */
    uint8_t mode = (dtg >> 5) & 0x07;
    
    /* 根据模式选择不同的计算公式 */
    if(mode <= 3) { 
        // 模式0xx: 死区时间 = dtg[7:0] * tDTS
        dead_time = dtg * tDTS;
        printf("[ATIM] 模式0xx: DT = DTG * tDTS\r\n");
    } 
    else if(mode == 4 || mode == 5) { 
        // 模式10x: 死区时间 = (64 + dtg[6:0]) * 2 * tDTS
        uint8_t dtg_val = dtg & 0x7F; // 取低7位
        dead_time = (64.0f + dtg_val) * 2.0f * tDTS;
        printf("[ATIM] 模式10x: DT = (64 + DTG[6:0]) * 2 * tDTS\r\n");
    }
    else if(mode == 6) { 
        // 模式110: 死区时间 = (32 + dtg[5:0]) * 8 * tDTS
        uint8_t dtg_val = dtg & 0x3F; // 取低6位
        dead_time = (32.0f + dtg_val) * 8.0f * tDTS;
        printf("[ATIM] 模式110: DT = (32 + DTG[5:0]) * 8 * tDTS\r\n");
    }
    else if(mode == 7) { 
        // 模式111: 死区时间 = (32 + dtg[5:0]) * 16 * tDTS
        uint8_t dtg_val = dtg & 0x3F; // 取低6位
        dead_time = (32.0f + dtg_val) * 16.0f * tDTS;
        printf("[ATIM] 模式111: DT = (32 + DTG[5:0]) * 16 * tDTS\r\n");
    }

    /* 打印DTG值的二进制表示(带格式) */
    printf("[ATIM] DTG=0x%02X (二进制: ", dtg);
    for(int i = 7; i >= 0; i--) {
        printf("%d", (dtg >> i) & 1);
        if(i == 5) printf(" "); // 在DTG[5]后加空格分组
    }
    printf(")\r\n");
    
    /* 打印计算结果 */
    printf("[ATIM] 计算死区时间 = %.2f ns (约 %.2f μs)\r\n", 
           dead_time, dead_time / 1000.0f);
}

/**
  * @brief  设置死区时间和PWM占空比
  * @param  ccr: 捕获比较值 (决定PWM占空比)
  * @param  dtg: 死区时间生成值 (0-255)
  * @note   占空比 = ccr / (arr + 1)
  *         死区时间由dtg值根据特定公式计算得出
  */
void ATIM_PWM_SetDeadZone(uint16_t ccr, uint8_t dtg)
{
    HAL_StatusTypeDef status;
    
    printf("\r\n[ATIM] 设置死区时间 (CCR=%u, DTG=0x%02X)\r\n", ccr, dtg);
    
    /* 计算并打印死区时间 */
    calc_dead_time(dtg);
    
    /* 更新死区时间配置 */
    sBreakDeadTimeConfig.DeadTime = dtg;
    status = HAL_TIMEx_ConfigBreakDeadTime(&timx_pwm_handle, &sBreakDeadTimeConfig);
    if(status != HAL_OK) {
        printf("[错误] 死区时间更新失败: %d\r\n", status);
        return;
    }
    
    /* 设置捕获比较值(CCR) */
    TIM1->CCR1 = ccr;
    
    /* 使能主输出(Main Output Enable) */
    __HAL_TIM_MOE_ENABLE(&timx_pwm_handle);
    
    printf("[ATIM] 死区时间设置成功\r\n");
}
