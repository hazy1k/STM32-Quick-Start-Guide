#include "atim.h"
#include "delay.h"
#include "usart.h"
#include "stdio.h"

volatile uint16_t tpad_default_val = 0;  // 无触摸时的基准值

TIM_HandleTypeDef tim_cap_handler;
TIM_IC_InitTypeDef tim_cap_ic_init;

// 定时器输入捕获初始化
static void tim_cap_init(uint32_t arr, uint16_t psc)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    // 使能时钟
    __HAL_RCC_TIM2_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    // 配置GPIO PA5 (TIM2_CH1)
    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    // 配置定时器
    tim_cap_handler.Instance = TIM2;
    tim_cap_handler.Init.Prescaler = psc;
    tim_cap_handler.Init.CounterMode = TIM_COUNTERMODE_UP;
    tim_cap_handler.Init.Period = arr;
    tim_cap_handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_IC_Init(&tim_cap_handler);
    
    // 配置输入捕获
    tim_cap_ic_init.ICPolarity = TIM_ICPOLARITY_RISING;
    tim_cap_ic_init.ICSelection = TIM_ICSELECTION_DIRECTTI;
    tim_cap_ic_init.ICPrescaler = TIM_ICPSC_DIV1;
    tim_cap_ic_init.ICFilter = 0x8;  // 增加输入滤波器
    HAL_TIM_IC_ConfigChannel(&tim_cap_handler, &tim_cap_ic_init, TIM_CHANNEL_1);
    
    // 启动输入捕获
    HAL_TIM_IC_Start(&tim_cap_handler, TIM_CHANNEL_1);
}

// 释放定时器资源
void tim_cap_deinit(void)
{
    HAL_TIM_IC_Stop(&tim_cap_handler, TIM_CHANNEL_1);
    HAL_TIM_IC_DeInit(&tim_cap_handler);
    __HAL_RCC_TIM2_CLK_DISABLE();
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_5);
}

// 复位TPAD（放电）
static void tpad_reset(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    // 配置为推挽输出放电
    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
    delay_ms(TPAD_DISCHARGE_TIME);
    
    // 复位定时器
    tim_cap_handler.Instance->SR = 0;
    tim_cap_handler.Instance->CNT = 0;
    
    // 重新配置为输入捕获
    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

// 获取单次捕获值（带超时保护）
static uint16_t tpad_get_val(void)
{
    uint32_t timeout = 0;
    const uint32_t max_timeout = 1000000; // 超时保护
    
    tpad_reset();
    
    while(__HAL_TIM_GET_FLAG(&tim_cap_handler, TIM_FLAG_CC1) == RESET)
    {
        if((tim_cap_handler.Instance->CNT > TPAD_ARR_MAX_VAL - 500) || 
           (timeout++ > max_timeout))
        {
            return tim_cap_handler.Instance->CNT;
        }
    }
    return TIM2->CCR1;
}

// 获取多次捕获中的最大值
static uint16_t tpad_get_max(uint8_t n)
{
    uint16_t max_val = 0;
    while(n--)
    {
        uint16_t val = tpad_get_val();
        if(val > max_val) max_val = val;
        delay_ms(1); // 采样间隔
    }
    return max_val;
}

// 电容按键扫描
uint8_t tpad_scan(uint8_t mode)
{
    static uint8_t keyen = 0;
    static uint16_t last_val = 0; // 上次检测值
    
    uint16_t val = tpad_get_max(5); // 采样5次取最大值
    
    if(val > (tpad_default_val + TPAD_GATE_VAL))
    {
        // 检测上升趋势（防止干扰）
        if(val > last_val + 5)
        {
            if(keyen == 0) // 首次触发
            {
                keyen = 3; // 设置去抖时间
                last_val = val;
                return 1;
            }
        }
        last_val = val;
    }
    else
    {
        last_val = val;
    }
    
    // 按键状态更新
    if(keyen) keyen--;
    
    return 0;
}

// 初始化电容按键
uint8_t tpad_init(uint16_t psc)
{
    uint16_t buf[TPAD_SAMPLE_SIZE];
    uint32_t temp = 0;
    
    tim_cap_init(TPAD_ARR_MAX_VAL, psc - 1);
    
    // 多次采样
    for(uint8_t i = 0; i < TPAD_SAMPLE_SIZE; i++)
    {
        buf[i] = tpad_get_val();
        delay_ms(5);
    }
    
    // 冒泡排序（升序）
    for(uint8_t i = 0; i < TPAD_SAMPLE_SIZE - 1; i++)
    {
        for(uint8_t j = 0; j < TPAD_SAMPLE_SIZE - 1 - i; j++)
        {
            if(buf[j] > buf[j+1])
            {
                uint16_t tmp = buf[j];
                buf[j] = buf[j+1];
                buf[j+1] = tmp;
            }
        }
    }
    
    // 取中间样本计算平均值
    const uint8_t start_idx = (TPAD_SAMPLE_SIZE - TPAD_TRIM_SAMPLES) / 2;
    for(uint8_t i = start_idx; i < start_idx + TPAD_TRIM_SAMPLES; i++)
    {
        temp += buf[i];
    }
    tpad_default_val = temp / TPAD_TRIM_SAMPLES;
    
    printf("TPAD Default Value: %u\r\n", tpad_default_val);
    
    // 检查基准值是否合理
    if(tpad_default_val > TPAD_ARR_MAX_VAL / 2)
    {
        printf("Error: Invalid baseline value!\r\n");
        return 1;
    }
    
    printf("TPAD Initialized. Sensitivity: %d\r\n", TPAD_GATE_VAL);
    return 0;
}
