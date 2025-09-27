#include <rtthread.h>
#include <rthw.h>

static rt_tick_t rt_tick = 0;
extern rt_list_t rt_thread_priority_table[RT_THREAD_PRIORITY_MAX];
extern rt_uint32_t rt_thread_ready_priority_group;

#if 0
void rt_tick_increase(void)
{
    rt_ubase_t i;
    struct rt_thread *thread;
    rt_tick ++;

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
    
    /* 任务调度 */
	rt_schedule();
}

#else
void rt_tick_increase(void)
{
    /* 系统时基计数器加1操作,rt_tick是一个全局变量 */
    ++ rt_tick;
    
    /* 扫描系统定时器列表 */
    rt_timer_check();
}
#endif

/**
 * 该函数用于返回操作系统启动到现在的当前 tick，tick是一个全局变量
 * @return current tick
 */
rt_tick_t rt_tick_get(void)
{
    /* return the global tick */
    return rt_tick;
}


