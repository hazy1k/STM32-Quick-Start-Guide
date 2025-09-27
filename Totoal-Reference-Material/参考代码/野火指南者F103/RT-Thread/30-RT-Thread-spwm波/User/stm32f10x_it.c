/* 该文件统一用于存放中断服务函数 */
#include "stm32f10x_it.h"
#include "./spwm/bsp_spwm.h"

extern uint16_t indexWave[];

extern __IO uint32_t rgb_color;

/* 呼吸灯中断服务函数 */
void BRE_TIMx_IRQHandler(void)
{	
	static uint16_t pwm_index = 0;			//用于PWM查表
	static uint16_t period_cnt = 0;		//用于计算周期数
	static uint16_t amplitude_cnt = 0;	//用于计算幅值等级

	if (TIM_GetITStatus(BRE_TIMx, TIM_IT_Update) != RESET)	//TIM_IT_Update
 	{		
			amplitude_cnt++;
				
			//每个PWM表中的每个元素有AMPLITUDE_CLASS个等级，
		  //每增加一级多输出一次脉冲，即PWM表中的元素多使用一次
		  //使用256次，根据RGB颜色分量设置通道输出
			if(amplitude_cnt > (AMPLITUDE_CLASS-1))		 						
			{		
				period_cnt++;
				
				//每个PWM表中的每个元素使用period_class次
				if(period_cnt > period_class)
				{				

					pwm_index++;												//标志PWM表指向下一个元素
				
					//若PWM表已到达结尾，重新指向表头
					if( pwm_index >=  POINT_NUM)			
					{
						pwm_index=0;								
					}
					
					period_cnt = 0;											//重置周期计数标志
				}
				
				amplitude_cnt=0;											//重置幅值计数标志
			}
			else
			{	
					//每个PWM表中的每个元素有AMPLITUDE_CLASS个等级，
					//每增加一级多输出一次脉冲，即PWM表中的元素多使用一次

					//根据RGB颜色分量值，设置各个通道是否输出当前的PWM表元素表示的亮度
					//红
					if(((rgb_color&0xFF0000)>>16) >= amplitude_cnt)				
						BRE_TIMx->BRE_RED_CCRx = indexWave[pwm_index];	//根据PWM表修改定时器的比较寄存器值
					else
						BRE_TIMx->BRE_RED_CCRx = 0;		//比较寄存器值为0，通道输出高电平，该通道LED灯灭
					
					//绿
					if(((rgb_color&0x00FF00)>>8) >= amplitude_cnt)				
						BRE_TIMx->BRE_GREEN_CCRx = indexWave[pwm_index];	//根据PWM表修改定时器的比较寄存器值
					else
						BRE_TIMx->BRE_GREEN_CCRx = 0;	//比较寄存器值为0，通道输出高电平，该通道LED灯灭
					
					//蓝
					if((rgb_color&0x0000FF) >= amplitude_cnt)				
						BRE_TIMx->BRE_BLUE_CCRx = indexWave[pwm_index];	//根据PWM表修改定时器的比较寄存器值
					else
						BRE_TIMx->BRE_BLUE_CCRx = 0;		//比较寄存器值为0，通道输出高电平，该通道LED灯灭	

			}					
		
		TIM_ClearITPendingBit (BRE_TIMx, TIM_IT_Update);	//必须要清除中断标志位
	}
}
