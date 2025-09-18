#ifndef __GTIM_H__
#define __GTIM_H__ 

#include <sys.h>
#include <led.h>

#define GTIMx TIM3
#define GTIMx_IRQn TIM3_IRQn
#define GTIMx_IRQHandler TIM3_IRQHandler
#define GTIMx_CLK_ENABLE() __HAL_RCC_TIM3_CLK_ENABLE()
void TIMx_Init(uint16_t arr, uint16_t psc);

#endif /* __TIM_H__ */
