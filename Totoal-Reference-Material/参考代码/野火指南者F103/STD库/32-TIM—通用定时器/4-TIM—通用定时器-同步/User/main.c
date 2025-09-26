
// 通用定时器TIMx,x[2,3,4]同步应用
#include "stm32f10x.h"
#include "./tim/bsp_general_tim.h"


/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{
  /* 初始化通用定时器级联同步PWM输出 */
	/* 实现：TIM2发生事件更新时发送触发信号驱动TIM3计数 */
	/*       TIM3发生事件更新时发送触发信号驱动TIM4计数 */
	TIMx_Configuration();
	
  while(1)
  {
        
  }
}
/*********************************************END OF FILE**********************/

