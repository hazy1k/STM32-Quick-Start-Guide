#include "os.h"


void  OSTimeTick (void)
{
#if 0
	unsigned int i;
	CPU_SR_ALLOC();
	
	/* 进入临界区 */
	OS_CRITICAL_ENTER();
	
	for(i=0; i<OS_CFG_PRIO_MAX; i++)
	{
		if(OSRdyList[i].HeadPtr->TaskDelayTicks > 0)
		{
			OSRdyList[i].HeadPtr->TaskDelayTicks --;
			if(OSRdyList[i].HeadPtr->TaskDelayTicks == 0)
			{
				/* 为0则表示延时时间到，让任务就绪 */			
				//OS_RdyListInsert (OSRdyList[i].HeadPtr);
				OS_PrioInsert(i);
			}
		}
	}
	
	/* 退出临界区 */
	OS_CRITICAL_EXIT();

#endif
	
	/* 更新时基列表 */
	OS_TickListUpdate();
	
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


