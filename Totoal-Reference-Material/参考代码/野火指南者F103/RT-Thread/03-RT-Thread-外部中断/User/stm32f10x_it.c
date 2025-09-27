/* 该文件统一用于存放中断服务函数 */
#include "stm32f10x_it.h"
#include "board.h"
#include "rtthread.h"

/* 外部定义消息队列控制块 */
extern rt_mq_t test_mq;

uint32_t send_data1 = 1;
uint32_t send_data2 = 2;
/*********************************************************************************
  * @ 函数名  ： KEY1_IRQHandler
  * @ 功能说明： 中断服务函数
  * @ 参数    ： 无  
  * @ 返回值  ： 无
  ********************************************************************************/
void KEY1_IRQHandler(void)
{
  //确保是否产生了EXTI Line中断
	if(EXTI_GetITStatus(KEY1_INT_EXTI_LINE) != RESET) 
	{
    /* 将数据写入（发送）到队列中，等待时间为 0  */
    rt_mq_send(	test_mq,	/* 写入（发送）队列的ID(句柄) */
                &send_data1,			/* 写入（发送）的数据 */
                sizeof(send_data1));	/* 数据的长度 */
		//清除中断标志位
		EXTI_ClearITPendingBit(KEY1_INT_EXTI_LINE);     
	}  
}
/*********************************************************************************
  * @ 函数名  ： KEY1_IRQHandler
  * @ 功能说明： 中断服务函数
  * @ 参数    ： 无  
  * @ 返回值  ： 无
  ********************************************************************************/
void KEY2_IRQHandler(void)
{
  //确保是否产生了EXTI Line中断
	if(EXTI_GetITStatus(KEY2_INT_EXTI_LINE) != RESET) 
	{
    /* 将数据写入（发送）到队列中，等待时间为 0  */
    rt_mq_send(	test_mq,	/* 写入（发送）队列的ID(句柄) */
                &send_data2,			/* 写入（发送）的数据 */
                sizeof(send_data2));			/* 数据的长度 */
		//清除中断标志位
		EXTI_ClearITPendingBit(KEY2_INT_EXTI_LINE);     
	}  
}

