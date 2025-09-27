#ifndef __BSP_CIRCULAR_INTERPOLATION_H
#define	__BSP_CIRCULAR_INTERPOLATION_H

#include "./stepper/bsp_stepper_init.h"

/* 插补模式枚举 */
typedef enum{
  Circular = 0U,
  Linear
}InterpolationMODE_TypeDef;

/* 坐标轴枚举 */
typedef enum{
  x_axis = 0U,
  y_axis
}Axis_TypeDef;

/* 坐标轴象限枚举 */
typedef enum{
  quadrant_1st = 0U,
  quadrant_2nd,
  quadrant_3rd,
  quadrant_4th
}Quadrant_TypeDef;

/* 圆弧插补参数结构体 */
typedef struct{
  __IO int32_t startpoint[2];        //起点坐标X、Y
  __IO int32_t endpoint_x;           //终点坐标X
  __IO int32_t endpoint_y;           //终点坐标Y
  __IO uint32_t endpoint_pulse;      //到达终点位置需要的脉冲数
  __IO uint32_t active_axis;         //当前运动的轴
  __IO int32_t deviation;            //偏差参数F
  __IO int8_t devi_sign[2];          //偏差方程的运算符号，正负
  __IO uint8_t motionstatus : 1;     //插补运动状态
  __IO uint8_t dir_x : 1;            //X轴运动方向
  __IO uint8_t dir_y : 1;            //Y轴运动方向
  __IO uint8_t dir_interpo : 1;      //插补整体运动方向
  __IO uint8_t crood_pos : 2;        //起点坐标所在的象限
}CircularInterpolation_TypeDef;

extern InterpolationMODE_TypeDef mode;
extern CircularInterpolation_TypeDef circular_para;

void Circular_InterPolation(int32_t start_x, int32_t start_y, int32_t stop_x, int32_t stop_y, uint16_t speed, uint8_t dir);

#endif /* __BSP_STEP_CIRCULAR_INTERPOLATION_H */
