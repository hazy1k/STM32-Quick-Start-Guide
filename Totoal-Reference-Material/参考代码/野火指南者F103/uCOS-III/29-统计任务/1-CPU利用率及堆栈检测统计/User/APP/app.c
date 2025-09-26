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
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

OS_SEM SemOfKey;          //标志KEY1是否被单击的多值信号量


/*
*********************************************************************************************************
*                                                 TCB
*********************************************************************************************************
*/

static  OS_TCB   AppTaskStartTCB;

static  OS_TCB   AppTaskLed1TCB;
static  OS_TCB   AppTaskLed2TCB;
static  OS_TCB   AppTaskLed3TCB;
static  OS_TCB   AppTaskStatusTCB;

/*
*********************************************************************************************************
*                                                STACKS
*********************************************************************************************************
*/

static  CPU_STK  AppTaskStartStk[APP_TASK_START_STK_SIZE];

static  CPU_STK  AppTaskLed1Stk [ APP_TASK_LED1_STK_SIZE ];
static  CPU_STK  AppTaskLed2Stk [ APP_TASK_LED2_STK_SIZE ];
static  CPU_STK  AppTaskLed3Stk [ APP_TASK_LED3_STK_SIZE ];
static  CPU_STK  AppTaskStatusStk [ APP_TASK_STATUS_STK_SIZE ];

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  AppTaskStart  (void *p_arg);

static  void  AppTaskLed1  ( void * p_arg );
static  void  AppTaskLed2  ( void * p_arg );
static  void  AppTaskLed3  ( void * p_arg );
static  void  AppTaskStatus  ( void * p_arg );

/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Arguments   : none
*
* Returns     : none
*********************************************************************************************************
*/

int  main (void)
{
    OS_ERR  err;


    OSInit(&err);                                               /* Init uC/OS-III.                                      */

    OSTaskCreate((OS_TCB     *)&AppTaskStartTCB,                /* Create the start task                                */
                 (CPU_CHAR   *)"App Task Start",
                 (OS_TASK_PTR ) AppTaskStart,
                 (void       *) 0,
                 (OS_PRIO     ) APP_TASK_START_PRIO,
                 (CPU_STK    *)&AppTaskStartStk[0],
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE / 10,
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE,
                 (OS_MSG_QTY  ) 5u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);

    OSStart(&err);                                              /* Start multitasking (i.e. give control to uC/OS-III). */
		
		
}


/*
*********************************************************************************************************
*                                          STARTUP TASK
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Arguments   : p_arg   is the argument passed to 'AppTaskStart()' by 'OSTaskCreate()'.
*
* Returns     : none
*
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

static  void  AppTaskStart (void *p_arg)
{
    CPU_INT32U  cpu_clk_freq;
    CPU_INT32U  cnts;
    OS_ERR      err;


   (void)p_arg;

    BSP_Init();                                                 /* Initialize BSP functions                             */
    CPU_Init();

    cpu_clk_freq = BSP_CPU_ClkFreq();                           /* Determine SysTick reference freq.                    */
    cnts = cpu_clk_freq / (CPU_INT32U)OSCfg_TickRate_Hz;        /* Determine nbr SysTick increments                     */
    OS_CPU_SysTickInit(cnts);                                   /* Init uC/OS periodic time src (SysTick).              */

    Mem_Init();                                                 /* Initialize Memory Management Module                  */

#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&err);                               /* Compute CPU capacity with no task running            */
#endif

    CPU_IntDisMeasMaxCurReset();
    
		/* 创建多值信号量 SemOfKey */
    OSSemCreate((OS_SEM      *)&SemOfKey,    //指向信号量变量的指针
               (CPU_CHAR    *)"SemOfKey",    //信号量的名字
               (OS_SEM_CTR   )1,             //信号量这里是指示事件发生，所以赋值为0，表示事件还没有发生
               (OS_ERR      *)&err);         //错误类型
							 
    /* Create the Led1 task                                */
    OSTaskCreate((OS_TCB     *)&AppTaskLed1TCB,               
                 (CPU_CHAR   *)"App Task Led1",
                 (OS_TASK_PTR ) AppTaskLed1,
                 (void       *) 0,
                 (OS_PRIO     ) APP_TASK_LED1_PRIO,
                 (CPU_STK    *)&AppTaskLed1Stk[0],
                 (CPU_STK_SIZE) APP_TASK_LED1_STK_SIZE / 10,
                 (CPU_STK_SIZE) APP_TASK_LED1_STK_SIZE,
                 (OS_MSG_QTY  ) 5u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);

    /* Create the Led2 task                                */								 
    OSTaskCreate((OS_TCB     *)&AppTaskLed2TCB,               
                 (CPU_CHAR   *)"App Task Led2",
                 (OS_TASK_PTR ) AppTaskLed2,
                 (void       *) 0,
                 (OS_PRIO     ) APP_TASK_LED2_PRIO,
                 (CPU_STK    *)&AppTaskLed2Stk[0],
                 (CPU_STK_SIZE) APP_TASK_LED2_STK_SIZE / 10,
                 (CPU_STK_SIZE) APP_TASK_LED2_STK_SIZE,
                 (OS_MSG_QTY  ) 5u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);
    
    /* Create the Led3 task                                */
    OSTaskCreate((OS_TCB     *)&AppTaskLed3TCB,                
                 (CPU_CHAR   *)"App Task Led3",
                 (OS_TASK_PTR ) AppTaskLed3,
                 (void       *) 0,
                 (OS_PRIO     ) APP_TASK_LED3_PRIO,
                 (CPU_STK    *)&AppTaskLed3Stk[0],
                 (CPU_STK_SIZE) APP_TASK_LED3_STK_SIZE / 10,
                 (CPU_STK_SIZE) APP_TASK_LED3_STK_SIZE,
                 (OS_MSG_QTY  ) 5u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);
		
    /* Create the status task                                */
		OSTaskCreate((OS_TCB     *)&AppTaskStatusTCB,                
                 (CPU_CHAR   *)"App Task Status",
                 (OS_TASK_PTR ) AppTaskStatus,
                 (void       *) 0,
                 (OS_PRIO     ) APP_TASK_STATUS_PRIO,
                 (CPU_STK    *)&AppTaskStatusStk[0],
                 (CPU_STK_SIZE) APP_TASK_STATUS_STK_SIZE / 10,
                 (CPU_STK_SIZE) APP_TASK_STATUS_STK_SIZE,
                 (OS_MSG_QTY  ) 5u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);
                 
		OSTaskDel ( & AppTaskStartTCB, & err );
		
		
}



