#include "os.h"



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
	
	/* 初始化优先级变量 */
	OSPrioCur = (OS_PRIO)0;           
    OSPrioHighRdy = (OS_PRIO)0;
	
	/* 初始化优先级表 */
	OS_PrioInit();
	
	/* 初始化就绪列表 */
	OS_RdyListInit();
	
	/* 如果有时基任务，这两个初始化应该放到时基任务里面 */
	/* 时基计数器初始化 */
	OSTickCtr         = (OS_TICK)0u;
	/* 时基列表初始化 */
	OS_TickListInit();
	
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
#if 0		
		/* 手动配置任务1先运行 */
		OSTCBHighRdyPtr = OSRdyList[0].HeadPtr;
#endif
		OSPrioHighRdy   = OS_PrioGetHighest();		            /* 寻找最高的优先级 */
		OSPrioCur       = OSPrioHighRdy;
		
		OSTCBHighRdyPtr = OSRdyList[OSPrioHighRdy].HeadPtr;		/* 找到最高优先级的TCB */
		OSTCBCurPtr     = OSTCBHighRdyPtr;
		
		OSRunning       = OS_STATE_OS_RUNNING;                  /* 标记OS开始运行 */
		
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
	CPU_SR_ALLOC();
	
	/* 进入临界区 */
	OS_CRITICAL_ENTER();
	
	/* 查找最高优先级的任务 */
	OSPrioHighRdy   = OS_PrioGetHighest();
	OSTCBHighRdyPtr = OSRdyList[OSPrioHighRdy].HeadPtr;	
	
	/* 如果最高优先级的任务是当前任务则直接返回，不进行任务切换 */
	if (OSTCBHighRdyPtr == OSTCBCurPtr)
	{
		/* 退出临界区 */
		OS_CRITICAL_EXIT();
		
		return;
	}	
		/* 退出临界区 */
	OS_CRITICAL_EXIT();
	
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
				  (OS_PRIO)(OS_CFG_PRIO_MAX - 1u),
			      (CPU_STK    *)OSCfg_IdleTaskStkBasePtr,
			      (CPU_STK_SIZE)OSCfg_IdleTaskStkSize,
				  (OS_TICK       )0,
			      (OS_ERR     *)p_err );
}

/*
*********************************************************************************************************
*                                           就绪列表相关函数
*********************************************************************************************************
*/
/* 就绪列表初始化 */
/*
*              'OS_RDY_LIST  OSRdyTbl[OS_CFG_PRIO_MAX]'  初始化之后就像下面这样：
*
*                               +---------------+--------------+
*                               |               | TailPtr      |-----> 0
*                          [0]  | NbrEntries=0  +--------------+
*                               |               | HeadPtr      |-----> 0
*                               +---------------+--------------+
*                               |               | TailPtr      |-----> 0
*                          [1]  | NbrEntries=0  +--------------+
*                               |               | HeadPtr      |-----> 0
*                               +---------------+--------------+
*                                       :              :
*                                       :              :
*                                       :              :
*                               +---------------+--------------+
*                               |               | TailPtr      |-----> 0
*          [OS_CFG_PRIO_MAX-1]  | NbrEntries=0  +--------------+
*                               |               | HeadPtr      |-----> 0
*                               +---------------+--------------+
*/
void OS_RdyListInit(void)
{
	OS_PRIO i;
	OS_RDY_LIST *p_rdy_list;
	
	for( i=0u; i<OS_CFG_PRIO_MAX; i++ )
	{
		p_rdy_list = &OSRdyList[i];
		p_rdy_list->NbrEntries = (OS_OBJ_QTY)0;
		p_rdy_list->HeadPtr = (OS_TCB *)0;
		p_rdy_list->TailPtr = (OS_TCB *)0;
	}
}


