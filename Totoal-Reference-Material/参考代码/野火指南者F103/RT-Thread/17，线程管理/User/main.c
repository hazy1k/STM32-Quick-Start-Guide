/**
  *********************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   RT-Thread 3.0 + STM32 线程管理
  *********************************************************************
  * @attention
  *
  * 实验平台:野火 F103-霸道 STM32 开发板 
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
static rt_thread_t led1_thread = RT_NULL;
static rt_thread_t key_thread = RT_NULL;
/*
*************************************************************************
*                             函数声明
*************************************************************************
*/
static void led1_thread_entry(void* parameter);
static void key_thread_entry(void* parameter);

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
  rt_kprintf("这是一个[野火]-STM32F103霸道-RTT线程管理实验！\n\n");
  rt_kprintf("按下K1挂起线程，按下K2恢复线程\n");
	led1_thread =                          /* 线程控制块指针 */
    rt_thread_create( "led1",              /* 线程名字 */
                      led1_thread_entry,   /* 线程入口函数 */
                      RT_NULL,             /* 线程入口函数参数 */
                      512,                 /* 线程栈大小 */
                      3,                   /* 线程的优先级 */
                      20);                 /* 线程时间片 */
                   
    /* 启动线程，开启调度 */
   if (led1_thread != RT_NULL)
        rt_thread_startup(led1_thread);
    else
        return -1;
    
  key_thread =                          /* 线程控制块指针 */
    rt_thread_create( "key",              /* 线程名字 */
                      key_thread_entry,   /* 线程入口函数 */
                      RT_NULL,             /* 线程入口函数参数 */
                      512,                 /* 线程栈大小 */
                      2,                   /* 线程的优先级 */
                      20);                 /* 线程时间片 */
                   
    /* 启动线程，开启调度 */
   if (key_thread != RT_NULL)
        rt_thread_startup(key_thread);
    else
        return -1;
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
        rt_kprintf("led1_thread running,LED1_ON\r\n");
        
        LED1_OFF;     
        rt_thread_delay(500);   /* 延时500个tick */		 		
        rt_kprintf("led1_thread running,LED1_OFF\r\n");
    }
}

static void key_thread_entry(void* parameter)
{	
  rt_err_t uwRet = RT_EOK;	
    while (1)
    {
      if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON )/* K1 被按下 */
      {
        printf("挂起LED1线程！\n");
        uwRet = rt_thread_suspend(led1_thread);/* 挂起LED1线程 */
        if(RT_EOK == uwRet)
        {
          rt_kprintf("挂起LED1线程成功！\n");
        }
        else
        {
          rt_kprintf("挂起LED1线程失败！失败代码：0x%lx\n",uwRet);
        }
      } 
      if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON )/* K1 被按下 */
      {
        printf("恢复LED1线程！\n");
        uwRet = rt_thread_resume(led1_thread);/* 恢复LED1线程！ */
        if(RT_EOK == uwRet)
        {
          rt_kprintf("恢复LED1线程成功！\n");
        }
        else
        {
          rt_kprintf("恢复LED1线程失败！失败代码：0x%lx\n",uwRet);
        }
      }
      rt_thread_delay(20);
    }
}




/********************************END OF FILE****************************/
