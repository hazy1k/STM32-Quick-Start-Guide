#include <rtthread.h>
#include <rthw.h>

extern struct rt_thread *rt_current_thread;
extern rt_uint32_t rt_thread_ready_priority_group;
extern rt_list_t rt_thread_priority_table[RT_THREAD_PRIORITY_MAX];


rt_err_t rt_thread_init(struct rt_thread *thread,
                        const char       *name,
                        void (*entry)(void *parameter),
                        void             *parameter,
                        void             *stack_start,
                        rt_uint32_t       stack_size,
                        rt_uint8_t        priority,
                        rt_uint32_t       tick)
{
	/* 线程对象初始化 */
	/* 线程结构体开头部分的成员就是rt_object_t类型 */
	rt_object_init((rt_object_t)thread, RT_Object_Class_Thread, name);
    rt_list_init(&(thread->tlist));
	
	thread->entry = (void *)entry;
	thread->parameter = parameter;

	thread->stack_addr = stack_start;
	thread->stack_size = stack_size;
	
	/* 初始化线程栈，并返回线程栈指针 */
	thread->sp = (void *)rt_hw_stack_init( thread->entry, 
		                                   thread->parameter,
							               (void *)((char *)thread->stack_addr + thread->stack_size - 4) );
	
    thread->init_priority    = priority;
    thread->current_priority = priority;
    thread->number_mask = 0;
    
    /* 错误码和状态 */
    thread->error = RT_EOK;
    thread->stat  = RT_THREAD_INIT;
    
    /* 时间片相关 */
    thread->init_tick      = tick;
    thread->remaining_tick = tick;
    
    /* 初始化线程定时器 */
    rt_timer_init(&(thread->thread_timer),     /* 静态定时器对象 */
                  thread->name,                /* 定时器的名字，直接使用的是线程的名字 */
                  rt_thread_timeout,           /* 超时函数 */
                  thread,                      /* 超时函数形参 */
                  0,                           /* 延时时间 */
                  RT_TIMER_FLAG_ONE_SHOT);     /* 定时器的标志 */
    
    return RT_EOK;
}


/**
 * 该函数用于挂起指定的线程
 * @param thread 要被挂起的线程
 *
 * @return 操作状态, RT_EOK on OK, -RT_ERROR on error
 *
 * @note 如果挂起的是线程自身，在调用该函数后，
 * 必须调用rt_schedule()进行系统调度
 * 
 */
rt_err_t rt_thread_suspend(rt_thread_t thread)
{
    register rt_base_t temp;


    /* 只有就绪的线程才能被挂起，否则退出返回错误码 */
    if ((thread->stat & RT_THREAD_STAT_MASK) != RT_THREAD_READY)
    {
        return -RT_ERROR;
    }

    /* 关中断 */
    temp = rt_hw_interrupt_disable();

    /* 改变线程状态 */
    thread->stat = RT_THREAD_SUSPEND;
    /* 将线程从就绪列表删除 */
    rt_schedule_remove_thread(thread);

    /* 停止线程定时器 */
    rt_timer_stop(&(thread->thread_timer));

    /* 开中断 */
    rt_hw_interrupt_enable(temp);

    return RT_EOK;
}

/**
 * 该函数将让当前线程睡眠一段时间，单位为tick
 * 
 * @param tick 睡眠时间，单位为tick
 *
 * @return RT_EOK
 */
rt_err_t rt_thread_sleep(rt_tick_t tick)
{
    register rt_base_t temp;
    struct rt_thread *thread;

    /* 关中断 */
    temp = rt_hw_interrupt_disable();
    
    /* 获取当前线程的线程控制块 */
    thread = rt_current_thread;

    /* 挂起线程 */
    rt_thread_suspend(thread);

    /* 设置线程定时器的超时时间 */
    rt_timer_control(&(thread->thread_timer), RT_TIMER_CTRL_SET_TIME, &tick);
    
    /* 启动定时器 */
    rt_timer_start(&(thread->thread_timer));

    /* 开中断 */
    rt_hw_interrupt_enable(temp);

    /* 执行系统调度 */
    rt_schedule();

    return RT_EOK;
}


