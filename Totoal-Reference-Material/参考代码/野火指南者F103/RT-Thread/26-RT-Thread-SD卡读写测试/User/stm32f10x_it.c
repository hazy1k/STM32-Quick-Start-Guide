/* 该文件统一用于存放中断服务函数 */
#include "stm32f10x_it.h"
#include "board.h" 
/* RT-Thread相关头文件 */
#include <rthw.h>
#include <rtthread.h>
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



