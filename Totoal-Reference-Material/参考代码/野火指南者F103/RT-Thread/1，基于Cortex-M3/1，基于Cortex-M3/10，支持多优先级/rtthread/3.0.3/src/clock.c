#include <rtthread.h>
#include <rthw.h>

static rt_tick_t rt_tick = 0;
extern rt_list_t rt_thread_priority_table[RT_THREAD_PRIORITY_MAX];
extern rt_uint32_t rt_thread_ready_priority_group;

void rt_tick_increase(void)
{
    rt_ubase_t i;
    struct rt_thread *thread;
    rt_tick ++;
#if 0
	/* 扫描就绪列表中所有线程的remaining_tick，如果不为0，则减1 */
	for(i=0; i<RT_THREAD_PRIORITY_MAX; i++)
	{
        thread = rt_list_entry( rt_thread_priority_table[i].next,
								 struct rt_thread,
							     tlist);
		if(thread->remaining_tick > 0)
		{
			thread->remaining_tick --;
		}
	}
#else
    /* 扫描就绪列表中所有线程的remaining_tick，如果不为0，则减1 */
	for(i=0; i<RT_THREAD_PRIORITY_MAX; i++)
	{
        thread = rt_list_entry( rt_thread_priority_table[i].next,
								 struct rt_thread,
							     tlist);
		if(thread->remaining_tick > 0)
		{
			thread->remaining_tick --;
            if(thread->remaining_tick == 0)
            {
                //rt_schedule_insert_thread(thread);
                rt_thread_ready_priority_group |= thread->number_mask;
            }
		}
	}
#endif    
    /* 任务调度 */
	rt_schedule();
}

