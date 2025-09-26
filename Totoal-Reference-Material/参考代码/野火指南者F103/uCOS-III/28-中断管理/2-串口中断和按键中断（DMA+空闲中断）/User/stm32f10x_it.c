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
#include <includes.h>
#include "bsp_usart1.h"
#include "bsp_exti.h"


extern OS_TCB	 AppTaskUsartTCB;
extern OS_TCB	 AppTaskKeyTCB;


/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/


/**
  * @brief  USART 中断服务函数
  * @param  无
  * @retval 无
  */	
void macUSART_INT_FUN(void)
{
	OS_ERR   err;
	
	OSIntEnter(); 	                                     //进入中断
	

	if ( USART_GetITStatus ( macUSARTx, USART_IT_IDLE ) != RESET )
	{
    
    DMA_Cmd(USART_RX_DMA_CHANNEL, DISABLE);   

		USART_ReceiveData ( macUSARTx );  /* 清除标志位 */
    
    // 清DMA标志位
    DMA_ClearFlag( DMA1_FLAG_TC5 );          
    //  重新赋值计数值，必须大于等于最大可能接收到的数据帧数目
    USART_RX_DMA_CHANNEL->CNDTR = USART_RBUFF_SIZE;    
    DMA_Cmd(USART_RX_DMA_CHANNEL, ENABLE);     
    
		//给出信号量 ，发送接收到新数据标志，供前台程序查询
    		
    /* 发送任务信号量到任务 AppTaskKey */
		OSTaskSemPost((OS_TCB  *)&AppTaskUsartTCB,   //目标任务
									(OS_OPT   )OS_OPT_POST_NONE, //没选项要求
									(OS_ERR  *)&err);            //返回错误类型		
    
	}
	
	OSIntExit();	                                       //退出中断
	
}


/**
  * @brief  EXTI 中断服务函数
  * @param  无
  * @retval 无
  */
void macEXTI_INT_FUNCTION (void)
{
	OS_ERR   err;
	
	
	OSIntEnter(); 	                             //进入中断
	
	if(EXTI_GetITStatus(macEXTI_LINE) != RESET)  //确保是否产生了EXTI Line中断
	{
		/* 发送任务信号量到任务 AppTaskKey */
		OSTaskSemPost((OS_TCB  *)&AppTaskKeyTCB,   //目标任务
									(OS_OPT   )OS_OPT_POST_NONE, //没选项要求
									(OS_ERR  *)&err);            //返回错误类型		

		EXTI_ClearITPendingBit(macEXTI_LINE);      //清除中断标志位
	}  
	
	OSIntExit();	                               //退出中断
	
}


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
