#ifndef __BSP_LINEAR_INTERPOLATION_H
#define	__BSP_LINEAR_INTERPOLATION_H

#include "stm32f1xx_hal.h"
#include "./stepper/bsp_stepper_init.h"

/* 坐标轴枚举 */
typedef enum{
  x_axis = 0U,
  y_axis
}Axis_TypeDef;

/* 直线插补参数结构体 */
typedef struct{
  __IO uint32_t endpoint_x;           //终点坐标X
  __IO uint32_t endpoint_y;           //终点坐标Y
  __IO uint32_t endpoint_pulse;       //到达终点位置需要的脉冲数
  __IO uint32_t active_axis;          //当前运动的轴
  __IO int32_t deviation;             //偏差参数
}LinearInterpolation_TypeDef;

#define F       32

void InterPolation_Move(uint32_t inc_x, uint32_t inc_y, uint16_t speed);

#endif /* __BSP_STEP_LINEAR_INTERPOLATION_H */
