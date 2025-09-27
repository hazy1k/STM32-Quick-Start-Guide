/**
  *********************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   RT-Thread 3.0 + STM32 动态内存管理
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
#include <string.h>

/*
******************************************************************
*                               变量
******************************************************************
*/
/* 定义线程控制块 */
static rt_thread_t key_thread = RT_NULL;
static rt_thread_t usart_thread = RT_NULL;
/* 定义消息队列控制块 */
rt_mq_t test_mq = RT_NULL;
/* 定义信号量控制块 */
rt_sem_t test_sem = RT_NULL;

/************************* 全局变量声明 ****************************/
/*
 * 当我们在写应用程序的时候，可能需要用到一些全局变量。
 */

/* 相关宏定义 */
extern char Usart_Rx_Buf[USART_RBUFF_SIZE];

/*
*************************************************************************
*                             函数声明
*************************************************************************
*/
static void key_thread_entry(void* parameter);
static void usart_thread_entry(void* parameter);

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
	rt_kprintf("这是一个[野火]-STM32F103-指南者-RTT中断管理实验！\n");
  rt_kprintf("按下KEY1 | KEY2触发中断！\n");
  rt_kprintf("串口发送数据触发中断,任务处理数据!\n");
  /* 创建一个消息队列 */
	test_mq = rt_mq_create("test_mq",/* 消息队列名字 */
                     4,     /* 消息的最大长度 */
                     2,    /* 消息队列的最大容量 */
                     RT_IPC_FLAG_FIFO);/* 队列模式 FIFO(0x00)*/
  if (test_mq != RT_NULL)
    rt_kprintf("消息队列创建成功！\n\n");
  
  /* 创建一个信号量 */
	test_sem = rt_sem_create("test_sem",/* 消息队列名字 */
                     0,     /* 信号量初始值，默认有一个信号量 */
                     RT_IPC_FLAG_FIFO); /* 信号量模式 FIFO(0x00)*/
  if (test_sem != RT_NULL)
    rt_kprintf("信号量创建成功！\n\n");
  
  /* 创建一个任务 */
	key_thread =                          /* 线程控制块指针 */
    rt_thread_create( "key",              /* 线程名字 */
                      key_thread_entry,   /* 线程入口函数 */
                      RT_NULL,             /* 线程入口函数参数 */
                      512,                 /* 线程栈大小 */
                      1,                   /* 线程的优先级 */
                      20);                 /* 线程时间片 */
                   
    /* 启动线程，开启调度 */
   if (key_thread != RT_NULL)
        rt_thread_startup(key_thread);
    else
        return -1;
    
  usart_thread =                          /* 线程控制块指针 */
    rt_thread_create( "usart",              /* 线程名字 */
                      usart_thread_entry,   /* 线程入口函数 */
                      RT_NULL,             /* 线程入口函数参数 */
                      512,                 /* 线程栈大小 */
                      2,                   /* 线程的优先级 */
                      20);                 /* 线程时间片 */
                   
    /* 启动线程，开启调度 */
   if (usart_thread != RT_NULL)
        rt_thread_startup(usart_thread);
    else
        return -1;
}

/*
*************************************************************************
*                             线程定义
*************************************************************************
*/

static void key_thread_entry(void* parameter)
{		
  rt_err_t uwRet = RT_EOK;	
  uint32_t r_queue;
  /* 任务都是一个无限循环，不能返回 */
  while(1)
	{
    /* 队列读取（接收），等待时间为一直等待 */
		uwRet = rt_mq_recv(test_mq,	/* 读取（接收）队列的ID(句柄) */
								&r_queue,			/* 读取（接收）的数据保存位置 */
								sizeof(r_queue),		/* 读取（接收）的数据的长度 */
								RT_WAITING_FOREVER); 	/* 等待时间：一直等 */
		if(RT_EOK == uwRet)
		{
			rt_kprintf("触发中断的是KEY%d!\n",r_queue);
		}
		else
		{
			rt_kprintf("数据接收出错,错误代码: 0x%lx\n",uwRet);
		}
    LED1_TOGGLE;
  }
}

static void usart_thread_entry(void* parameter)
{
  rt_err_t uwRet = RT_EOK;	
    /* 任务都是一个无限循环，不能返回 */
  while (1)
  {
		uwRet = rt_sem_take(test_sem,	/* 获取串口中断的信号量 */
                        RT_WAITING_FOREVER); 	  /* 等待时间：0 */
    if(RT_EOK == uwRet)
    {
      rt_kprintf("收到数据:%s\n",Usart_Rx_Buf);
      memset(Usart_Rx_Buf,0,USART_RBUFF_SIZE);/* 清零 */
    }
  }
}




/********************************END OF FILE****************************/
