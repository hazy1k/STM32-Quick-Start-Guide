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

OS_Q          queue;                    //声明消息队列
OS_SEM        sem;                      //声明多值信号量
OS_PEND_DATA  mul_pend_array [ 2 ];     //声明等待对象数组


/*
*********************************************************************************************************
*                                                 TCB
*********************************************************************************************************
*/

static  OS_TCB   AppTaskStartTCB;      //任务控制块

static  OS_TCB   AppTaskPostTCB;
static  OS_TCB   AppTaskPendTCB;


/*
*********************************************************************************************************
*                                                STACKS
*********************************************************************************************************
*/

static  CPU_STK  AppTaskStartStk[APP_TASK_START_STK_SIZE];       //任务堆栈

static  CPU_STK  AppTaskPostStk [ APP_TASK_POST_STK_SIZE ];
static  CPU_STK  AppTaskPendStk [ APP_TASK_PEND_STK_SIZE ];


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  AppTaskStart  (void *p_arg);               //任务函数声明

static  void  AppTaskPost   ( void * p_arg );
static  void  AppTaskPend   ( void * p_arg );


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


		/* 创建多值信号量 sem */
		OSSemCreate ((OS_SEM      *)&sem,            //指向要创建的多值信号量
								 (CPU_CHAR    *)"Sem For Test",  //多值信号量的名字
								 (OS_SEM_CTR   )0,               //多值信号量初始不可用
								 (OS_ERR      *)&err);	         //返回错误类型
								 
		/* 创建消息队列 queue */
		OSQCreate ((OS_Q         *)&queue,           //指向要创建的消息队列
							 (CPU_CHAR     *)"Queue For Test", //消息队列的名字
							 (OS_MSG_QTY    )20,               //最多可容20条消息
							 (OS_ERR       *)&err);            //返回错误类型


		/* 初始化要等待的多个内核对象 */
		mul_pend_array [ 0 ] .PendObjPtr = ( OS_PEND_OBJ * ) & sem;
		mul_pend_array [ 1 ] .PendObjPtr = ( OS_PEND_OBJ * ) & queue;
	
	
		/* 创建 AppTaskPost 任务 */
    OSTaskCreate((OS_TCB     *)&AppTaskPostTCB,                             //任务控制块地址
                 (CPU_CHAR   *)"App Task Post",                             //任务名称
                 (OS_TASK_PTR ) AppTaskPost,                                //任务函数
                 (void       *) 0,                                          //传递给任务函数（形参p_arg）的实参
                 (OS_PRIO     ) APP_TASK_POST_PRIO,                         //任务的优先级
                 (CPU_STK    *)&AppTaskPostStk[0],                          //任务堆栈的基地址
                 (CPU_STK_SIZE) APP_TASK_POST_STK_SIZE / 10,                //任务堆栈空间剩下1/10时限制其增长
                 (CPU_STK_SIZE) APP_TASK_POST_STK_SIZE,                     //任务堆栈空间（单位：sizeof(CPU_STK)）
                 (OS_MSG_QTY  ) 5u,                                         //任务可接收的最大消息数
                 (OS_TICK     ) 0u,                                         //任务的时间片节拍数（0表默认值OSCfg_TickRate_Hz/10）
                 (void       *) 0,                                          //任务扩展（0表不扩展）
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //任务选项
                 (OS_ERR     *)&err);                                       //返回错误类型

		/* 创建 AppTaskPend 任务 */
    OSTaskCreate((OS_TCB     *)&AppTaskPendTCB,                             //任务控制块地址
                 (CPU_CHAR   *)"App Task Pend",                             //任务名称
                 (OS_TASK_PTR ) AppTaskPend,                                //任务函数
                 (void       *) 0,                                          //传递给任务函数（形参p_arg）的实参
                 (OS_PRIO     ) APP_TASK_PEND_PRIO,                         //任务的优先级
                 (CPU_STK    *)&AppTaskPendStk[0],                          //任务堆栈的基地址
                 (CPU_STK_SIZE) APP_TASK_PEND_STK_SIZE / 10,                //任务堆栈空间剩下1/10时限制其增长
                 (CPU_STK_SIZE) APP_TASK_PEND_STK_SIZE,                     //任务堆栈空间（单位：sizeof(CPU_STK)）
                 (OS_MSG_QTY  ) 5u,                                         //任务可接收的最大消息数
                 (OS_TICK     ) 0u,                                         //任务的时间片节拍数（0表默认值OSCfg_TickRate_Hz/10）
                 (void       *) 0,                                          //任务扩展（0表不扩展）
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //任务选项
                 (OS_ERR     *)&err);                                       //返回错误类型
    
		OSTaskDel ( & AppTaskStartTCB, & err );                     //删除起始任务本身，该任务不再运行
		
		
}


