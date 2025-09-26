/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                          (c) Copyright 2003-2013; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                            EXAMPLE CODE
*
*                                     ST Microelectronics STM32
*                                              on the
*
*                                     Micrium uC-Eval-STM32F107
*                                        Evaluation Board
*
* Filename      : app.c
* Version       : V1.00
* Programmer(s) : EHS
*                 DC
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include <includes.h>


/*
***************************************************************************
*                             LOCAL DEFINES
***************************************************************************
*/

OS_SEM SemOfKey;          //标志KEY1是否被单击的多值信号量


/*
**************************************************************************
*                                TCB
**************************************************************************
*/

static  OS_TCB   AppTaskStartTCB;                                //任务控制块

static  OS_TCB   AppTaskKeyTCB;
static  OS_TCB   AppTaskLed1TCB;


/*
***************************************************************************
*                                                STACKS
*************************************************************************
*/

static  CPU_STK  AppTaskStartStk[APP_TASK_START_STK_SIZE];       //任务堆栈

static  CPU_STK  AppTaskKeyStk [ APP_TASK_KEY_STK_SIZE ];
static  CPU_STK  AppTaskLed1Stk [ APP_TASK_LED1_STK_SIZE ];


/*
*************************************************************************
*                                         FUNCTION PROTOTYPES
*************************************************************************
*/

static  void  AppTaskStart  (void *p_arg);                       //任务函数声明

static  void  AppTaskKey  ( void * p_arg );
static  void  AppTaskLed1 ( void * p_arg );


/*
******************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  
*               It is assumed that your code will callmain() 
*               once you have performed all necessary initialization.
* Arguments   : none
*
* Returns     : none
*****************************************************************************
*/

int  main (void)
{
    OS_ERR  err;


    OSInit(&err);                                                           //初始化 uC/OS-III

	  /* 创建起始任务 */
    OSTaskCreate((OS_TCB     *)&AppTaskStartTCB,                            //任务控制块地址
                 (CPU_CHAR   *)"App Task Start",                            //任务名称
                 (OS_TASK_PTR ) AppTaskStart,                               //任务函数
                 (void       *) 0,                                          //传递给任务函数（形参p_arg）的实参
                 (OS_PRIO     ) APP_TASK_START_PRIO,                        //任务的优先级
                 (CPU_STK    *)&AppTaskStartStk[0],                         //任务堆栈的基地址
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE / 10,               //任务堆栈空间剩下1/10时限制其增长
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE,                    //任务堆栈空间（单位：sizeof(CPU_STK)）
                 (OS_MSG_QTY  ) 5u,                                         //任务可接收的最大消息数
                 (OS_TICK     ) 0u,                                         //任务的时间片节拍数（0表默认值OSCfg_TickRate_Hz/10）
                 (void       *) 0,                                          //任务扩展（0表不扩展）
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //任务选项
                 (OS_ERR     *)&err);                                       //返回错误类型

    OSStart(&err);                                                          //启动多任务管理（交由uC/OS-III控制）

}


