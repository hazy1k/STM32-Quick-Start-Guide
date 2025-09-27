#ifndef __BSP_STEPPER_S_SPEED_H
#define	__BSP_STEPPER_S_SPEED_H

#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"
#include "./usart/bsp_debug_usart.h"
#include "./stepper/bsp_stepper_init.h"
#include "math.h"
#include "stdlib.h"
#include "string.h"


#define CONVER(speed)  ((speed) * FSPR * MICRO_STEP / 60)  // 根据电机转速（r/min），计算电机步速（step/s）

#define FORM_LEN 	   8000

typedef struct {
	uint8_t 	status;			/*状态*/
	uint8_t 	dir;				/*方向*/
	uint32_t 	pos;				/*位置*/
	uint32_t  pluse_time; /*脉冲时间*/
}Stepper_Typedef;

/*S加减速所用到的参数*/
typedef struct {
  int32_t   Vo;                	/*初始速度*/
  int32_t   Vt;               	/*末速度*/
  int32_t 	AccelTotalStep;   	/*加速总步数*/  
	int32_t		INC_AccelTotalStep;	/*加加速度步数*/
	int32_t 	Dec_AccelTotalStep;	/*减加速度步数*/
  float   	Form[FORM_LEN];     /*S加减速 速度表*/ 
}SpeedCalc_TypeDef;

extern SpeedCalc_TypeDef Speed ;

/*电机速度决策中的四个状态*/
#define ACCEL                 1   //  加速状态
#define AVESPEED              2   //  匀速状态
#define DECEL                 3   //  减速状态
#define STOP                  0   //  停止状态
																												  

/*频率相关参数*/
//定时器实际时钟频率为：168MHz/(TIM_PRESCALER+1)
//其中 高级定时器的 频率为168MHz,其他定时器为84MHz
//168/(168)=1Mhz
//具体需要的频率可以自己计算
#define TIM_PRESCALER         72 
#define T1_FREQ               (SystemCoreClock/(TIM_PRESCALER+1)) // 频率ft值


/*电机单圈参数*/
#define STEP_ANGLE						1.8									//步进电机的步距角 单位：度
#define FSPR              		(360.0f/1.8f)         //步进电机的一圈所需脉冲数
			
#define MICRO_STEP        		32          				//细分器细分数 
#define SPR               		(FSPR*MICRO_STEP)   //细分后一圈所需脉冲数

#define CONVER(speed)  ((speed) * FSPR * MICRO_STEP / 60)  // 根据电机转速（r/min），计算电机步速（step/s）

#define TRUE                  1
#define FALSE                 0

#define MIN_SPEED                              (T1_FREQ / (65535.0f))// 最低频率/速度

extern uint8_t print_flag;
void CalcSpeed(int32_t Vo, int32_t Vt, float T);
void stepper_move_S(int start_speed,int end_speed,float time);
void speed_decision(void);
#endif 
