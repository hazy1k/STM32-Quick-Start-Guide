#ifndef __BSP_STEPPER_S_SPEED_H
#define	__BSP_STEPPER_S_SPEED_H

#include "stm32f1xx_hal.h"
#include "./usart/bsp_debug_usart.h"
#include "./stepper/bsp_stepper_init.h"
#include <stdbool.h>

#define FORM_LEN 	   8000

typedef struct {
	__IO uint8_t  status;             /* 状态 */
	__IO uint8_t  dir;                /* 方向 */
	__IO uint32_t pos;                /* 位置 */
  __IO uint16_t pluse_time;         /* 脉冲时间 */
}Stepper_Typedef;

/* S加减速所用到的参数 */
typedef struct {
  __IO int32_t Vo;                  /* 初始速度 */
  __IO int32_t Vt;                  /* 末速度 */
  __IO int32_t AccelTotalStep;      /* 加速总步数 */
  __IO int32_t DecPoint;            /* 开始减速的步数 */
  __IO int32_t TotalStep;           /* 完整曲线总步数 */
	__IO int32_t INC_AccelTotalStep;  /* 加加速度步数 */
	__IO int32_t Dec_AccelTotalStep;  /* 减加速度步数 */
  __IO float   Form[FORM_LEN];/* S加减速 速度表 */
}SpeedCalc_TypeDef;

/* 电机速度决策中的四个状态 */
typedef enum {
  STOP = 0U,                        /* 停止状态 */
  ACCEL,                            /* 加速状态 */
  UNIFORM,                          /* 匀速状态 */
  DECEL,                            /* 减速状态 */
}StateEnum_TypeDef;

/*频率相关参数*/
#define T1_FREQ               (SystemCoreClock / TIM_PRESCALER)//频率ft值
/*电机单圈参数*/
#define STEP_ANGLE						1.8f									           //步进电机的步距角 单位：度
#define FSPR              		(360.0f / 1.8f)                  //步进电机的一圈所需脉冲数
			
#define MICRO_STEP        		32          				             //细分器细分数 
#define SPR               		(FSPR * MICRO_STEP)              //细分后一圈所需脉冲数

#define CONVER(speed)         (float)(speed * SPR / 60.0f)     //根据电机转速（r/min），计算电机步速（step/s）
  
#define MIN_SPEED             (float)(T1_FREQ / 65535U)        //最低频率/速度

extern Stepper_Typedef Stepper;

bool Stepper_Move_S(int16_t start_speed, int16_t end_speed, float acc_time, int32_t step);

#endif 
