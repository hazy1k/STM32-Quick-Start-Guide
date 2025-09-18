#ifndef __GTIM_H__
#define __GTIM_H__ 

#include <sys.h>
#include <led.h>

void TIM14_PWM_Init(uint16_t arr, uint16_t psc);
void TIM_SetCompare(uint16_t compare);

#endif /* __GTIM_H__ */
