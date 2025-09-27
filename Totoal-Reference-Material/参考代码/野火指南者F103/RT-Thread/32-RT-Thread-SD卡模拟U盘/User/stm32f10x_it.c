/* 该文件统一用于存放中断服务函数 */
#include "stm32f10x_it.h"
#include "./sdio/bsp_sdio_sdcard.h"	

#include "usb_lib.h"
#include "usb_istr.h"

#include "board.h"
#include "rtthread.h"

/*
 * 函数名：SDIO_IRQHandler
 * 描述  ：在SDIO_ITConfig(）这个函数开启了sdio中断	，
 			数据传输结束时产生中断
 * 输入  ：无		 
 * 输出  ：无
 */
void SDIO_IRQHandler(void) 
{
  /* Process All SDIO Interrupt Sources */
  /* 进入中断 */
	rt_interrupt_enter();

  SD_ProcessIRQSrc();

	/* 离开中断 */
	rt_interrupt_leave();
}

/*
 * 函数名：USB_LP_CAN1_RX0_IRQHandler
 * 描述  ：USB低优先级中断请求
 * 输入  ：无		 
 * 输出  ：无
 */
void USB_LP_CAN1_RX0_IRQHandler(void)
{
  /* 进入中断 */
	rt_interrupt_enter();

  USB_Istr();

	/* 离开中断 */
	rt_interrupt_leave();
}

/*
 * 函数名：USB_HP_CAN1_TX_IRQHandler
 * 描述  ：USB高优先级中断请求 或发送中断
 * 输入  ：无		 
 * 输出  ：无
 */
void USB_HP_CAN1_TX_IRQHandler(void)
{
  /* 进入中断 */
	rt_interrupt_enter();

  CTR_HP();
		
	/* 离开中断 */
	rt_interrupt_leave();
}