/* 在就绪链表中插入一个TCB */
void  OS_RdyListInsert (OS_TCB  *p_tcb)
{
    /* 将优先级插入到优先级表 */
	OS_PrioInsert(p_tcb->Prio);
	
    if (p_tcb->Prio == OSPrioCur) 
	{
        /* 如果是当前优先级则插入到链表尾部 */
		OS_RdyListInsertTail(p_tcb);
    } 
	else 
	{
        /* 否则插入到链表头部 */
		OS_RdyListInsertHead(p_tcb);
    }
}

/* 在就绪链表头部插入一个TCB */

	/*
	*  在链表头部插入一个TCB有以下两种情况
	*              CASE 0: 插入到一个空的链表
	*
	*                     OS_RDY_LIST
	*                     +--------------+
	*                     | TailPtr      |-> 0
	*                     +--------------+
	*                     | HeadPtr      |-> 0
	*                     +--------------+
	*                     | NbrEntries=0 |
	*                     +--------------+
	*
	*
	*
	*              CASE 1: 链表已经有节点
	*
	*                     OS_RDY_LIST
	*                     +--------------+          OS_TCB
	*                     | TailPtr      |--+---> +------------+
	*                     +--------------+  |     | NextPtr    |->0
	*                     | HeadPtr      |--/     +------------+
	*                     +--------------+     0<-| PrevPtr    |
	*                     | NbrEntries=1 |        +------------+
	*                     +--------------+        :            :
	*                                             :            :
	*                                             +------------+
	*
	*
	*                     OS_RDY_LIST
	*                     +--------------+
	*                     | TailPtr      |-----------------------------------------------+
	*                     +--------------+          OS_TCB               OS_TCB          |     OS_TCB
	*                     | HeadPtr      |------> +------------+       +------------+    +-> +------------+
	*                     +--------------+        | NextPtr    |------>| NextPtr    | ...... | NextPtr    |->0
	*                     | NbrEntries=N |        +------------+       +------------+        +------------+
	*                     +--------------+     0<-| PrevPtr    |<------| PrevPtr    | ...... | PrevPtr    |
	*                                             +------------+       +------------+        +------------+
	*                                             :            :       :            :        :            :
	*                                             :            :       :            :        :            :
	*                                             +------------+       +------------+        +------------+
	*/
	
void  OS_RdyListInsertHead (OS_TCB  *p_tcb)
{
    OS_RDY_LIST  *p_rdy_list;
    OS_TCB       *p_tcb2;



    /* 获取链表根部 */
	p_rdy_list = &OSRdyList[p_tcb->Prio];
	
	/* CASE 0: 链表是空链表 */
    if (p_rdy_list->NbrEntries == (OS_OBJ_QTY)0) 
	{
		p_rdy_list->NbrEntries =  (OS_OBJ_QTY)1;            
        p_tcb->NextPtr         =  (OS_TCB   *)0;            
        p_tcb->PrevPtr         =  (OS_TCB   *)0;
        p_rdy_list->HeadPtr    =  p_tcb;                    
        p_rdy_list->TailPtr    =  p_tcb;
    }
	/* CASE 1: 链表已有节点 */	
	else 
	{                                               
        p_rdy_list->NbrEntries++;                           
        p_tcb->NextPtr         = p_rdy_list->HeadPtr;       
        p_tcb->PrevPtr         = (OS_TCB    *)0;
        p_tcb2                 = p_rdy_list->HeadPtr;
        p_tcb2->PrevPtr        = p_tcb;
        p_rdy_list->HeadPtr    = p_tcb;
    }
}


