/**
  *********************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   RT-Thread 3.0 + 指南者 工程模版
  *********************************************************************
  * @attention
  *
  * 实验平台:野火 F103-MINI STM32 开发板 
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
*************************************************************************
*                               变量
*************************************************************************
*/
/* 定义线程控制块 */
static struct rt_thread led1_thread;
static struct rt_thread led2_thread;

/* 定义线程控栈时要求RT_ALIGN_SIZE个字节对齐 */
ALIGN(RT_ALIGN_SIZE)
/* 定义线程栈 */
static rt_uint8_t rt_led1_thread_stack[1024];
static rt_uint8_t rt_led2_thread_stack[1024];
/*
*************************************************************************
*                             函数声明
*************************************************************************
*/
static void led1_thread_entry(void* parameter);
static void led2_thread_entry(void* parameter);


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
	
	rt_thread_init(&led1_thread,                 /* 线程控制块 */
                   "led1",                       /* 线程名字 */
                   led1_thread_entry,            /* 线程入口函数 */
                   RT_NULL,                      /* 线程入口函数参数 */
                   &rt_led1_thread_stack[0],     /* 线程栈起始地址 */
                   sizeof(rt_led1_thread_stack), /* 线程栈大小 */
                   3,                            /* 线程的优先级 */
                   20);                          /* 线程时间片 */
    rt_thread_startup(&led1_thread);             /* 启动线程，开启调度 */
                   
	rt_thread_init(&led2_thread,                 /* 线程控制块 */
                   "led2",                       /* 线程名字 */
                   led2_thread_entry,            /* 线程入口函数 */
                   RT_NULL,                      /* 线程入口函数参数 */
                   &rt_led2_thread_stack[0],     /* 线程栈起始地址 */
                   sizeof(rt_led2_thread_stack), /* 线程栈大小 */
                   4,                            /* 线程的优先级 */
                   20);                          /* 线程时间片 */
    rt_thread_startup(&led2_thread);             /* 启动线程，开启调度 */                   
}

/*
*************************************************************************
*                             线程定义
*************************************************************************
*/

static void led1_thread_entry(void* parameter)
{	
    while (1)
    {
        LED1_ON;
        rt_thread_delay(500);   /* 延时500个tick */
        
        LED1_OFF;     
        rt_thread_delay(500);   /* 延时500个tick */		 		

    }
}

static void led2_thread_entry(void* parameter)
{	
    while (1)
    {
        LED2_ON;
        rt_thread_delay(300);   /* 延时300个tick */
        
        LED2_OFF;     
        rt_thread_delay(300);   /* 延时300个tick */		 		

    }
}
/********************************END OF FILE****************************/
