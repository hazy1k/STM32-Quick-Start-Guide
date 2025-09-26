#include "os.h"

void OSTaskCreate (OS_TCB        *p_tcb, 
                   OS_TASK_PTR   p_task, 
                   void          *p_arg,
                   CPU_STK       *p_stk_base,
                   CPU_STK_SIZE  stk_size,
                   OS_ERR        *p_err)
{
	CPU_STK       *p_sp;

	p_sp = OSTaskStkInit (p_task,
	p_arg,
	p_stk_base,
	stk_size);
	p_tcb->StkPtr = p_sp;
	p_tcb->StkSize = stk_size;

	*p_err = OS_ERR_NONE;
}

