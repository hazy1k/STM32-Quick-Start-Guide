#ifndef __ATIM_H
#define __ATIM_H

#include <sys.h>

void TIM8_PWM_Init(uint16_t arr, uint16_t psc);

#define TIM8_CH1_CCRx TIM8->CCR1
#define TIM8_CH2_CCRx TIM8->CCR2
#define TIM8_CH3_CCRx TIM8->CCR3
#define TIM8_CH4_CCRx TIM8->CCR4

#endif /* __ATIM_H */
