/**
  *********************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   RT-Thread 3.0 + STM32 软件定时器
  *********************************************************************
  * @attention
  *
  * 实验平台:野火 F103-指南者 STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  **********************************************************************
  */ 

/*
*************************************************************************
*                             包含的头文件
*************************************************************************
*/ 
#include "board.h"
#include "rtthread.h"


/*
******************************************************************
*                               变量
******************************************************************
*/
/* 定义线软件定时器制块 */
static rt_timer_t swtmr1 = RT_NULL;
static rt_timer_t swtmr2 = RT_NULL;
/************************* 全局变量声明 ****************************/
/*
 * 当我们在写应用程序的时候，可能需要用到一些全局变量。
 */
static uint32_t TmrCb_Count1 = 0;
static uint32_t TmrCb_Count2 = 0;
 
/*
*************************************************************************
*                             函数声明
*************************************************************************
*/
static void swtmr1_callback(void* parameter);
static void swtmr2_callback(void* parameter);

/*
*************************************************************************
*                             main 函数
*************************************************************************
*/
/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{	
   
    /* 
	 * 开发板硬件初始化，RTT系统初始化已经在main函数之前完成，
	 * 即在component.c文件中的rtthread_startup()函数中完成了。
	 * 所以在main函数中，只需要创建线程和启动线程即可。
	 */
	rt_kprintf("这是一个[野火]-STM32F103-指南者-RTT软件定时器实验！\n");
	rt_kprintf("定时器回调函数1只执行一次就被销毁\n");
	rt_kprintf("定时器回调函数2则循环执行\n");
  /* 创建一个软件定时器 */
	swtmr1 = rt_timer_create("swtmr1_callback", /* 软件定时器的名称 */
                        swtmr1_callback,/* 软件定时器的回调函数 */
                        0,			/* 定时器超时函数的入口参数 */
                        5000,   /* 软件定时器的超时时间(周期回调时间) */
                        RT_TIMER_FLAG_ONE_SHOT | RT_TIMER_FLAG_SOFT_TIMER);
                        /* 软件定时器模式 一次模式 */
  /* 启动定时器 */
  if (swtmr1 != RT_NULL) 
      rt_timer_start(swtmr1);
  
     /* 创建一个软件定时器 */
	swtmr2 = rt_timer_create("swtmr2_callback", /* 软件定时器的名称 */
                        swtmr2_callback,/* 软件定时器的回调函数 */
                        0,			/* 定时器超时函数的入口参数 */
                        1000,   /* 软件定时器的超时时间(周期回调时间) */
                        RT_TIMER_FLAG_PERIODIC | RT_TIMER_FLAG_SOFT_TIMER);
                        /* 软件定时器模式 周期模式 */
  /* 启动定时器 */
  if (swtmr2 != RT_NULL) 
      rt_timer_start(swtmr2);
}

/*
*************************************************************************
*                             线程定义
*************************************************************************
*/

static void swtmr1_callback(void* parameter)
{		
    uint32_t tick_num1;

    TmrCb_Count1++;						/* 每回调一次加一 */

    tick_num1 = (uint32_t)rt_tick_get();	/* 获取滴答定时器的计数值 */
	
    rt_kprintf("swtmr1_callback函数执行 %d 次\n", TmrCb_Count1);
    rt_kprintf("滴答定时器数值=%d\n", tick_num1);
}

static void swtmr2_callback(void* parameter)
{	
    uint32_t tick_num2;
	
    TmrCb_Count2++;				/* 每回调一次加一 */
	
    tick_num2 = (uint32_t)rt_tick_get();	/* 获取滴答定时器的计数值 */
	
    rt_kprintf("swtmr2_callback函数执行 %d 次\n", TmrCb_Count2);
	
    rt_kprintf("滴答定时器数值=%d\n", tick_num2);
}




/********************************END OF FILE****************************/
