/**
  *********************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   RT-Thread 3.0 + STM32 邮箱
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
/* 定义邮箱控制块 */
static rt_mailbox_t test_mail = RT_NULL;

/************************* 全局变量声明 ****************************/
/*
 * 当我们在写应用程序的时候，可能需要用到一些全局变量。
 */
char test_str1[] = "this is a mail test 1";/* 邮箱消息test1 */
char test_str2[] = "this is a mail test 2";/* 邮箱消息test2 */
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
	rt_kprintf("这是一个[野火]-STM32F103-指南者-RTT邮箱消息实验！\n");
	rt_kprintf("按下K1 | K2进行邮箱实验测试!\n");
  /* 创建一个邮箱 */
	 test_mail = rt_mb_create("test_mail", /* 邮箱名字 */
                            10, 
                            RT_IPC_FLAG_FIFO);/* 信号量模式 FIFO(0x00)*/
  if (test_mail != RT_NULL)
    rt_kprintf("邮箱创建成功！\n\n");
    
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
  rt_err_t uwRet = RT_EOK;
  char *r_str;
  /* 任务都是一个无限循环，不能返回 */
  while(1)
	{
    /* 等待接邮箱消息 */
    uwRet = rt_mb_recv(test_mail, /* 邮箱对象句柄 */
                      (rt_uint32_t*)&r_str, /* 接收邮箱消息 */
                      RT_WAITING_FOREVER);/* 指定超时事件,一直等 */
    
      if(RT_EOK == uwRet) /* 如果接收完成并且正确 */
      {
        rt_kprintf ( "邮箱的内容是:%s\n\n",r_str);		
        LED1_TOGGLE;       //LED1	反转
      }
      else
        rt_kprintf ( "邮箱接收错误！错误码是0x%x\n",uwRet);	
  }
}

static void send_thread_entry(void* parameter)
{	
  rt_err_t uwRet = RT_EOK;
    /* 任务都是一个无限循环，不能返回 */
  while (1)
  {     //如果KEY1被单击
    if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON )      
		{
      rt_kprintf ( "KEY1被单击\n" );
			/* 发送一个邮箱消息1 */
			uwRet = rt_mb_send(test_mail,(rt_uint32_t)&test_str1); 
      if(RT_EOK == uwRet)
        rt_kprintf ( "邮箱消息发送成功\n" );
      else
        rt_kprintf ( "邮箱消息发送失败\n" );
		}
      //如果KEY2被单击
		if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON ) 
		{
      rt_kprintf ( "KEY2被单击\n" );	
			/* 发送一个邮箱2 */
			uwRet = rt_mb_send(test_mail,(rt_uint32_t)&test_str2); 
      if(RT_EOK == uwRet)
        rt_kprintf ( "邮箱消息发送成功\n" );
      else
        rt_kprintf ( "邮箱消息发送失败\n" );
		}
		rt_thread_delay(20);     //每20ms扫描一次		
  }
}




/********************************END OF FILE****************************/
