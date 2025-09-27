/**
  *********************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   RT-Thread 3.0 + STM32 事件标志组
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
/* 定义线程控制块 */
static rt_thread_t receive_thread = RT_NULL;
static rt_thread_t send_thread = RT_NULL;
/* 定义事件控制块 */
static rt_event_t test_event = RT_NULL;

/************************* 全局变量声明 ****************************/
/*
 * 当我们在写应用程序的时候，可能需要用到一些全局变量。
 */
#define KEY1_EVENT  (0x01 << 0)//设置事件掩码的位0
#define KEY2_EVENT  (0x01 << 1)//设置事件掩码的位1
/*
*************************************************************************
*                             函数声明
*************************************************************************
*/
static void receive_thread_entry(void* parameter);
static void send_thread_entry(void* parameter);

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
	rt_kprintf("这是一个[野火]-STM32F103-指南者-RTT事件标志组实验！\n");
   /* 创建一个事件 */
	test_event = rt_event_create("test_event",/* 事件标志组名字 */
                        RT_IPC_FLAG_PRIO); /* 事件模式 FIFO(0x00)*/
  if (test_event != RT_NULL)
    rt_kprintf("事件创建成功！\n\n");
    
	receive_thread =                          /* 线程控制块指针 */
    rt_thread_create( "receive",              /* 线程名字 */
                      receive_thread_entry,   /* 线程入口函数 */
                      RT_NULL,             /* 线程入口函数参数 */
                      512,                 /* 线程栈大小 */
                      3,                   /* 线程的优先级 */
                      20);                 /* 线程时间片 */
                   
    /* 启动线程，开启调度 */
   if (receive_thread != RT_NULL)
        rt_thread_startup(receive_thread);
    else
        return -1;
    
  send_thread =                          /* 线程控制块指针 */
    rt_thread_create( "send",              /* 线程名字 */
                      send_thread_entry,   /* 线程入口函数 */
                      RT_NULL,             /* 线程入口函数参数 */
                      512,                 /* 线程栈大小 */
                      2,                   /* 线程的优先级 */
                      20);                 /* 线程时间片 */
                   
    /* 启动线程，开启调度 */
   if (send_thread != RT_NULL)
        rt_thread_startup(send_thread);
    else
        return -1;
}

/*
*************************************************************************
*                             线程定义
*************************************************************************
*/

static void receive_thread_entry(void* parameter)
{		
  rt_uint32_t recved;
  /* 任务都是一个无限循环，不能返回 */
  while(1)
	{
			/* 等待接收事件标志 */
     rt_event_recv(test_event,  /* 事件对象句柄 */
                   KEY1_EVENT|KEY2_EVENT,/* 接收线程感兴趣的事件 */
                   RT_EVENT_FLAG_AND|RT_EVENT_FLAG_CLEAR,/* 接收选项 */
                   RT_WAITING_FOREVER,/* 指定超时事件,一直等 */
                   &recved);    /* 指向接收到的事件 */
      if(recved == (KEY1_EVENT|KEY2_EVENT)) /* 如果接收完成并且正确 */
      {
        rt_kprintf ( "Key1与Key2都按下\n");		
        LED1_TOGGLE;       //LED1	反转
      }
      else
        rt_kprintf ( "事件错误！\n");	
  }
}

static void send_thread_entry(void* parameter)
{	
    /* 任务都是一个无限循环，不能返回 */
  while (1)
  {
    if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON )       //如果KEY2被单击
		{
      rt_kprintf ( "KEY1被单击\n" );
			/* 发送一个事件1 */
			rt_event_send(test_event,KEY1_EVENT);    					
		}
    
		if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON )       //如果KEY2被单击
		{
      rt_kprintf ( "KEY2被单击\n" );	
			/* 发送一个事件2 */
			rt_event_send(test_event,KEY2_EVENT);    				
		}
		rt_thread_delay(20);     //每20ms扫描一次		
  }
}




/********************************END OF FILE****************************/
