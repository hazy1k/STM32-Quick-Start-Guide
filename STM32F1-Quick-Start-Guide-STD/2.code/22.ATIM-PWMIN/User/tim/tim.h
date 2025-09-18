#ifndef __TIM_H__
#define __TIM_H__

#include "stm32f10x.h"

// PWM输入捕获结构体
typedef struct {
    uint32_t period;     // PWM周期 (单位: 定时器计数)
    uint32_t duty_cycle; // PWM占空比 (单位: 定时器计数)
    float frequency;     // PWM频率 (Hz)
    float duty_ratio;    // 占空比百分比 (%)
} PWM_CaptureData;

void TIM2_PWM_Init(void);
void TIM1_PWMCapture_Init(void);
void TIM2_SetPWM(uint32_t period, uint32_t duty_cycle);
PWM_CaptureData TIM1_GetPWMCaptureData(void);

#endif /* __TIM_H__ */
