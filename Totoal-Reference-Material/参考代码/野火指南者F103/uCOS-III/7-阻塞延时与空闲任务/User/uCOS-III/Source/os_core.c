#include "os.h"

/* 就绪列表初始化 */
void OS_RdyListInit(void)
{
	OS_PRIO i;
	OS_RDY_LIST *p_rdy_list;
	
	for( i=0u; i<OS_CFG_PRIO_MAX; i++ )
	{
		p_rdy_list = &OSRdyList[i];
		p_rdy_list->HeadPtr = (OS_TCB *)0;
		p_rdy_list->TailPtr = (OS_TCB *)0;
	}
}

/* RTOS初始化
** 初始化全局变量
*/
void OSInit (OS_ERR *p_err)
{
	/* 配置OS初始状态为停止态 */
	OSRunning =  OS_STATE_OS_STOPPED;
	
	/* 初始化两个全局TCB，这两个TCB用于任务切换 */
	OSTCBCurPtr = (OS_TCB *)0;
	OSTCBHighRdyPtr = (OS_TCB *)0;
	
	/* 初始化就绪列表 */
	OS_RdyListInit();
	
	/* 初始化空闲任务 */
	OS_IdleTaskInit(p_err);
	if (*p_err != OS_ERR_NONE) 
	{
        return;
    }
}

/* 启动RTOS，将不再返回 */
void OSStart (OS_ERR *p_err)
{	
	if( OSRunning == OS_STATE_OS_STOPPED )
	{
		/* 手动配置任务1先运行 */
		OSTCBHighRdyPtr = OSRdyList[0].HeadPtr;
		
		/* 启动任务切换，不会返回 */
		OSStartHighRdy();
		
		/* 不会运行到这里，运行到这里表示发生了致命的错误 */
		*p_err = OS_ERR_FATAL_RETURN;
	}
	else
	{
		*p_err = OS_STATE_OS_RUNNING;
	}
}

/* 任务切换，实际就是触发PendSV异常，然后在PendSV异常中进行上下文切换 */
void OSSched(void)
{
#if 0	/* 非常简单的任务调度：两个任务轮流执行 */
	if( OSTCBCurPtr == OSRdyList[0].HeadPtr )
	{
		OSTCBHighRdyPtr = OSRdyList[1].HeadPtr;
	}
	else
	{
		OSTCBHighRdyPtr = OSRdyList[0].HeadPtr;
	}
#endif
	
	/* 如果当前任务是空闲任务，那么就去尝试执行任务1或者任务2，看看他们的延时时间是否结束
	   如果任务的延时时间均没有到期，那就返回继续执行空闲任务 */
	if( OSTCBCurPtr == &OSIdleTaskTCB )
	{
		if(OSRdyList[0].HeadPtr->TaskDelayTicks == 0)
		{
			OSTCBHighRdyPtr = OSRdyList[0].HeadPtr;
		}
		else if(OSRdyList[1].HeadPtr->TaskDelayTicks == 0)
		{
			OSTCBHighRdyPtr = OSRdyList[1].HeadPtr;
		}
		else
		{
			return;		/* 任务延时均没有到期则返回，继续执行空闲任务 */
		} 
	}
	else
	{
		/*如果是task1或者task2的话，检查下另外一个任务,如果另外的任务不在延时中，就切换到该任务
        否则，判断下当前任务是否应该进入延时状态，如果是的话，就切换到空闲任务。否则就不进行任何切换 */
		if(OSTCBCurPtr == OSRdyList[0].HeadPtr)
		{
			if(OSRdyList[1].HeadPtr->TaskDelayTicks == 0)
			{
				OSTCBHighRdyPtr = OSRdyList[1].HeadPtr;
			}
			else if(OSTCBCurPtr->TaskDelayTicks != 0)
			{
				OSTCBHighRdyPtr = &OSIdleTaskTCB;
			}
			else 
			{
				return;		/* 返回，不进行切换，因为两个任务都处于延时中 */
			}
		}
		else if(OSTCBCurPtr == OSRdyList[1].HeadPtr)
		{
			if(OSRdyList[0].HeadPtr->TaskDelayTicks == 0)
			{
				OSTCBHighRdyPtr = OSRdyList[0].HeadPtr;
			}
			else if(OSTCBCurPtr->TaskDelayTicks != 0)
			{
				OSTCBHighRdyPtr = &OSIdleTaskTCB;
			}
			else 
			{
				return;		/* 返回，不进行切换，因为两个任务都处于延时中 */
			}
		}
	}
	
	/* 任务切换 */
	OS_TASK_SW();
}


/* 空闲任务 */
void  OS_IdleTask (void  *p_arg)
{
	p_arg = p_arg;
	
	/* 空闲任务什么都不做，只对全局变量OSIdleTaskCtr ++ 操作 */
	for(;;)
	{
		OSIdleTaskCtr++;
	}
}

/* 空闲任务初始化 */
void  OS_IdleTaskInit(OS_ERR  *p_err)
{	
	/* 初始化空闲任务计数器 */
	OSIdleTaskCtr = (OS_IDLE_CTR)0;
	
	/* 创建空闲任务 */
	OSTaskCreate( (OS_TCB     *)&OSIdleTaskTCB, 
			      (OS_TASK_PTR )OS_IdleTask, 
			      (void       *)0,
			      (CPU_STK    *)OSCfg_IdleTaskStkBasePtr,
			      (CPU_STK_SIZE)OSCfg_IdleTaskStkSize,
			      (OS_ERR     *)p_err );
}