/* 在就绪链表尾部插入一个TCB */

	/*
	*  在就绪链表尾部插入一个TCB 有以下 两种情况
	*              CASE 0: 链表是空的
	*
	*                     OS_RDY_LIST
	*                     +--------------+
	*                     | TailPtr      |-> 0
	*                     +--------------+
	*                     | HeadPtr      |-> 0
	*                     +--------------+
	*                     | NbrEntries=0 |
	*                     +--------------+
	*
	*
	*
	*              CASE 1: 链表已有节点
	*
	*                     OS_RDY_LIST
	*                     +--------------+          OS_TCB
	*                     | TailPtr      |--+---> +------------+
	*                     +--------------+  |     | NextPtr    |->0
	*                     | HeadPtr      |--/     +------------+
	*                     +--------------+     0<-| PrevPtr    |
	*                     | NbrEntries=1 |        +------------+
	*                     +--------------+        :            :
	*                                             :            :
	*                                             +------------+
	*
	*
	*                     OS_RDY_LIST
	*                     +--------------+
	*                     | TailPtr      |-----------------------------------------------+
	*                     +--------------+          OS_TCB               OS_TCB          |     OS_TCB
	*                     | HeadPtr      |------> +------------+       +------------+    +-> +------------+
	*                     +--------------+        | NextPtr    |------>| NextPtr    | ...... | NextPtr    |->0
	*                     | NbrEntries=N |        +------------+       +------------+        +------------+
	*                     +--------------+     0<-| PrevPtr    |<------| PrevPtr    | ...... | PrevPtr    |
	*                                             +------------+       +------------+        +------------+
	*                                             :            :       :            :        :            :
	*                                             :            :       :            :        :            :
	*                                             +------------+       +------------+        +------------+
	*/
	
void  OS_RdyListInsertTail (OS_TCB  *p_tcb)
{
    OS_RDY_LIST  *p_rdy_list;
    OS_TCB       *p_tcb2;


    /* 获取链表根部 */
    p_rdy_list = &OSRdyList[p_tcb->Prio];
	
	/* CASE 0: 链表是空链表 */
    if (p_rdy_list->NbrEntries == (OS_OBJ_QTY)0) 
	{          
        p_rdy_list->NbrEntries  = (OS_OBJ_QTY)1;            
        p_tcb->NextPtr          = (OS_TCB   *)0;           
        p_tcb->PrevPtr          = (OS_TCB   *)0;
        p_rdy_list->HeadPtr     = p_tcb;                   
        p_rdy_list->TailPtr     = p_tcb;
    } 
	/* CASE 1: 链表已有节点 */
	else 
	{                                               
        p_rdy_list->NbrEntries++;                           
        p_tcb->NextPtr          = (OS_TCB   *)0;            
        p_tcb2                  = p_rdy_list->TailPtr;
        p_tcb->PrevPtr          = p_tcb2;
        p_tcb2->NextPtr         = p_tcb;                   
        p_rdy_list->TailPtr     = p_tcb;
    }
}


/* 将链表中的节点从头部移动到尾部 */

	/*
	*  将链表中的节点从头部移动到尾部，包含下面三种情况：
	*              CASE 0: 链表为空，无事可做。
	*
	*              CASE 1: 链表只有一个节点，无事可做。
	*
	*              CASE 2: 链表只有两个节点。
	*
	*                     OS_RDY_LIST
	*                     +--------------+
	*                     | TailPtr      |--------------------------+
	*                     +--------------+          OS_TCB          |     OS_TCB
	*                     | HeadPtr      |------> +------------+    +-> +------------+
	*                     +--------------+        | NextPtr    |------> | NextPtr    |->0
	*                     | NbrEntries=2 |        +------------+        +------------+
	*                     +--------------+     0<-| PrevPtr    | <------| PrevPtr    |
	*                                             +------------+        +------------+
	*                                             :            :        :            :
	*                                             :            :        :            :
	*                                             +------------+        +------------+
	*
	*
	*              CASE N: 链表有两个节点以上。
	*
	*                     OS_RDY_LIST
	*                     +--------------+
	*                     | TailPtr      |-----------------------------------------------+
	*                     +--------------+          OS_TCB               OS_TCB          |     OS_TCB
	*                     | HeadPtr      |------> +------------+       +------------+    +-> +------------+
	*                     +--------------+        | NextPtr    |------>| NextPtr    | ...... | NextPtr    |->0
	*                     | NbrEntries=N |        +------------+       +------------+        +------------+
	*                     +--------------+     0<-| PrevPtr    |<------| PrevPtr    | ...... | PrevPtr    |
	*                                             +------------+       +------------+        +------------+
	*                                             :            :       :            :        :            :
	*                                             :            :       :            :        :            :
	*                                             +------------+       +------------+        +------------+
	*/
	
