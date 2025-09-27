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


/*
******************************************************************
*                               变量
******************************************************************
*/
/* 定义线程控制块 */
static rt_thread_t alloc_thread = RT_NULL;
static rt_thread_t free_thread = RT_NULL;

/* 定义申请内存的指针 */
static rt_uint32_t *p_test = RT_NULL;

/************************* 全局变量声明 ****************************/
/*
 * 当我们在写应用程序的时候，可能需要用到一些全局变量。
 */

/* 相关宏定义 */
#define  TEST_SIZE   100	  //内存大小


/*
*************************************************************************
*                             函数声明
*************************************************************************
*/
static void alloc_thread_entry(void* parameter);
static void free_thread_entry(void* parameter);

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
	rt_kprintf("这是一个[野火]-STM32F103-指南者-RTT动态内存管理实验！\n");
	
  /* 创建一个任务 */
	alloc_thread =                          /* 线程控制块指针 */
    rt_thread_create( "alloc",              /* 线程名字 */
                      alloc_thread_entry,   /* 线程入口函数 */
                      RT_NULL,             /* 线程入口函数参数 */
                      512,                 /* 线程栈大小 */
                      1,                   /* 线程的优先级 */
                      20);                 /* 线程时间片 */
                   
    /* 启动线程，开启调度 */
   if (alloc_thread != RT_NULL)
        rt_thread_startup(alloc_thread);
    else
        return -1;
    
  free_thread =                          /* 线程控制块指针 */
    rt_thread_create( "free",              /* 线程名字 */
                      free_thread_entry,   /* 线程入口函数 */
                      RT_NULL,             /* 线程入口函数参数 */
                      512,                 /* 线程栈大小 */
                      2,                   /* 线程的优先级 */
                      20);                 /* 线程时间片 */
                   
    /* 启动线程，开启调度 */
   if (free_thread != RT_NULL)
        rt_thread_startup(free_thread);
    else
        return -1;
}

/*
*************************************************************************
*                             线程定义
*************************************************************************
*/

static void alloc_thread_entry(void* parameter)
{		
  rt_kprintf("正在向内存池申请内存...........\n");
  p_test = rt_malloc(TEST_SIZE);    /* 申请动态内存 */
  if(RT_NULL == p_test)/* 没有申请成功 */
     rt_kprintf("动态内存申请失败！\n");
  else
    rt_kprintf("动态内存申请成功，地址为%d！\n\n",p_test);
  
  rt_kprintf("正在向p_test写入数据...........\n");
  *p_test = 1234;
  rt_kprintf("已经写入p_test地址的数据\n");
	rt_kprintf("*p_test = %.4d ,地址为:%d \n\n", *p_test,p_test);

  /* 任务都是一个无限循环，不能返回 */
  while(1)
	{
		LED2_TOGGLE;
    rt_thread_delay(1000);     //每1000ms扫描一次
  }
}

static void free_thread_entry(void* parameter)
{
  rt_kprintf("正在释放内存...........\n");
  rt_free(p_test);
  rt_kprintf("释放内存成功！\n\n");

    /* 任务都是一个无限循环，不能返回 */
  while (1)
  {
    LED1_TOGGLE;
		rt_thread_delay(500);     //每500ms扫描一次		
  }
}




/********************************END OF FILE****************************/
