/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTI
  
  AL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "los_sem.h"
#include "los_queue.h"
/* 开发板硬件bsp头文件 */
#include "bsp_led.h"
#include "bsp_usart.h"
#include "bsp_key.h"
#include "bsp_exti.h"

 /* 定义消息队列的句柄 */
extern UINT32 Test_Queue_Handle;

static uint32_t send_data1 = 1;
static uint32_t send_data2 = 2;

/*********************************************************************************
  * @ 函数名  ： KEY1_IRQHandler
  * @ 功能说明： 中断服务函数
  * @ 参数    ： 无  
  * @ 返回值  ： 无
  ********************************************************************************/
void KEY1_IRQHandler(void)
{
	UINTPTR uvIntSave;
	
	uvIntSave = LOS_IntLock();		//关中断
	
  //确保是否产生了EXTI Line中断
	if(EXTI_GetITStatus(KEY1_INT_EXTI_LINE) != RESET) 
	{
			/* 将数据写入（发送）到队列中，等待时间为 0  */
			LOS_QueueWrite(	Test_Queue_Handle,	/* 写入（发送）队列的ID(句柄) */
											&send_data1,				/* 写入（发送）的数据 */
											sizeof(send_data1),	/* 数据的长度 */
											0);									/* 等待时间为 0  */
		//清除中断标志位
		EXTI_ClearITPendingBit(KEY1_INT_EXTI_LINE);     
	}  
    
	LOS_IntRestore(uvIntSave);	//开中断
}
/*********************************************************************************
  * @ 函数名  ： KEY1_IRQHandler
  * @ 功能说明： 中断服务函数
  * @ 参数    ： 无  
  * @ 返回值  ： 无
  ********************************************************************************/
void KEY2_IRQHandler(void)
{
	UINTPTR uvIntSave;
	
	uvIntSave = LOS_IntLock();		//关中断
	
  //确保是否产生了EXTI Line中断
	if(EXTI_GetITStatus(KEY2_INT_EXTI_LINE) != RESET) 
	{
		/* 将数据写入（发送）到队列中，等待时间为 0  */
		LOS_QueueWrite(	Test_Queue_Handle,	/* 写入（发送）队列的ID(句柄) */
										&send_data2,				/* 写入（发送）的数据 */
										sizeof(send_data2),	/* 数据的长度 */
										0);									/* 等待时间为 0  */
		//清除中断标志位
		EXTI_ClearITPendingBit(KEY2_INT_EXTI_LINE);     
	}  
		
	LOS_IntRestore(uvIntSave);	//开中断
}

/*********************************************************************************
  * @ 函数名  ： DEBUG_USART_IRQHandler
  * @ 功能说明： 串口中断服务函数
  * @ 参数    ： 无  
  * @ 返回值  ： 无
  ********************************************************************************/
void DEBUG_USART_IRQHandler(void)
{
	UINTPTR uvIntSave;
	
	uvIntSave = LOS_IntLock();		//关中断

	if(USART_GetITStatus(DEBUG_USARTx,USART_IT_IDLE)!=RESET)
	{		
		Uart_DMA_Rx_Data();       /* 释放一个信号量，表示数据已接收 */
		USART_ReceiveData(DEBUG_USARTx); /* 清除标志位 */
	}	 
  
	LOS_IntRestore(uvIntSave);	//开中断
}