#if 0
void rt_thread_delay(rt_tick_t tick)
{
    register rt_base_t temp;
    struct rt_thread *thread;
    
	/* 失能中断 */
    temp = rt_hw_interrupt_disable();
    
    thread = rt_current_thread;
    thread->remaining_tick = tick;
    
    /* 改变线程状态 */
    thread->stat = RT_THREAD_SUSPEND;
    rt_thread_ready_priority_group &= ~thread->number_mask;

    /* 使能中断 */
    rt_hw_interrupt_enable(temp);
    
	/* 进行系统调度 */
	rt_schedule();
}
#else
rt_err_t rt_thread_delay(rt_tick_t tick)
{
    return rt_thread_sleep(tick);
}    
#endif


/**
 * 该函数用于恢复一个线程然后将其放到就绪列表
 *
 * @param thread 需要被恢复的线程
 *
 * @return 操作状态, RT_EOK on OK, -RT_ERROR on error
 */
rt_err_t rt_thread_resume(rt_thread_t thread)
{
    register rt_base_t temp;
    
    /* 将被恢复的线程必须在挂起态，否则返回错误码 */
    if ((thread->stat & RT_THREAD_STAT_MASK) != RT_THREAD_SUSPEND)
    {
        return -RT_ERROR;
    }

    /* 关中断 */
    temp = rt_hw_interrupt_disable();

    /* 从挂起队列移除 */
    rt_list_remove(&(thread->tlist));

    /* 开中断 */
    rt_hw_interrupt_enable(temp);

    /* 插入就绪列表 */
    rt_schedule_insert_thread(thread);

    return RT_EOK;
}

rt_thread_t rt_thread_self(void)
{
    return rt_current_thread;
}

/**
 * 启动一个线程并将其放到系统的就绪列表中
 * 
 * @param thread 待启动的线程
 *
 * @return 操作状态, RT_EOK on OK, -RT_ERROR on error
 */
rt_err_t rt_thread_startup(rt_thread_t thread)
{
    /* 设置当前优先级为初始优先级 */
    thread->current_priority = thread->init_priority;
    thread->number_mask = 1L << thread->current_priority;    
    
    /* 改变线程的状态为挂起状态 */
    thread->stat = RT_THREAD_SUSPEND;    
    /* 然后恢复线程 */
    rt_thread_resume(thread);
    
    if (rt_thread_self() != RT_NULL)
    {
        /* 系统调度 */
        rt_schedule();
    }

    return RT_EOK;
}

/**
 * 线程超时函数
 * 当线程延时到期或者等待的资源可用或者超时时，该函数会被调用
 *
 * @param parameter 超时函数的形参
 */
void rt_thread_timeout(void *parameter)
{
    struct rt_thread *thread;

    thread = (struct rt_thread *)parameter;

    /* 设置错误码为超时 */
    thread->error = -RT_ETIMEOUT;

    /* 将线程从挂起列表中删除 */
    rt_list_remove(&(thread->tlist));

    /* 将线程插入到就绪列表 */
    rt_schedule_insert_thread(thread);

    /* 系统调度 */
    rt_schedule();
}

/**
 * 该函数将让当前线程让出处理器，调度器选择最高优先级的线程运行。当前让出处理器之后，
 * 当前线程还是在就绪态。
 *
 * @return RT_EOK
 */
rt_err_t rt_thread_yield(void)
{
    register rt_base_t level;
    struct rt_thread *thread;

    /* 关中断 */
    level = rt_hw_interrupt_disable();

    /* 获取当前线程的线程控制块 */
    thread = rt_current_thread;
    
    /* 如果线程在就绪态，且同一个优先级下不止一个线程 */
    if ((thread->stat & RT_THREAD_STAT_MASK) == RT_THREAD_READY &&
        thread->tlist.next != thread->tlist.prev)
    {
        /* 将时间片耗完的线程从就绪列表移除 */
        rt_list_remove(&(thread->tlist));
        
        /* 将线程插入到该优先级下的链表的尾部 */
        rt_list_insert_before(&(rt_thread_priority_table[thread->current_priority]),
                              &(thread->tlist));

        /* 开中断 */
        rt_hw_interrupt_enable(level);

        /* 执行调度 */
        rt_schedule();

        return RT_EOK;
    }

    /* 开中断 */
    rt_hw_interrupt_enable(level);

    return RT_EOK;
}