void  OS_RdyListMoveHeadToTail (OS_RDY_LIST  *p_rdy_list)
{
    OS_TCB  *p_tcb1;
    OS_TCB  *p_tcb2;
    OS_TCB  *p_tcb3;



    switch (p_rdy_list->NbrEntries) 
	{
        case 0:
        case 1:
             break;

        case 2:                                             
             p_tcb1              = p_rdy_list->HeadPtr;    
             p_tcb2              = p_rdy_list->TailPtr;    
             p_tcb1->PrevPtr     = p_tcb2;
             p_tcb1->NextPtr     = (OS_TCB *)0;
             p_tcb2->PrevPtr     = (OS_TCB *)0;
             p_tcb2->NextPtr     = p_tcb1;
             p_rdy_list->HeadPtr = p_tcb2;
             p_rdy_list->TailPtr = p_tcb1;
             break;

        default:                                            
             p_tcb1              = p_rdy_list->HeadPtr;     
             p_tcb2              = p_rdy_list->TailPtr;     
             p_tcb3              = p_tcb1->NextPtr;         
             p_tcb3->PrevPtr     = (OS_TCB *)0;            
             p_tcb1->NextPtr     = (OS_TCB *)0;           
             p_tcb1->PrevPtr     = p_tcb2;                  
             p_tcb2->NextPtr     = p_tcb1;                 
             p_rdy_list->HeadPtr = p_tcb3;                  
             p_rdy_list->TailPtr = p_tcb1;
             break;
    }
}

/* 从链表中移除一个TCB */

	/*
	*  从链表中移除一个TCB有以下几种情况：
	*              CASE 0: 链表为空，无事可做。
	*
	*              CASE 1: 链表只有一个节点。
	*
	*                     OS_RDY_LIST
	*                     +--------------+          OS_TCB
	*                     | TailPtr      |--+---> +------------+
	*                     +--------------+  |     | NextPtr    |->0
	*                     | HeadPtr      |--/     +------------+
	*                     +--------------+     0<-| PrevPtr    |
	*                     | NbrEntries=1 |        +------------+
	*                     +--------------+        :            :
	*                                             :            :
	*                                             +------------+
	*
	*              CASE N: 链表中有两个以上的节点。
	*
	*                     OS_RDY_LIST
	*                     +--------------+
	*                     | TailPtr      |-----------------------------------------------+
	*                     +--------------+          OS_TCB               OS_TCB          |     OS_TCB
	*                     | HeadPtr      |------> +------------+       +------------+    +-> +------------+
	*                     +--------------+        | NextPtr    |------>| NextPtr    | ...... | NextPtr    |->0
	*                     | NbrEntries=N |        +------------+       +------------+        +------------+
	*                     +--------------+     0<-| PrevPtr    |<------| PrevPtr    | ...... | PrevPtr    |
	*                                             +------------+       +------------+        +------------+
	*                                             :            :       :            :        :            :
	*                                             :            :       :            :        :            :
	*                                             +------------+       +------------+        +------------+
	*/

