/**
  ******************************************************************************
  * @file    bsp_stepper_T_speed.c
  * @author  fire
  * @version V1.0
  * @date    2020-xx-xx
  * @brief   步进电机梯形加减速算法
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F103 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
#include "./stepper/bsp_stepper_T_speed.h"


//系统加减速参数
speedRampData srd;
//记录步进电机的位置
//int stepPosition = 0;
//系统电机、串口状态
struct GLOBAL_FLAGS status = {FALSE, FALSE,TRUE};


/*! \brief 以给定的步数移动步进电机
 *  通过计算加速到最大速度，以给定的步数开始减速
 *  如果加速度和减速度很小，步进电机会移动很慢，还没达到最大速度就要开始减速
 *  \param step   移动的步数 (正数为顺时针，负数为逆时针).
 *  \param accel  加速度,如果取值为10，实际值为10*0.1*rad/sec^2=1rad/sec^2
 *  \param decel  减速度,如果取值为10，实际值为10*0.1*rad/sec^2=1rad/sec^2
 *  \param speed  最大速度,如果取值为10，实际值为10*0.1*rad/sec=1rad/sec
 */
void stepper_move_T( int32_t step, uint32_t accel, uint32_t decel, uint32_t speed)
{  
    //达到最大速度时的步数.
    unsigned int max_s_lim;
    //必须开始减速的步数(如果还没加速到达最大速度时)。
    unsigned int accel_lim;

	/*根据步数和正负判断*/
	if(step == 0)
	{
			return ;
	}
	else if(step < 0)//逆时针
    {
        srd.dir = CCW;
        step = -step;
    }
    else//顺时针
    {
        srd.dir = CW;
    }	// 输出电机方向
		MOTOR_DIR(srd.dir);
		    
    // 如果只移动一步
    if(step == 1)
    {
        // 只移动一步
        srd.accel_count = -1;
        // 减速状态
        srd.run_state = DECEL;
        // 短延时
        srd.step_delay = 1000;
        // 配置电机为运行状态
        status.running = TRUE;
     }
		
    // 步数不为零才移动
    else if(step != 0)
    {
		// 设置最大速度极限, 计算得到min_delay用于定时器的计数器的值。
		// min_delay = (alpha / tt)/ w
		srd.min_delay = (int32_t)(A_T_x10/speed);

		// 通过计算第一个(c0) 的步进延时来设定加速度，其中accel单位为0.1rad/sec^2
		// step_delay = 1/tt * sqrt(2*alpha/accel)
		// step_delay = ( tfreq*0.676/10 )*10 * sqrt( (2*alpha*100000) / (accel*10) )/100
		srd.step_delay = (int32_t)((T1_FREQ_148 * sqrt(A_SQ / accel))/10);

		// 计算多少步之后达到最大速度的限制
		// max_s_lim = speed^2 / (2*alpha*accel)
		max_s_lim = (uint32_t)(speed*speed/(A_x200*accel/10));
		// 如果达到最大速度小于0.5步，我们将四舍五入为0
		// 但实际我们必须移动至少一步才能达到想要的速度
		if(max_s_lim == 0)
		{
				max_s_lim = 1;
		}

		// 计算多少步之后我们必须开始减速
		// n1 = (n1+n2)decel / (accel + decel)
		accel_lim = (uint32_t)(step*decel/(accel+decel));
		// 我们必须加速至少1步才能才能开始减速.
		if(accel_lim == 0)
		{
			accel_lim = 1;
		}
		// 使用限制条件我们可以计算出第一次开始减速的位置
		//srd.decel_val为负数
		if(accel_lim <= max_s_lim)
		{
			srd.decel_val = accel_lim - step;
		}
		else{
			srd.decel_val = -(max_s_lim*accel/decel);
		}
		// 当只剩下一步我们必须减速
		if(srd.decel_val == 0)
		{
			srd.decel_val = -1;
		}

		// 计算开始减速时的步数
		srd.decel_start = step + srd.decel_val;

		// 如果最大速度很慢，我们就不需要进行加速运动
		if(srd.step_delay <= srd.min_delay)
		{
			srd.step_delay = srd.min_delay;
			srd.run_state = RUN;
		}
		else
		{
			srd.run_state = ACCEL;
		}
		// 复位加速度计数值
		srd.accel_count = 0;
		status.running = TRUE;
	}
	/*获取当前计数值*/
	int tim_count=__HAL_TIM_GET_COUNTER(&TIM_TimeBaseStructure);
	/*在当前计数值基础上设置定时器比较值*/
	__HAL_TIM_SET_COMPARE(&TIM_TimeBaseStructure,MOTOR_PUL_CHANNEL_x,tim_count+srd.step_delay); 
	/*使能定时器通道*/
	TIM_CCxChannelCmd(MOTOR_PUL_TIM, MOTOR_PUL_CHANNEL_x, TIM_CCx_DISABLE);
  __HAL_TIM_ENABLE_IT(&TIM_TimeBaseStructure, TIM_IT_CC1);
  __HAL_TIM_MOE_ENABLE(&TIM_TimeBaseStructure);
  __HAL_TIM_ENABLE(&TIM_TimeBaseStructure);
}

