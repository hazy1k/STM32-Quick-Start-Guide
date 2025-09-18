#ifndef __ATIM_H
#define __ATIM_H

#include <sys.h>

/* 使用TIM8通道1输出PWM */
#define PWM_TIMER               TIM8
#define PWM_CHANNEL            TIM_CHANNEL_1
#define PWM_GPIO_PORT          GPIOC
#define PWM_GPIO_PIN           GPIO_PIN_6
#define PWM_GPIO_AF            GPIO_AF3_TIM8
#define PWM_IRQn               TIM8_UP_TIM13_IRQn
#define PWM_IRQHandler         TIM8_UP_TIM13_IRQHandler

/* 函数声明 */
void tim8_pwm_init(uint16_t arr, uint16_t psc);
void tim8_set_pulse_count(uint32_t count);

#endif /* __ATIM_H */
