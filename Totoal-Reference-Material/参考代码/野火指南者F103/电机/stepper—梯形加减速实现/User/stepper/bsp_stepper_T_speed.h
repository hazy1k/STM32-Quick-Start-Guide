#ifndef __BSP_STEPPER_T_SPEED_H
#define	__BSP_STEPPER_T_SPEED_H

#include "stm32f1xx.h"
#include "./stepper/bsp_stepper_init.h"
#include "math.h"

/*梯形加减速相关变量*/
typedef struct 
{
	/*当前电机状态*/
	uint8_t  run_state ; 
	/*旋转方向*/
	uint8_t  dir ;    
	/*脉冲间隔*/
	int  step_delay;  
	/*减速位置*/
	int  decel_start; 
	/*减速步数*/
	int  decel_val;   
	/*最小间隔*/
	int  min_delay;   
	/*加速步数*/
	int  accel_count; 
}speedRampData;

/*/系统状态*/
struct GLOBAL_FLAGS 
{
	//当步进电机正在运行时，值为1
	unsigned char running:1;
	//当串口接收到数据时，值为1
	unsigned char cmd:1;
	//当驱动器正常输出时,值为1
	unsigned char out_ena:1;
};

#define ACCEL_R(x)	ceil(x)//向上取整
#define DECEL_R(x)	floor(x)//向下取整

#define FALSE             0
#define TRUE              1


/*电机速度决策中的四个状态*/
#define STOP              0 // 停止状态
#define ACCEL             1 // 加速状态
#define DECEL             2 // 减速状态
#define RUN               3 // 匀速状态

/*频率相关参数*/
//定时器实际时钟频率为：168MHz/(TIM_PRESCALER+1)
//其中 高级定时器的 频率为168MHz,其他定时器为84MHz
//168/(5+1)=28Mhz
//具体需要的频率可以自己计算
#define TIM_PRESCALER      3 
#define T1_FREQ           (SystemCoreClock/(TIM_PRESCALER+1)) // 频率ft值


/*电机单圈参数*/
#define STEP_ANGLE				1.8									//步进电机的步距角 单位：度
#define FSPR              (360.0/1.8)         //步进电机的一圈所需脉冲数

#define MICRO_STEP        32          				//细分器细分数 
#define SPR               (FSPR*MICRO_STEP)   //细分后一圈所需脉冲数

/*数学常数,用于简化计算*/

#define ALPHA             ((float)(2*3.14159/SPR))       // α= 2*pi/spr
#define A_T_x10           ((float)(10*ALPHA*T1_FREQ))
#define T1_FREQ_148       ((float)((T1_FREQ*0.676)/10)) // 0.69为误差修正值(计算过程，文档中有写)
#define A_SQ              ((float)(2*100000*ALPHA)) 
#define A_x200            ((float)(200*ALPHA))

extern void speed_decision(void);

#endif
