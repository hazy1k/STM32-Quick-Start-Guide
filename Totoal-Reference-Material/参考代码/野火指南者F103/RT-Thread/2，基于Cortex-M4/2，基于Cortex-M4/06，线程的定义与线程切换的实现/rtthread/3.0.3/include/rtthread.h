#ifndef __RT_THREAD_H__
#define __RT_THREAD_H__

/*
*************************************************************************
*                             包含的头文件
*************************************************************************
*/
#include <rtdef.h>
#include <rtconfig.h>
#include <rtservice.h>

/*
*************************************************************************
*                               函数声明
*************************************************************************
*/
/*
-------------------------------------------------------------------------
*                               线程接口
-------------------------------------------------------------------------
*/
rt_err_t rt_thread_init(struct rt_thread *thread,
                        void (*entry)(void *parameter),
                        void             *parameter,
                        void             *stack_start,
                        rt_uint32_t       stack_size);
												
rt_err_t rt_thread_resume(rt_thread_t thread);
												
rt_err_t rt_thread_startup(rt_thread_t thread);
												
												
												
/*
-------------------------------------------------------------------------
*                             调度器接口
-------------------------------------------------------------------------
*/
void rt_system_scheduler_init(void);
void rt_schedule_insert_thread(struct rt_thread *thread);
void rt_system_scheduler_start(void);

void rt_schedule(void);

#endif /* __RT_THREAD_H__ */

