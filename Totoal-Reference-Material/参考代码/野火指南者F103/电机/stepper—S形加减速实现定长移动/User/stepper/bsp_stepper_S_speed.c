/**
  ******************************************************************************
  * @file    bsp_stepper_S_speed.c
  * @author  fire
  * @version V1.0
  * @date    2020-xx-xx
  * @brief   S形加减速
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F103 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
#include "./stepper/bsp_stepper_S_speed.h"
#include <math.h>
#include <string.h>

/*

在这里采用的是数学匀变速的数学模型，但是速度也并

不是匀速变化的，所以准确的说是加速度的匀变数学模型。

a-t 曲线如下 （V-t曲线 请看 文档）

	|    .
 a|   /|\             
	|  / | \
	| /  |  \            
	|/___|___\_t 		
	0   t/2
		
以上就是加速度的匀变模型（默认初始的加速度为0）

那么有了这个模型就很容易可以算出一些不为人知的秘密！！！

首先有了以上的模型就可以算出每一时刻的加速度，所以先看 

0~t/2 这段 a=K*T ，其中K为加速度a的斜率

将加速度积分就是速度，所以 V = a dt = K*T dt

得： V = K*T^2/2

将速度积分就是位移，所以 S = V dt = K*T^2/2 dt 

得： S = K*T^3/6

*/

/* 算法相关结构体变量定义 */
SpeedCalc_TypeDef Speed;
Stepper_Typedef Stepper = {0};

/**
  * @brief  S曲线速度表计算
  * @param  pScurve：S曲线结构体指针
  * @param  Vo：初速度
  * @param  Vt：末速度
  * @param  T：加速时间
  * @retval 0：正常，1：参数错误
	*	@note 	无
  */
static bool CalcSpeed(int32_t Vo, int32_t Vt, float T)
{
  int32_t i = 0;
  float Vm =0.0f;                // 中间点速度
  float K = 0.0f;                // 加加速度
  float Ti = 0.0f;               // 时间间隔 dt
  float Sumt = 0.0f;             // 时间累加量
  float DeltaV = 0.0f;           // 速度的增量dv
  float temp = 0.0f;             // 中间变量

	/***************************************************************************/
	/* 计算初、末速度 */
  Speed.Vo = CONVER(Vo);
  Speed.Vt = CONVER(Vt);
  
	/***************************************************************************/
	/* 计算初始参数 */
	T = T / 2.0f;						//加加速段的时间（加速度斜率>0的时间）
	
  Vm = (Speed.Vo + Speed.Vt) / 2.0f;	//计算中点的速度

  K = fabsf((2.0f * (Vm - Speed.Vo)) / (T * T));// 根据中点速度计算加加速度
		
  Speed.INC_AccelTotalStep = (int32_t)((Speed.Vo * T) + ((K * T * T* T) / 6.0f));// 加加速需要的步数
	
  Speed.Dec_AccelTotalStep = (int32_t)(((Speed.Vt + Speed.Vo) * T - Speed.INC_AccelTotalStep));// 减加速需要的步数 S = Vt * Time - S1
  
	/***************************************************************************/
	/* 计算共需要的步数，并校检内存大小，申请内存空间存放速度表 */
  Speed.AccelTotalStep = Speed.Dec_AccelTotalStep + Speed.INC_AccelTotalStep;              // 加速需要的步数 
  if( Speed.AccelTotalStep  % 2 != 0)     // 由于浮点型数据转换成整形数据带来了误差,所以这里加1
    Speed.AccelTotalStep  += 1;
	
	/* 判断内存长度 */
	if(FORM_LEN < Speed.AccelTotalStep)
	{
		printf("FORM_LEN 缓存长度不足\r\n，请将 FORM_LEN 修改为 %d \r\n", Speed.AccelTotalStep);
		return false;
	}

	/***************************************************************************/
	/* 计算第一步的时间 */
		
	/* 根据第一步的时间计算，第一步的速度和脉冲时间间隔 */
	/* 根据位移为0的时候的情况，计算时间的关系式 -> 根据位移和时间的公式S = 1/2 * K * Ti^3  可得 Ti=6 * 1 / K开1/3次方 */
  Ti = pow(6.0f * 1.0f / K, 1.0f / 3.0f); //开方求解 Ti 时间常数
  Sumt += Ti;//累计时间常数
	/* 根据V=1/2*K*T^2,可以计算第一步的速度 */
  DeltaV = 0.5f * K * Sumt * Sumt;
	/* 在初始速度的基础上增加速度 */
  Speed.Form[0] = Speed.Vo + DeltaV;
  
	/***************************************************************************/
	/* 最小速度限幅 */
  if(Speed.Form[0] <= MIN_SPEED)//以当前定时器频率所能达到的最低速度
    Speed.Form[0] = MIN_SPEED;
	
  /***************************************************************************/
	/* 计算S形速度表 */
  for(i = 1; i < Speed.AccelTotalStep; i++)
  {
		/* 根据时间周期与频率成反比的关系，可以计算出Ti,在这里每次计算上一步时间，用于积累到当前时间 */
		Ti = 1.0f / Speed.Form[i-1];
    if(i < Speed.INC_AccelTotalStep)
    {
			/* 累积时间 */
      Sumt += Ti;
			/* 速度的变化量 dV = 1/2 * K * Ti^2 */
      DeltaV = 0.5f * K * Sumt * Sumt;
			/* 根据初始速度和变化量求得速度表 */
      Speed.Form[i] = Speed.Vo + DeltaV;
			/* 为了保证在最后一步可以使得时间严谨的与预期计算的时间一致，在最后一步进行处理 */
      if(i == Speed.INC_AccelTotalStep - 1)
        {Sumt  = fabsf(Sumt - T);}
    }
    /* 减加速度计算 */
    else
    {
			/* 时间累积 */
      Sumt += Ti;
			/* 计算速度 */
      temp = fabsf(T - Sumt);
      DeltaV = 0.5f * K * temp * temp;
      Speed.Form[i] = Speed.Vt - DeltaV;
    }
  }
  return true;
}


