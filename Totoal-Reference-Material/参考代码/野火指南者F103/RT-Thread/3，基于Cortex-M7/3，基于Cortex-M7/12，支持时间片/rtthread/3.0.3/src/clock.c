#include <rtthread.h>
#include <rthw.h>

static rt_tick_t rt_tick = 0;
extern rt_list_t rt_thread_priority_table[RT_THREAD_PRIORITY_MAX];
extern rt_uint32_t rt_thread_ready_priority_group;




void rt_tick_increase(void)
{
    struct rt_thread *thread;    
    
    
    /* 系统时基计数器加1操作,rt_tick是一个全局变量 */
    ++ rt_tick;

    /* 获取当前线程线程控制块 */
    thread = rt_thread_self();

    /* 时间片递减 */
    -- thread->remaining_tick;
    
    /* 如果时间片用完，则重置时间片，然后让出处理器 */
    if (thread->remaining_tick == 0)
    {
        /* 重置时间片 */
        thread->remaining_tick = thread->init_tick;

        /* 让出处理器 */
        rt_thread_yield();
    }
    
    /* 扫描系统定时器列表 */
    rt_timer_check();
}


/**
 * 该函数用于返回操作系统启动到现在的当前 tick，tick是一个全局变量
 * @return current tick
 */
rt_tick_t rt_tick_get(void)
{
    /* 返回全局变量 rt_tick */
    return rt_tick;
}