/**
  * @brief  速度决策
	*	@note 	在中断中使用，每进一次中断，决策一次
  * @retval 无
  */
void speed_decision()
{
	uint32_t tim_count=0;
	uint32_t tmp = 0;
	// 保存新（下）一个延时周期
	uint16_t new_step_delay=0;
	// 加速过程中最后一次延时（脉冲周期）.
	static uint16_t last_accel_delay=0;
	// 总移动步数计数器
	static uint32_t step_count = 0;
	static int32_t rest = 0;
	//定时器使用翻转模式，需要进入两次中断才输出一个完整脉冲
	static uint8_t i=0;
  
	if(__HAL_TIM_GET_IT_SOURCE(&TIM_TimeBaseStructure, MOTOR_TIM_IT_CCx) !=RESET)
	{
		// 清楚定时器中断
		__HAL_TIM_CLEAR_IT(&TIM_TimeBaseStructure, MOTOR_TIM_IT_CCx);

		// 设置比较值
		tim_count=__HAL_TIM_GET_COUNTER(&TIM_TimeBaseStructure);
		tmp = tim_count+srd.step_delay/2;
		__HAL_TIM_SET_COMPARE(&TIM_TimeBaseStructure,MOTOR_PUL_CHANNEL_x,tmp);

		i++;     // 定时器中断次数计数值
		if(i==2) // 2次，说明已经输出一个完整脉冲
		{
			i=0;   // 清零定时器中断次数计数值
			switch(srd.run_state) 
			{
				/*步进电机停止状态*/
				case STOP:
				step_count = 0;  // 清零步数计数器
				rest = 0;        // 清零余值
				// 关闭通道
				HAL_TIM_OC_Stop_IT(&TIM_TimeBaseStructure,MOTOR_PUL_CHANNEL_x);


				status.running = FALSE;
				break;
				/*步进电机加速状态*/
				case ACCEL:
				TIM_CCxChannelCmd(MOTOR_PUL_TIM, MOTOR_PUL_CHANNEL_x, TIM_CCx_ENABLE);
				step_count++;
				srd.accel_count++;

				new_step_delay = srd.step_delay - (((2 *srd.step_delay) + rest)/(4 * srd.accel_count + 1));//计算新(下)一步脉冲周期(时间间隔)
				rest = ((2 * srd.step_delay)+rest)%(4 * srd.accel_count + 1);// 计算余数，下次计算补上余数，减少误差
				//检查是够应该开始减速
					if(step_count >= srd.decel_start) {
						srd.accel_count = srd.decel_val;
						srd.run_state = DECEL;
					}
					//检查是否到达期望的最大速度
					else if(new_step_delay <= srd.min_delay) {
						last_accel_delay = new_step_delay;
						new_step_delay = srd.min_delay;    
						rest = 0;                          
						srd.run_state = RUN;
					}
					break;
				/*步进电机最大速度运行状态*/
				case RUN:

					step_count++;
					new_step_delay = srd.min_delay;

					//检查是否需要开始减速
					if(step_count >= srd.decel_start) 
					{
						srd.accel_count = srd.decel_val;
						//以最后一次加速的延时作为开始减速的延时
						new_step_delay = last_accel_delay;
						srd.run_state = DECEL;
						
					}
					break;
				/*步进电机减速状态*/
				case DECEL:

					step_count++;
					srd.accel_count++;
					  new_step_delay = srd.step_delay - (((2 * srd.step_delay) + rest)/(4 * srd.accel_count + 1)); //计算新(下)一步脉冲周期(时间间隔)
					  rest = ((2 * srd.step_delay)+rest)%(4 * srd.accel_count + 1);// 计算余数，下次计算补上余数，减少误差
					//检查是否为最后一步
					if(srd.accel_count >= 0)
					{
						srd.run_state = STOP;
					}
					break;
			}
			/*求得下一次间隔时间*/
			srd.step_delay = new_step_delay;
		}
	}
}

