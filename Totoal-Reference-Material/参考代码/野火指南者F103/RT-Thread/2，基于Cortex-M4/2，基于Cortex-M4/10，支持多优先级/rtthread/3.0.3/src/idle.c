#include <rtthread.h>
#include <rthw.h>

#define IDLE_THREAD_STACK_SIZE      512 

ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t rt_thread_stack[IDLE_THREAD_STACK_SIZE];

struct rt_thread idle;

extern rt_list_t rt_thread_priority_table[RT_THREAD_PRIORITY_MAX];

rt_ubase_t  rt_idletask_ctr = 0;

void rt_thread_idle_entry(void *parameter)
{
    parameter = parameter;
    while (1)
    {
        rt_idletask_ctr ++;
    }
}

/**
 * @ingroup SystemInit
 *
 * 初始化空闲线程，启动空闲线程
 *
 * @note 当系统初始化的时候该函数必须被调用
 */
void rt_thread_idle_init(void)
{
    
    /* 初始化线程 */
    rt_thread_init(&idle,
                   "idle",
                   rt_thread_idle_entry,
                   RT_NULL,
                   &rt_thread_stack[0],
                   sizeof(rt_thread_stack),
                   RT_THREAD_PRIORITY_MAX-1);
    
	/* 将线程插入到就绪列表 */
    //rt_list_insert_before( &(rt_thread_priority_table[RT_THREAD_PRIORITY_MAX-1]),&(idle.tlist) );
    rt_thread_startup(&idle);
}

















