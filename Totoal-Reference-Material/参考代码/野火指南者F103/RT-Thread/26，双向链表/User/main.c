/**
  *********************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   RT-Thread 3.0 + STM32 双向链表实验
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
static rt_thread_t test1_thread = RT_NULL;
static rt_thread_t test2_thread = RT_NULL;

/************************* 全局变量声明 ****************************/
/*
 * 当我们在写应用程序的时候，可能需要用到一些全局变量。
 */

/*
*************************************************************************
*                             函数声明
*************************************************************************
*/
static void test1_thread_entry(void* parameter);
static void test2_thread_entry(void* parameter);

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
	rt_kprintf("这是一个[野火]-STM32F103-指南者-RTT双向链表操作实验！\n");
  
	test1_thread =                          /* 线程控制块指针 */
    rt_thread_create( "test1",              /* 线程名字 */
                      test1_thread_entry,   /* 线程入口函数 */
                      RT_NULL,             /* 线程入口函数参数 */
                      512,                 /* 线程栈大小 */
                      2,                   /* 线程的优先级 */
                      20);                 /* 线程时间片 */
                   
    /* 启动线程，开启调度 */
   if (test1_thread != RT_NULL)
        rt_thread_startup(test1_thread);
    else
        return -1;
    
  test2_thread =                          /* 线程控制块指针 */
    rt_thread_create( "test2",              /* 线程名字 */
                      test2_thread_entry,   /* 线程入口函数 */
                      RT_NULL,             /* 线程入口函数参数 */
                      512,                 /* 线程栈大小 */
                      3,                   /* 线程的优先级 */
                      20);                 /* 线程时间片 */
                   
    /* 启动线程，开启调度 */
   if (test2_thread != RT_NULL)
        rt_thread_startup(test2_thread);
    else
        return -1;
}

/*
*************************************************************************
*                             线程定义
*************************************************************************
*/

static void test1_thread_entry(void* parameter)
{		
  rt_list_t *head;  /* 定义一个双向链表的头节点 */
  rt_list_t *node1;  /* 定义一个双向链表的头节点 */
  rt_list_t *node2;  /* 定义一个双向链表的头节点 */
  
  head = rt_malloc(sizeof(rt_list_t));/* 申请动态内存 */
  if(RT_NULL == head)/* 没有申请成功 */
     rt_kprintf("动态内存申请失败！\n");
  else
    rt_kprintf("动态内存申请成功，头结点地址为%d！\n",head);
  
  rt_kprintf("\n双向链表初始化中......\n");
  rt_list_init(head);
  if(rt_list_isempty(head))
    rt_kprintf("双向链表初始化成功!\n\n");
  
  /* 插入节点：顺序插入与从末尾插入 */
  rt_kprintf("添加节点和尾节点添加......\n");			
  /* 动态申请第一个结点的内存 */
  node1 = rt_malloc(sizeof(rt_list_t));
  /* 动态申请第二个结点的内存 */
  node2 = rt_malloc(sizeof(rt_list_t));
  
  rt_kprintf("添加第一个节点与第二个节点.....\n");
  /* 因为这是在某个节点后面添加一个节点函数
     为后面的rt_list_insert_before（某个节点之前）
     添加节点做铺垫,两个函数添加完之后的顺序是
     head -> node1 -> node2 */
  rt_list_insert_after(head,node2);
  rt_list_insert_before(node2,node1);
  if ((node1->prev == head) && (node2->prev == node1))
    rt_kprintf("添加节点成功!\n\n");
  else
    rt_kprintf("添加节点失败!\n\n");
  
  rt_kprintf("删除节点......\n");	/* 删除已有节点 */
  rt_list_remove(node1);
  rt_free(node1);/* 释放第一个节点的内存 */
  if (node2->prev == head)
    rt_kprintf("删除节点成功\n\n");

  /* 任务都是一个无限循环，不能返回 */
  while(1)
	{
    LED1_TOGGLE;
		rt_thread_delay(500);     //每500ms扫描一次
  }
}

static void test2_thread_entry(void* parameter)
{	

    /* 任务都是一个无限循环，不能返回 */
  while (1)
  {     
    rt_kprintf("任务运行中!\n");
    LED2_TOGGLE;
		rt_thread_delay(1000);     //每1000ms扫描一次		
  }
}




/********************************END OF FILE****************************/