static  void  AppTaskLed1 ( void * p_arg )
{
    OS_ERR      err;
    uint32_t    i;
  
   (void)p_arg;
  

    while (DEF_TRUE) {                                          /* Task body, always written as an infinite loop.       */
			
      printf("AppTaskLed1 Running\n");
      
      for(i=0;i<10000;i++)      //模拟任务占用cpu
      {
        ;
      }
      
      macLED1_TOGGLE ();
			OSTimeDlyHMSM (0,0,0,500,OS_OPT_TIME_PERIODIC,&err);
    }
		
		
}



static  void  AppTaskLed2 ( void * p_arg )
{
    OS_ERR      err;
    uint32_t    i;
  
   (void)p_arg;


    while (DEF_TRUE) {                                          /* Task body, always written as an infinite loop.       */
			printf("AppTaskLed2 Running\n");
     
      for(i=0;i<100000;i++)      //模拟任务占用cpu
      {
        ;
      }
      macLED2_TOGGLE ();

			OSTimeDlyHMSM (0,0,0,500,OS_OPT_TIME_PERIODIC,&err);
    }
		
		
}



static  void  AppTaskLed3 ( void * p_arg )
{
    OS_ERR      err;

    uint32_t    i;
   (void)p_arg;


    while (DEF_TRUE) {                                        
				
      macLED3_TOGGLE ();
            
      for(i=0;i<500000;i++)      //模拟任务占用cpu
      {
        ;
      }
      
      printf("AppTaskLed3 Running\n");

      
			OSTimeDlyHMSM (0,0,0,500,OS_OPT_TIME_PERIODIC,&err);
      
    }

}

static  void  AppTaskStatus  ( void * p_arg )
{ 
    OS_ERR      err;

    CPU_SR_ALLOC();

    (void)p_arg;

    while (DEF_TRUE) {
      
      OS_CRITICAL_ENTER();                              //进入临界段，避免串口打印被打断
      printf("------------------------------------------------------------\n");
      printf ( "CPU利用率：%d.%d%%\r\n",
               OSStatTaskCPUUsage / 100, OSStatTaskCPUUsage % 100 );  
      printf ( "CPU最大利用率：%d.%d%%\r\n", 
               OSStatTaskCPUUsageMax / 100, OSStatTaskCPUUsageMax % 100 );

         
      printf ( "LED1任务的CPU利用率：%d.%d%%\r\n", 
               AppTaskLed1TCB.CPUUsageMax / 100, AppTaskLed1TCB.CPUUsageMax % 100 ); 
      printf ( "LED2任务的CPU利用率：%d.%d%%\r\n", 
               AppTaskLed2TCB.CPUUsageMax / 100, AppTaskLed2TCB.CPUUsageMax % 100 ); 
      printf ( "LED3任务的CPU利用率：%d.%d%%\r\n", 
               AppTaskLed3TCB.CPUUsageMax / 100, AppTaskLed3TCB.CPUUsageMax % 100 ); 
      printf ( "统计任务的CPU利用率：%d.%d%%\r\n", 
               AppTaskStatusTCB.CPUUsageMax / 100, AppTaskStatusTCB.CPUUsageMax % 100 ); 

      printf ( "LED1任务的已用和空闲堆栈大小分别为：%d,%d\r\n", 
               AppTaskLed1TCB.StkUsed, AppTaskLed1TCB.StkFree ); 
      printf ( "LED2任务的已用和空闲堆栈大小分别为：%d,%d\r\n", 
               AppTaskLed2TCB.StkUsed, AppTaskLed2TCB.StkFree ); 
      printf ( "LED3任务的已用和空闲堆栈大小分别为：%d,%d\r\n", 
               AppTaskLed3TCB.StkUsed, AppTaskLed3TCB.StkFree );     
      printf ( "统计任务的已用和空闲堆栈大小分别为：%d,%d\r\n", 
               AppTaskStatusTCB.StkUsed, AppTaskStatusTCB.StkFree );
               
      printf("------------------------------------------------------------\n");         
      OS_CRITICAL_EXIT();                               //退出临界段

      OSTimeDlyHMSM (0,0,0,500,OS_OPT_TIME_PERIODIC,&err);
        
    }
}