static  void  AppTaskStart (void *p_arg)
{
    CPU_INT32U  cpu_clk_freq;
    CPU_INT32U  cnts;
    OS_ERR      err;


   (void)p_arg;

    BSP_Init();                                                 //板级初始化
    CPU_Init();                                                 //初始化 CPU 组件（时间戳、关中断时间测量和主机名）

    cpu_clk_freq = BSP_CPU_ClkFreq();                           //获取 CPU 内核时钟频率（SysTick 工作时钟）
    cnts = cpu_clk_freq / (CPU_INT32U)OSCfg_TickRate_Hz;        //根据用户设定的时钟节拍频率计算 SysTick 定时器的计数值
    OS_CPU_SysTickInit(cnts);                                   //调用 SysTick 初始化函数，设置定时器计数值和启动定时器

    Mem_Init();                                                 //初始化内存管理组件（堆内存池和内存池表）

#if OS_CFG_STAT_TASK_EN > 0u                                    //如果使能（默认使能）了统计任务
    OSStatTaskCPUUsageInit(&err);                               //计算没有应用任务（只有空闲任务）运行时 CPU 的（最大）
#endif                                                          //容量（决定 OS_Stat_IdleCtrMax 的值，为后面计算 CPU 
                                                                //使用率使用）。
    CPU_IntDisMeasMaxCurReset();                                //复位（清零）当前最大关中断时间

    
		/* 创建多值信号量 SemOfKey */
    OSSemCreate((OS_SEM      *)&SemOfKey,    //指向信号量变量的指针
               (CPU_CHAR    *)"SemOfKey",    //信号量的名字
               (OS_SEM_CTR   )0,             //信号量这里是指示事件发生，所以赋值为0，表示事件还没有发生
               (OS_ERR      *)&err);         //错误类型
							 

		/* 创建 AppTaskKey 任务 */
    OSTaskCreate((OS_TCB     *)&AppTaskKeyTCB,                              //任务控制块地址
                 (CPU_CHAR   *)"App Task Key",                              //任务名称
                 (OS_TASK_PTR ) AppTaskKey,                                 //任务函数
                 (void       *) 0,                                          //传递给任务函数（形参p_arg）的实参
                 (OS_PRIO     ) APP_TASK_KEY_PRIO,                          //任务的优先级
                 (CPU_STK    *)&AppTaskKeyStk[0],                           //任务堆栈的基地址
                 (CPU_STK_SIZE) APP_TASK_KEY_STK_SIZE / 10,                 //任务堆栈空间剩下1/10时限制其增长
                 (CPU_STK_SIZE) APP_TASK_KEY_STK_SIZE,                      //任务堆栈空间（单位：sizeof(CPU_STK)）
                 (OS_MSG_QTY  ) 5u,                                         //任务可接收的最大消息数
                 (OS_TICK     ) 0u,                                         //任务的时间片节拍数（0表默认值OSCfg_TickRate_Hz/10）
                 (void       *) 0,                                          //任务扩展（0表不扩展）
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //任务选项
                 (OS_ERR     *)&err);                                       //返回错误类型
    
		/* 创建 LED1 任务 */
    OSTaskCreate((OS_TCB     *)&AppTaskLed1TCB,                             //任务控制块地址
                 (CPU_CHAR   *)"App Task Led1",                             //任务名称
                 (OS_TASK_PTR ) AppTaskLed1,                                //任务函数
                 (void       *) 0,                                          //传递给任务函数（形参p_arg）的实参
                 (OS_PRIO     ) APP_TASK_LED1_PRIO,                         //任务的优先级
                 (CPU_STK    *)&AppTaskLed1Stk[0],                          //任务堆栈的基地址
                 (CPU_STK_SIZE) APP_TASK_LED1_STK_SIZE / 10,                //任务堆栈空间剩下1/10时限制其增长
                 (CPU_STK_SIZE) APP_TASK_LED1_STK_SIZE,                     //任务堆栈空间（单位：sizeof(CPU_STK)）
                 (OS_MSG_QTY  ) 5u,                                         //任务可接收的最大消息数
                 (OS_TICK     ) 0u,                                         //任务的时间片节拍数（0表默认值OSCfg_TickRate_Hz/10）
                 (void       *) 0,                                          //任务扩展（0表不扩展）
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //任务选项
                 (OS_ERR     *)&err);                                       //返回错误类型
		
		OSTaskDel ( & AppTaskStartTCB, & err );                     //删除起始任务本身，该任务不再运行
		
		
}


/*
****************************************************************************
*                                          KEY TASK
****************************************************************************
*/
static  void  AppTaskKey ( void * p_arg )
{
	OS_ERR      err;

	uint8_t ucKey1Press = 0;
	
	
	(void)p_arg;

					 
	while (DEF_TRUE) {                                                         //任务体
		if( Key_Scan ( macKEY1_GPIO_PORT, macKEY1_GPIO_PIN, 1, & ucKey1Press ) ) //如果KEY1被单击
		  OSSemPost((OS_SEM  *)&SemOfKey,                                        //发布SemOfKey
							 (OS_OPT   )OS_OPT_POST_ALL,                                   //发布给所有等待任务
							 (OS_ERR  *)&err);                                             //返回错误类型

		OSTimeDlyHMSM ( 0, 0, 0, 20, OS_OPT_TIME_DLY, & err );                   //每20ms扫描一次
		
	}
	
}


/*
*********************************************************************************
*                                          LED1 TASK
*********************************************************************************
*/

static  void  AppTaskLed1 ( void * p_arg )
{
    OS_ERR         err;
	  CPU_INT32U     cpu_clk_freq;
	  CPU_TS         ts_sem_post, ts_sem_get;
		CPU_SR_ALLOC();  //使用到临界段（在关/开中断时）时必需该宏，该宏声明和定义一个局部变
										 //量，用于保存关中断前的 CPU 状态寄存器 SR（临界段关中断只需保存SR）
										 //，开中断时将该值还原。
    (void)p_arg;


	  cpu_clk_freq = BSP_CPU_ClkFreq();               //获取CPU时钟，时间戳是以该时钟计数
	
	
    while (DEF_TRUE) {                              //任务体
					
			OSSemPend ((OS_SEM   *)&SemOfKey,             //等待该信号量被发布
								 (OS_TICK   )0,                     //无期限等待
								 (OS_OPT    )OS_OPT_PEND_BLOCKING,  //如果没有信号量可用就等待
								 (CPU_TS   *)&ts_sem_post,          //获取信号量最后一次被发布的时间戳
								 (OS_ERR   *)&err);                 //返回错误类型
				
			ts_sem_get = OS_TS_GET();                     //获取解除等待时的时间戳
				
			macLED1_TOGGLE ();                            //切换LED1的亮灭状态
				
			OS_CRITICAL_ENTER();                          //进入临界段，不希望下面串口打印遭到中断
			
			printf ( "\r\n发布信号量的时间戳是%d", ts_sem_post );
			printf ( "\r\n解除等待状态的时间戳是%d", ts_sem_get );
			printf ( "\r\n接收到信号量与发布信号量的时间相差%dus\r\n", 
			        ( ts_sem_get - ts_sem_post ) / ( cpu_clk_freq / 1000000 ) );
			
			OS_CRITICAL_EXIT(); 

    }
		
		
}












