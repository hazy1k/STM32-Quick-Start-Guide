#include "atim.h"
#include "./BSP/LED/led.h"

TIM_HandleTypeDef tim8_handle;
static uint32_t pulse_remain = 0;  // 剩余脉冲计数

/* PWM初始化 */
void tim8_pwm_init(uint16_t arr, uint16_t psc) {
    GPIO_InitTypeDef gpio_init = {0};
    TIM_OC_InitTypeDef oc_init = {0};
    
    /* 1. 使能时钟 */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_TIM8_CLK_ENABLE();
    
    /* 2. 配置GPIO */
    gpio_init.Pin = PWM_GPIO_PIN;
    gpio_init.Mode = GPIO_MODE_AF_PP;
    gpio_init.Pull = GPIO_PULLUP;
    gpio_init.Speed = GPIO_SPEED_FREQ_HIGH;
    gpio_init.Alternate = PWM_GPIO_AF;
    HAL_GPIO_Init(PWM_GPIO_PORT, &gpio_init);
    
    /* 3. 配置定时器 */
    tim8_handle.Instance = PWM_TIMER;
    tim8_handle.Init.Prescaler = psc;
    tim8_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    tim8_handle.Init.Period = arr;
    tim8_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    tim8_handle.Init.RepetitionCounter = 0;
    HAL_TIM_PWM_Init(&tim8_handle);
    
    /* 4. 配置PWM通道 */
    oc_init.OCMode = TIM_OCMODE_PWM1;
    oc_init.Pulse = arr / 2;  // 默认50%占空比
    oc_init.OCPolarity = TIM_OCPOLARITY_HIGH;
    HAL_TIM_PWM_ConfigChannel(&tim8_handle, &oc_init, PWM_CHANNEL);
    
    /* 5. 配置中断 */
    HAL_NVIC_SetPriority(PWM_IRQn, 1, 3);
    HAL_NVIC_EnableIRQ(PWM_IRQn);
    __HAL_TIM_ENABLE_IT(&tim8_handle, TIM_IT_UPDATE);
    
    /* 6. 启动PWM */
    HAL_TIM_PWM_Start(&tim8_handle, PWM_CHANNEL);
}

/* 设置脉冲数量 */
void tim8_set_pulse_count(uint32_t count) {
    if (count == 0) return;
    
    pulse_remain = count;
    HAL_TIM_GenerateEvent(&tim8_handle, TIM_EVENTSOURCE_UPDATE);
    __HAL_TIM_ENABLE(&tim8_handle);
}

/* 定时器中断处理 */
void PWM_IRQHandler(void) {
    if (__HAL_TIM_GET_FLAG(&tim8_handle, TIM_FLAG_UPDATE)) {
        uint16_t pulses = 0;
        
        if (pulse_remain > 0) {
            pulses = (pulse_remain > 256) ? 256 : pulse_remain;
            pulse_remain -= pulses;
            
            PWM_TIMER->RCR = pulses - 1;  // 设置重复计数器
            HAL_TIM_GenerateEvent(&tim8_handle, TIM_EVENTSOURCE_UPDATE);
            __HAL_TIM_ENABLE(&tim8_handle);
        } else {
            PWM_TIMER->CR1 &= ~TIM_CR1_CEN;  // 关闭定时器
        }
        
        __HAL_TIM_CLEAR_IT(&tim8_handle, TIM_IT_UPDATE);
    }
}