void  OS_RdyListRemove (OS_TCB  *p_tcb)
{
    OS_RDY_LIST  *p_rdy_list;
    OS_TCB       *p_tcb1;
    OS_TCB       *p_tcb2;



    p_rdy_list = &OSRdyList[p_tcb->Prio];
	
	/* 保存要删除的TCB节点的前一个和后一个节点 */
    p_tcb1     = p_tcb->PrevPtr;
    p_tcb2     = p_tcb->NextPtr;
	
	/* 要移除的TCB节点是链表中的第一个节点 */
    if (p_tcb1 == (OS_TCB *)0) 
	{                            
        /* 且该链表中只有一个节点 */
		if (p_tcb2 == (OS_TCB *)0) 
		{                       
            /* 根节点全部初始化为0 */
			p_rdy_list->NbrEntries = (OS_OBJ_QTY)0;
            p_rdy_list->HeadPtr    = (OS_TCB   *)0;
            p_rdy_list->TailPtr    = (OS_TCB   *)0;
			
            /* 清除在优先级表中相应的位 */
			OS_PrioRemove(p_tcb->Prio);
        }
		/* 该链表中不止一个节点 */		
		else 
		{
            /* 节点减1 */
			p_rdy_list->NbrEntries--;
            p_tcb2->PrevPtr        = (OS_TCB   *)0;        
            p_rdy_list->HeadPtr    = p_tcb2; 
        }
    }
	/* 要移除的TCB节点不是链表中的第一个节点 */	
	else 
	{
        p_rdy_list->NbrEntries--;                          
        p_tcb1->NextPtr = p_tcb2;
		
        /* 如果要删除的节点的下一个节点是0，即要删除的节点是最后一个节点 */
		if (p_tcb2 == (OS_TCB *)0) 
		{
            p_rdy_list->TailPtr = p_tcb1;
        } 
		else 
		{
            p_tcb2->PrevPtr     = p_tcb1;
        }
    }
	
	/* 复位从就绪列表中删除的TCB的PrevPtr和NextPtr这两个指针 */
    p_tcb->PrevPtr = (OS_TCB *)0;
    p_tcb->NextPtr = (OS_TCB *)0;
}


/*
*********************************************************************************************************
*                                             任务就绪函数
*********************************************************************************************************
*/
void  OS_TaskRdy (OS_TCB  *p_tcb)
{
    /* 从时基列表删除 */
	OS_TickListRemove(p_tcb);

	/* 插入就绪列表 */
	OS_RdyListInsert(p_tcb);
}


/*
*********************************************************************************************************
*                                             时间片调度
*********************************************************************************************************
*/
#if OS_CFG_SCHED_ROUND_ROBIN_EN > 0u
void OS_SchedRoundRobin(OS_RDY_LIST  *p_rdy_list)
{
	OS_TCB   *p_tcb;
    CPU_SR_ALLOC();
	
	/*  进入临界段 */
	CPU_CRITICAL_ENTER();
	
	p_tcb = p_rdy_list->HeadPtr;
	
	/* 如果TCB节点为空，则退出 */
    if (p_tcb == (OS_TCB *)0) 
	{
        CPU_CRITICAL_EXIT();
        return;
    }

    /* 如果是空闲任务，也退出 */
	if (p_tcb == &OSIdleTaskTCB) 
	{
        CPU_CRITICAL_EXIT();
        return;
    }

    /* 时间片自减 */
	if (p_tcb->TimeQuantaCtr > (OS_TICK)0) 
	{
        p_tcb->TimeQuantaCtr--;
    }

    /* 时间片没有用完，则退出 */
	if (p_tcb->TimeQuantaCtr > (OS_TICK)0) 
	{               
        CPU_CRITICAL_EXIT();
        return;
    }

    /* 如果当前优先级只有一个任务，则退出 */
	if (p_rdy_list->NbrEntries < (OS_OBJ_QTY)2) 
	{           
        CPU_CRITICAL_EXIT();                        
        return;
    }	
	
	/* 时间片耗完，将任务放到链表的最后一个节点 */
	OS_RdyListMoveHeadToTail(p_rdy_list);
	
	/* 重新获取任务节点 */
	p_tcb = p_rdy_list->HeadPtr;
	/* 重载默认的时间片计数值 */
	p_tcb->TimeQuantaCtr = p_tcb->TimeQuanta;
	
	/* 退出临界段 */
	CPU_CRITICAL_EXIT();
}
#endif 	/* OS_CFG_SCHED_ROUND_ROBIN_EN > 0u */















