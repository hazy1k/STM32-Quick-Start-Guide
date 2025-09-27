/*
*************************************************************************
*                             包含的头文件
*************************************************************************
*/
#include <rtthread.h>
#include <rthw.h>

extern struct rt_thread idle;
extern struct rt_thread rt_flag1_thread;
extern struct rt_thread rt_flag2_thread;

/*
*************************************************************************
*                                 全局变量
*************************************************************************
*/
/* 线程控制块指针，用于指向当前线程 */
struct rt_thread *rt_current_thread;

/* 线程优先级表 */
rt_list_t rt_thread_priority_table[RT_THREAD_PRIORITY_MAX];

/* 线程就绪优先级组 */
rt_uint32_t rt_thread_ready_priority_group;

/* 当前优先级 */
rt_uint8_t rt_current_priority;

/* 线程休眠列表 */
rt_list_t rt_thread_defunct;


/*
*************************************************************************
*                               函数实现
*************************************************************************
*/

/* 初始化系统调度器 */
void rt_system_scheduler_init(void)
{	
#if 0
	register rt_base_t offset;	

	
	/* 线程就绪列表初始化 */
	for (offset = 0; offset < RT_THREAD_PRIORITY_MAX; offset ++)
	{
			rt_list_init(&rt_thread_priority_table[offset]);
	}
	
	/* 初始化当前线程控制块指针 */
	rt_current_thread = RT_NULL;
#else
    register rt_base_t offset;	

	
	/* 线程优先级表初始化 */
	for (offset = 0; offset < RT_THREAD_PRIORITY_MAX; offset ++)
	{
			rt_list_init(&rt_thread_priority_table[offset]);
	}
    
	/* 初始化当前优先级为空闲线程的优先级 */
    rt_current_priority = RT_THREAD_PRIORITY_MAX - 1;
    
	/* 初始化当前线程控制块指针 */
	rt_current_thread = RT_NULL;
    
    /* 初始化线程就绪优先级组 */
    rt_thread_ready_priority_group = 0;
#endif    
}

/* 启动系统调度器 */
void rt_system_scheduler_start(void)
{
#if 0
	register struct rt_thread *to_thread;	

	/* 手动指定第一个运行的线程 */
	to_thread = rt_list_entry(rt_thread_priority_table[0].next,
							  struct rt_thread,
							  tlist);
	rt_current_thread = to_thread;
														
	/* 切换到第一个线程，该函数在context_rvds.S中实现，在rthw.h声明，
       用于实现第一次线程切换。当一个汇编函数在C文件中调用的时候，
       如果有形参，则执行的时候会将形参传人到CPU寄存器r0。*/
	rt_hw_context_switch_to((rt_uint32_t)&to_thread->sp);
#else
    register struct rt_thread *to_thread;
    register rt_ubase_t highest_ready_priority;
                              
    /* 获取就绪的最高优先级 */
    highest_ready_priority = __rt_ffs(rt_thread_ready_priority_group) - 1;
                              
    /* 获取将要运行线程的线程控制块 */
    to_thread = rt_list_entry(rt_thread_priority_table[highest_ready_priority].next,
                              struct rt_thread,
                              tlist);

    rt_current_thread = to_thread;

    /* 切换到新的线程 */
    rt_hw_context_switch_to((rt_uint32_t)&to_thread->sp);

    /* 永远不会返回 */                         
#endif                              
}


void rt_schedule_insert_thread(struct rt_thread *thread)
{
    register rt_base_t temp;

    /* 关中断 */
    temp = rt_hw_interrupt_disable();

    /* 改变线程状态 */
    thread->stat = RT_THREAD_READY;

    /* 将线程插入就绪列表 */
    rt_list_insert_before(&(rt_thread_priority_table[thread->current_priority]),
                          &(thread->tlist));

    /* 设置线程就绪优先级组中对应的位 */
    rt_thread_ready_priority_group |= thread->number_mask;

    /* 开中断 */
    rt_hw_interrupt_enable(temp);
}


void rt_schedule_remove_thread(struct rt_thread *thread)
{
    register rt_base_t temp;


    /* 关中断 */
    temp = rt_hw_interrupt_disable();
    
    /* 将线程从就绪列表删除 */
    rt_list_remove(&(thread->tlist));
    
    if (rt_list_isempty(&(rt_thread_priority_table[thread->current_priority])))
    {
        rt_thread_ready_priority_group &= ~thread->number_mask;
    }

    /* 开中断 */
    rt_hw_interrupt_enable(temp);
}


void rt_schedule(void)
{
    rt_base_t level;
    register rt_ubase_t highest_ready_priority;
    struct rt_thread *to_thread;
    struct rt_thread *from_thread;

    /* 关中断 */
    level = rt_hw_interrupt_disable();

    /* 获取就绪的最高优先级 */
    highest_ready_priority = __rt_ffs(rt_thread_ready_priority_group) - 1;
    /* 获取就绪的最高优先级对应的线程控制块 */
    to_thread = rt_list_entry(rt_thread_priority_table[highest_ready_priority].next,
                              struct rt_thread,
                              tlist);

    /* 如果目标线程不是当前线程，则要进行线程切换 */
    if (to_thread != rt_current_thread)
    {
        rt_current_priority = (rt_uint8_t)highest_ready_priority;
        from_thread         = rt_current_thread;
        rt_current_thread   = to_thread;

        rt_hw_context_switch((rt_uint32_t)&from_thread->sp,
                             (rt_uint32_t)&to_thread->sp);
        
        /* 开中断 */
        rt_hw_interrupt_enable(level);

    }
    else 
    {
        /* 开中断 */
        rt_hw_interrupt_enable(level);
    }
}




