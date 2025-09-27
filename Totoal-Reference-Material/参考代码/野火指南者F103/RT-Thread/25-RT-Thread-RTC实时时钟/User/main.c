/**
  *********************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   RT-Thread 3.0 + STM32 工程模版
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
*************************************************************************
*                               变量
*************************************************************************
*/
/* 定义线程控制块 */
static rt_thread_t led1_thread = RT_NULL;
/* 定义信号量控制块 */
rt_sem_t test_sem = RT_NULL;

extern __IO uint32_t TimeDisplay ;
extern struct rtc_time systmtime;
/*
*************************************************************************
*                             函数声明
*************************************************************************
*/
static void led1_thread_entry(void* parameter);


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
	/* 创建一个信号量 */
	test_sem = rt_sem_create("test_sem",/* 信号量名字 */
                     0,     /* 信号量初始值0 */
                     RT_IPC_FLAG_FIFO); /* 信号量模式 FIFO(0x00)*/
  if (test_sem != RT_NULL)
    rt_kprintf("信号量创建成功！\n\n");
	
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
}

/*
*************************************************************************
*                             线程定义
*************************************************************************
*/

static void led1_thread_entry(void* parameter)
{	
	rt_err_t err = RT_EOK;
	while (1)
	{
		/* 每过1s 更新一次时间*/
		err = rt_sem_take(test_sem,0); /* 获取信号量,等待时间0 */	
		if(RT_EOK == err)	
		{
			/* 当前时间 */
			Time_Display( RTC_GetCounter(),&systmtime); 		  
			TimeDisplay = 0;
		}
		//按下按键，通过串口修改时间
		if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON  )
		{
			struct rtc_time set_time;

			/*使用串口接收设置的时间，输入数字时注意末尾要加回车*/
			Time_Regulate_Get(&set_time);
			/*用接收到的时间设置RTC*/
			Time_Adjust(&set_time);
			
			//向备份寄存器写入标志
			BKP_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);

		} 
		rt_thread_delay(20);   /* 延时500个tick */		 		
	}
}

/********************************END OF FILE****************************/
