/**
 ****************************************************************************************************
 * @file        uc-os2_demo.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-02-14
 * @brief       UCOSII实验1-任务调度 实验
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台：实验平台:正点原子 Mini Pro H750开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#include "uc-os2_demo.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/LED/led.h"
/*uC/OS-III*********************************************************************************************/
#include "os.h"
#include "cpu.h"

/******************************************************************************************************/
/* UCOSII任务设置 */

/* START 任务 配置
 * 包括: 任务优先级 堆栈大小 等
 */
#define START_TASK_PRIO                 10      /* 开始任务的优先级设置为最低 */
#define START_STK_SIZE                  128     /* 堆栈大小 */

OS_STK START_TASK_STK[START_STK_SIZE];          /* 任务堆栈 */
void start_task(void *p_arg);                   /* 任务函数 */


/* LED0 任务 配置
 * 包括: 任务优先级 堆栈大小 等
 */
#define LED0_TASK_PRIO                  7       /* 开始任务的优先级设置为最低 */
#define LED0_STK_SIZE                   128     /* 堆栈大小 */
OS_STK LED0_TASK_STK[LED0_STK_SIZE];            /* 任务堆栈 */
void led0_task(void *p_arg);                    /* 任务函数 */

/* LED1 任务 配置
 * 包括: 任务优先级 堆栈大小 等
 */
#define LED1_TASK_PRIO                  6       /* 开始任务的优先级设置为最低 */
#define LED1_STK_SIZE                   128     /* 堆栈大小 */
OS_STK LED1_TASK_STK[LED0_STK_SIZE];            /* 任务堆栈 */
void led1_task(void *p_arg);                    /* 任务函数 */

/******************************************************************************************************/
/**
 * @brief       uC/OS-II例程入口函数
 * @param       无
 * @retval      无
 */
void uc_os2_demo(void)
{
    OSInit();                                                   /* UCOS初始化 */
    OSTaskCreateExt((void(*)(void *) )start_task,               /* 任务函数 */
                    (void *          )0,                        /* 传递给任务函数的参数 */
                    (OS_STK *        )&START_TASK_STK[START_STK_SIZE - 1], /* 任务堆栈栈顶 */
                    (INT8U           )START_TASK_PRIO,          /* 任务优先级 */
                    (INT16U          )START_TASK_PRIO,          /* 任务ID，这里设置为和优先级一样 */
                    (OS_STK *        )&START_TASK_STK[0],       /* 任务堆栈栈底 */
                    (INT32U          )START_STK_SIZE,           /* 任务堆栈大小 */
                    (void *          )0,                        /* 用户补充的存储区 */
                    (INT16U          )OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR | OS_TASK_OPT_SAVE_FP); /* 任务选项,为了保险起见，所有任务都保存浮点寄存器的值 */
    OSStart();  /* 开始任务 */
    
    for (;;)
    {
        /* 不会进入这里 */
    }
}

/**
 * @brief       start_task
 * @param       p_arg : 传入参数(未用到)
 * @retval      无
 */
void start_task(void *p_arg)
{
    OS_CPU_SR cpu_sr = 0;
    CPU_INT32U cnts;

    OSStatInit();                   /* 开启统计任务 */
    
    /* 根据配置的节拍频率配置SysTick */
    cnts = (CPU_INT32U)((480 * 1000000) / OS_TICKS_PER_SEC);
    OS_CPU_SysTickInit(cnts);
    OS_ENTER_CRITICAL();            /* 进入临界区(关闭中断) */
    
    OSTaskCreateExt((void(*)(void *) )led0_task,
                    (void *          )0,
                    (OS_STK *        )&LED0_TASK_STK[LED0_STK_SIZE - 1],
                    (INT8U           )LED0_TASK_PRIO,
                    (INT16U          )LED0_TASK_PRIO,
                    (OS_STK *        )&LED0_TASK_STK[0],
                    (INT32U          )LED0_STK_SIZE,
                    (void *          )0,
                    (INT16U          )OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR | OS_TASK_OPT_SAVE_FP);
    /* LED1任务 */
    OSTaskCreateExt((void(*)(void *) )led1_task,
                    (void *          )0,
                    (OS_STK *        )&LED1_TASK_STK[LED1_STK_SIZE - 1],
                    (INT8U           )LED1_TASK_PRIO,
                    (INT16U          )LED1_TASK_PRIO,
                    (OS_STK *        )&LED1_TASK_STK[0],
                    (INT32U          )LED1_STK_SIZE,
                    (void *          )0,
                    (INT16U          )OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR | OS_TASK_OPT_SAVE_FP);
                    
    OS_EXIT_CRITICAL();             /* 退出临界区(开中断) */
    OSTaskSuspend(START_TASK_PRIO); /* 挂起开始任务 */
}
/**
 * @brief       LED0任务
 * @param       p_arg : 传入参数(未用到)
 * @retval      无
 */
void led0_task(void *p_arg)
{
    while (1)
    {
        LED0(0);
        OSTimeDly(80);
        LED0(1);
        OSTimeDly(920);
    }
}

/**
 * @brief       LED1任务
 * @param       p_arg : 传入参数(未用到)
 * @retval      无
 */
void led1_task(void *p_arg)
{
    while (1)
    {
        LED1(0);
        OSTimeDly(300);
        LED1(1);
        OSTimeDly(300);
    }
}


