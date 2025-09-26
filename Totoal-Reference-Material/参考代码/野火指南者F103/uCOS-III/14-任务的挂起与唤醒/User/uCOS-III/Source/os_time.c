#include "os.h"


void  OSTimeTick (void)
{	
	/* 更新时基列表 */
	OS_TickListUpdate();
	
#if OS_CFG_SCHED_ROUND_ROBIN_EN > 0u
	/* 时间片调度 */
	OS_SchedRoundRobin(&OSRdyList[OSPrioCur]);
#endif
	
	/* 任务调度 */
	OSSched();
}

/* 阻塞延时 */
void  OSTimeDly(OS_TICK dly)
{
	CPU_SR_ALLOC();
	
	/* 进入临界区 */
	OS_CRITICAL_ENTER();
#if 0	
	/* 设置延时时间 */
	OSTCBCurPtr->TaskDelayTicks = dly;	
	
	/* 从就绪列表中移除 */
	//OS_RdyListRemove(OSTCBCurPtr);
	OS_PrioRemove(OSTCBCurPtr->Prio);
#endif
	
	/* 插入到时基列表 */
	OS_TickListInsert(OSTCBCurPtr, dly);
	
	/* 从就绪列表移除 */
	OS_RdyListRemove(OSTCBCurPtr);
	
	/* 退出临界区 */
	OS_CRITICAL_EXIT();
	
	/* 任务调度 */
	OSSched();	
}


