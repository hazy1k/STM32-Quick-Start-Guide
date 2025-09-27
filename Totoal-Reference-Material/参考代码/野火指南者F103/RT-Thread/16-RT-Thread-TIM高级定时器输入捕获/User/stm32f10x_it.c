/* 该文件统一用于存放中断服务函数 */
#include "stm32f10x_it.h"
#include "board.h" 

__IO uint16_t IC2Value = 0;
__IO uint16_t IC1Value = 0;
__IO float DutyCycle = 0;
__IO float Frequency = 0;

uint32_t num = 0;

/*
 * 如果是第一个上升沿中断，计数器会被复位，锁存到CCR1寄存器的值是0，CCR2寄存器的值也是0
 * 无法计算频率和占空比。当第二次上升沿到来的时候，CCR1和CCR2捕获到的才是有效的值。其中
 * CCR1对应的是周期，CCR2对应的是占空比。
 */
void ADVANCE_TIM_IRQHandler(void)
{
  /* 清除中断标志位 */
  TIM_ClearITPendingBit(ADVANCE_TIM, TIM_IT_CC1);

  /* 获取输入捕获值 */
  IC1Value = TIM_GetCapture1(ADVANCE_TIM);
  IC2Value = TIM_GetCapture2(ADVANCE_TIM);
	
  // 注意：捕获寄存器CCR1和CCR2的值在计算占空比和频率的时候必须加1
	if (IC1Value != 0)
  {
    /* 占空比计算 */
    DutyCycle = (float)((IC2Value+1) * 100) / (IC1Value+1);

    /* 频率计算 */
    Frequency = (72000000/(ADVANCE_TIM_PSC+1))/(float)(IC1Value+1);
		num++;
		if(num>5000)
		{
			printf("占空比：%0.2f%%   频率：%0.2fHz\n",DutyCycle,Frequency);
			num = 0;
		}
  }
  else
  {
    DutyCycle = 0;
    Frequency = 0;
  }
}
