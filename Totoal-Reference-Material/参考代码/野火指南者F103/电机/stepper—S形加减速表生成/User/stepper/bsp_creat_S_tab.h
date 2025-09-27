#ifndef __BSP_CREAT_S_TAB_H
#define	__BSP_CREAT_S_TAB_H

#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"
#include "./usart/bsp_debug_usart.h"
#include "math.h"
#include "stdlib.h"
#include "string.h"

#define FORM_LEN 	   1000
#define MICRO_STEP     32    // 驱动器细分数
#define FSPR           200   // 步进电机单圈步数
#define CONVER(speed)  ((speed) * FSPR * MICRO_STEP / 60)  // 根据电机转速（r/min），计算电机步速（step/s）


typedef struct 
{
	int		Vo;  //初速度 单位 Step/s
	int		Vt;  //末速度 单位 Step/s
	int		AccelHalfStep;   //加速阶段半路程	单位 Step   
	int		AccelTotalStep;  //总路程，加速阶段总步数 Step
	float   Form[FORM_LEN]; // 速度表格 单位 Step/s
	
}Speed_s;


extern uint8_t print_flag;
void CalculateSpeedTab(int Vo, int Vt, float Time);

#endif 
