/*
*************************************************************************
*                             包含的头文件
*************************************************************************
*/
#include <rtthread.h>
#include <rthw.h>


/*
*************************************************************************
*                                 全局变量
*************************************************************************
*/
/* 线程控制块指针，用于指向当前线程 */
struct rt_thread *rt_current_thread;

/* 线程就绪列表 */
rt_list_t rt_thread_priority_table[RT_THREAD_PRIORITY_MAX];

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
	register rt_base_t offset;	

	
	/* 线程就绪列表初始化 */
	for (offset = 0; offset < RT_THREAD_PRIORITY_MAX; offset ++)
	{
			rt_list_init(&rt_thread_priority_table[offset]);
	}
	
	/* 初始化当前线程控制块指针 */
	rt_current_thread = RT_NULL;
	
	/* 初始化线程休眠列表，当线程创建好没有启动之前会被放入到这个列表 */
	rt_list_init(&rt_thread_defunct);
}

/* 启动系统调度器 */
void rt_system_scheduler_start(void)
{
	register struct rt_thread *to_thread;
	

	/* 手动指定第一个运行的线程 */
	to_thread = rt_list_entry(rt_thread_priority_table[0].next,
							  struct rt_thread,
							  tlist);
	rt_current_thread = to_thread;
														
	/* 切换到第一个线程，该函数在context_rvds.S中实现，在rthw.h声明，
       用于实现第一次任务切换。当一个汇编函数在C文件中调用的时候，
       如果有形参，则执行的时候会将形参传人到CPU寄存器r0。*/
	rt_hw_context_switch_to((rt_uint32_t)&to_thread->sp);
}

/* 系统调度 */
void rt_schedule(void)
{
	struct rt_thread *to_thread;
	struct rt_thread *from_thread;
	
	
	
	/* 两个线程轮流切换 */
	if( rt_current_thread == rt_list_entry( rt_thread_priority_table[0].next,
													               struct rt_thread,
													               tlist) )
	{
		from_thread = rt_current_thread;
		to_thread = rt_list_entry( rt_thread_priority_table[1].next,
													               struct rt_thread,
													               tlist);
	  rt_current_thread = to_thread;
	}
	else
	{
		from_thread = rt_current_thread;
		to_thread = rt_list_entry( rt_thread_priority_table[0].next,
																		 struct rt_thread,
																		 tlist);
	  rt_current_thread = to_thread;																		 
	}
	
	/* 产生上下文切换 */
	rt_hw_context_switch((rt_uint32_t)&from_thread->sp,(rt_uint32_t)&to_thread->sp);	
	
}