/**
  * @brief  速度决策
  * @param  pScurve：S曲线结构体指针
  * @retval 无
	*	@note 	在中断中使用，每进一次中断，决策一次
  */
void Speed_Decision(void)
{
	/* 脉冲计数 */
  static __IO uint8_t i = 0;
  static __IO uint32_t index = 0;
  
	if(__HAL_TIM_GET_IT_SOURCE(&TIM_TimeBaseStructure, MOTOR_TIM_IT_CCx) != RESET)
	{
		/* 清除定时器中断 */
		__HAL_TIM_CLEAR_IT(&TIM_TimeBaseStructure, MOTOR_TIM_IT_CCx);
		
		/******************************************************************/
		/* 两次为一个脉冲周期 */
		i++;
    if(i == 2)
    {
			/* 脉冲周期完整后清零 */
      i = 0;
      /* 判断当前的状态 */
      switch(Stepper.status)
      {
        case ACCEL:
          if(Stepper.pos >= (Speed.AccelTotalStep - 1))
          {
            Stepper.status = UNIFORM;
            index -= 1;
            break;
          }
          /* 获取每一步的定时器计数值 */
          Stepper.pluse_time = (uint16_t)(T1_FREQ / Speed.Form[index] / 2U);
          /* 步数位置索引递增 */
          index++;
          break;
        case DECEL:
          if(Stepper.pos >= (Speed.TotalStep - 1))
          {
            /* 进入停止状态，清空速度表并且关闭输出通道 */
            HAL_TIM_OC_Stop_IT(&TIM_TimeBaseStructure, MOTOR_PUL_CHANNEL_x);
            memset((void*)Speed.Form, 0, sizeof(float) * FORM_LEN);
            index = 0;
            Stepper.status = STOP;
            break;
          }
          /* 获取每一步的定时器计数值 */
          Stepper.pluse_time = (uint16_t)(T1_FREQ / Speed.Form[index] / 2U);
          /* 步数位置索引递增 */
          index--;
          break;
        case UNIFORM:
          if(Stepper.pos >= Speed.DecPoint)
          {
            Stepper.status = DECEL;
          }
          break;
      }
      /* 步数位置索引递增 */
      Stepper.pos++;
    }
		/**********************************************************************/
		/* 获取当前计数器数值 */
		uint32_t tim_count = __HAL_TIM_GET_COUNTER(&TIM_TimeBaseStructure);
		/* 计算下一次时间 */
		uint16_t tmp = tim_count + Stepper.pluse_time;
		/* 设置比较值 */
		__HAL_TIM_SET_COMPARE(&TIM_TimeBaseStructure, MOTOR_PUL_CHANNEL_x, tmp);
	}
}

/**
  * @brief  步进电机S曲线加减速
  * @param  axis：想要控制的轴编号
  * @param  start_speed：启动速度，单位：转/分钟
	* @param  end_speed：目标速度，单位：转/分钟
	* @param  acc_time：加速时间，单位：秒
	* @param  step：运动步数，单位：步（需考虑细分）
  * @retval true：正常
  * @retval false：参数设置错误或速度表空间不足
	*	@note   无
  */
bool Stepper_Move_S(int16_t start_speed, int16_t end_speed, float acc_time, int32_t step)
{
  /* 判断是否正在运动 */
  if(Stepper.status != STOP)
    return false;
  
	/* 计算参数 */
	if(CalcSpeed(start_speed, end_speed, acc_time) != true)
    return false;
  
  if(step < 0)
  {
    step = -step;
    MOTOR_DIR(CCW);
  }
  else
  {
    MOTOR_DIR(CW);
  }
  
  /* 计算减速点，当运动步数小于两倍加速段时无法完成s加减速 */
  if(step >= Speed.AccelTotalStep * 2)
  {
    Speed.TotalStep = step;
    Speed.DecPoint = Speed.TotalStep - Speed.AccelTotalStep;
  }
  else
  {
    printf("加减速参数设置错误！\r\n");
    return false;
  }
  
	/* 初始化结构体 */
  memset(&Stepper, 0, sizeof(Stepper_Typedef));
  
  /* 初始化电机状态 */
	Stepper.status = ACCEL;
	Stepper.pos = 0;
  
  /* 计算第一步的定时器参数 */
  Stepper.pluse_time = (uint16_t)(T1_FREQ / Speed.Form[0] / 2U);

  /* 清零计数器 */
  __HAL_TIM_SET_COUNTER(&TIM_TimeBaseStructure, 0);
  __HAL_TIM_SET_COMPARE(&TIM_TimeBaseStructure, MOTOR_PUL_CHANNEL_x, Stepper.pluse_time);
  /* 使能定时器通道 */
	TIM_CCxChannelCmd(MOTOR_PUL_TIM, MOTOR_PUL_CHANNEL_x, TIM_CCx_ENABLE);
	/* 开启输出比较中断 */
	HAL_TIM_OC_Start_IT(&TIM_TimeBaseStructure, MOTOR_PUL_CHANNEL_x);
  
  return true;
}

/**
  * @brief  定时器中断服务函数
  * @param  无
  * @retval 无
	*	@note   无
  */
void MOTOR_PUL_IRQHandler(void)
{
  Speed_Decision();
}