/*
*********************************************************************************************************
*                                          POST TASK
*********************************************************************************************************
*/
void TmrCallback (OS_TMR *p_tmr, void *p_arg)             //软件定时器 tmr_for_key 的回调函数
{
	OS_ERR             err;
	
	static uint8_t     ucKey1Press = 0;                                       
	
	
  if ( Key_Scan ( macKEY1_GPIO_PORT, macKEY1_GPIO_PIN, 1, & ucKey1Press ) )//如果KEY1被单击
	{
		/* 发布多值信号量 sem */
    OSSemPost ((OS_SEM  *)&sem,                                            //多值信号量
               (OS_OPT   )OS_OPT_POST_ALL,                                 //发给全部等待任务
               (OS_ERR  *)&err);		                                       //返回错误类型
	}

}


static  void  AppTaskPost ( void * p_arg )
{
	OS_ERR      err;
	OS_TMR      tmr_for_key;                                 //声明软件定时器

	
	(void)p_arg;

					 
  /* 创建软件定时器 */
  OSTmrCreate ((OS_TMR              *)&tmr_for_key,        //软件定时器对象
               (CPU_CHAR            *)"Tmr For Key",       //命名软件定时器
               (OS_TICK              )0,                   //定时器初始值
               (OS_TICK              )1,                   //周期重载值100ms
               (OS_OPT               )OS_OPT_TMR_PERIODIC, //周期性定时
               (OS_TMR_CALLBACK_PTR  )TmrCallback,         //回调函数
               (void                *)"Timer Over!",       //传递实参给回调函数
               (OS_ERR              *)&err);               //返回错误类型
							 
	/* 启动软件定时器 */						 
  OSTmrStart ((OS_TMR   *)&tmr_for_key,                    //软件定时器对象
              (OS_ERR   *)err);                            //返回错误类型
							 
							 
	while (DEF_TRUE) {                                       //任务体
    /* 发布消息到消息队列 queue */
    OSQPost ((OS_Q        *)&queue,                        //消息队列
             (void        *)"One Message",                 //消息内容
             (OS_MSG_SIZE  )sizeof ( "One Message" ),      //消息长度
             (OS_OPT       )OS_OPT_POST_FIFO |             //发到入队端
						                OS_OPT_POST_ALL,               //发给全部等待任务
             (OS_ERR      *)&err);                         //返回错误类型

    OSTimeDlyHMSM ( 0, 0, 1, 0, OS_OPT_TIME_DLY, & err );  //每隔1s发送一次消息
		
	}

}


/*
*********************************************************************************************************
*                                          PEND TASK
*********************************************************************************************************
*/
static  void  AppTaskPend ( void * p_arg )
{
	OS_ERR      err;

	
	(void)p_arg;

					 
	while (DEF_TRUE) {                                                           //任务体
    /* 等待多个内核对象 */
		OSPendMulti ((OS_PEND_DATA  *)mul_pend_array,       //等待对象数组
								 (OS_OBJ_QTY     )2,                    //等待对象数目
								 (OS_TICK        )0,                    //无期限等待
								 (OS_OPT         )OS_OPT_PEND_BLOCKING, //阻塞任务
								 (OS_ERR        *)&err);                //返回错误类型

		/* 查看哪些等待对象可用 */
		if ( mul_pend_array [ 0 ] .RdyObjPtr == mul_pend_array [ 0 ] .PendObjPtr ) //如果 sem 可用
		{
			printf("\r\nKEY1被单击\r\n");
		} 
		if ( mul_pend_array [ 1 ] .RdyObjPtr == mul_pend_array [ 1 ] .PendObjPtr )//如果 queue 可用
		{
			printf("\r\n接收到一条消息，内容为：%s，长度为：%d字节\r\n",
			       ( char * ) mul_pend_array [ 1 ] .RdyMsgPtr,  mul_pend_array [ 1 ] .RdyMsgSize );
		}
		
	}
	
}











