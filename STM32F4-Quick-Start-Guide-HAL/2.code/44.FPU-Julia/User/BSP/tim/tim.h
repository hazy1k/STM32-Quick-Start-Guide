#ifndef __TIM_H__
#define __TIM_H__ 

#include <sys.h>
#include <led.h>

#define TIMx TIM6
#define TIMx_IRQn TIM6_IRQn

void BSP_TIM_Init(uint16_t arr, uint16_t psc);

#endif /* __TIM_H__ */
