/**
 ****************************************************************************************************
 * @file        uc-os3_demo.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-04-13
 * @brief       uC/OS-III 互斥信号量优化优先级翻转实验
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 Mini Pro H750开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#include "uc-os3_demo.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LCD/lcd.h"
#include "./MALLOC/malloc.h"
/*uC/OS-III*********************************************************************************************/
#include "os.h"
#include "cpu.h"

/******************************************************************************************************/
/*uC/OS-III配置*/

/* START_TASK 任务 配置
 * 包括: 任务优先级 任务栈大小 任务控制块 任务栈 任务函数
 */
#define START_TASK_PRIO 2                               /* 任务优先级 */
#define START_STK_SIZE  512                             /* 任务栈大小 */
OS_TCB                  StartTask_TCB;                  /* 任务控制块 */
CPU_STK                 StartTask_STK[START_STK_SIZE];  /* 任务栈 */
void start_task(void *p_arg);                           /* 任务函数 */

/* TASK1 任务 配置
 * 包括: 任务优先级 任务栈大小 任务控制块 任务栈 任务函数
 */
#define TASK1_PRIO      (OS_CFG_PRIO_MAX - 6)           /* 任务优先级 */
#define TASK1_STK_SIZE  1024                            /* 任务栈大小 */
OS_TCB                  Task1Task_TCB;                  /* 任务控制块 */
CPU_STK                 *Task1Task_STK;                 /* 任务栈 */
void task1(void *p_arg);                                /* 任务函数 */

/* TASK2 任务 配置
 * 包括: 任务优先级 任务栈大小 任务控制块 任务栈 任务函数
 */
#define TASK2_PRIO      (OS_CFG_PRIO_MAX - 5)           /* 任务优先级 */
#define TASK2_STK_SIZE  1024                            /* 任务栈大小 */
OS_TCB                  Task2Task_TCB;                  /* 任务控制块 */
CPU_STK                 *Task2Task_STK;                 /* 任务栈 */
void task2(void *p_arg);                                /* 任务函数 */

/* TASK3 任务 配置
 * 包括: 任务优先级 任务栈大小 任务控制块 任务栈 任务函数
 */
#define TASK3_PRIO      (OS_CFG_PRIO_MAX - 4)           /* 任务优先级 */
#define TASK3_STK_SIZE  1024                            /* 任务栈大小 */
OS_TCB                  Task3Task_TCB;                  /* 任务控制块 */
CPU_STK                 *Task3Task_STK;                 /* 任务栈 */
void task3(void *p_arg);                                /* 任务函数 */

OS_MUTEX mutex;

/******************************************************************************************************/

/**
 * @brief       uC/OS-III例程入口函数
 * @param       无
 * @retval      无
 */
void uc_os3_demo(void)
{
    OS_ERR err;
    
    lcd_show_string(10, 10, 220, 32, 32, "STM32", RED);
    lcd_show_string(10, 47, 220, 24, 24, "Mutex Pri Overturn", RED);
    lcd_show_string(10, 76, 220, 16, 16, "ATOM@ALIENTEK", RED);
    
    /* 初始化uC/OS-III */
    OSInit(&err);
    
    /* 创建Start Task */
    OSTaskCreate(   (OS_TCB        *)&StartTask_TCB,
                    (CPU_CHAR      *)"start_task",
                    (OS_TASK_PTR    )start_task,
                    (void          *)0,
                    (OS_PRIO        )START_TASK_PRIO,
                    (CPU_STK       *)StartTask_STK,
                    (CPU_STK_SIZE   )START_STK_SIZE / 10,
                    (CPU_STK_SIZE   )START_STK_SIZE,
                    (OS_MSG_QTY     )0,
                    (OS_TICK        )0,
                    (void          *)0,
                    (OS_OPT         )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                    (OS_ERR        *)&err);
    
    /* 开始任务调度 */
    OSStart(&err);
    
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
    OS_ERR err;
    CPU_INT32U cnts;
    
    /* 初始化CPU库 */
    CPU_Init();
    
    /* 根据配置的节拍频率配置SysTick */
    cnts = (CPU_INT32U)(HAL_RCC_GetSysClockFreq() / OSCfg_TickRate_Hz);
    OS_CPU_SysTickInit(cnts);
    
    /* 开启时间片调度，时间片设为默认值 */
    OSSchedRoundRobinCfg(OS_TRUE, 0, &err);
    
    /* 创建互斥信号量 */
    OSMutexCreate(  (OS_MUTEX  *)&mutex,
                    (CPU_CHAR  *)"mutex",
                    (OS_ERR    *)&err);
    
    /* 创建Task1 */
    Task1Task_STK = (CPU_STK *)mymalloc(SRAMIN, TASK1_STK_SIZE * sizeof(CPU_STK));
    OSTaskCreate(   (OS_TCB        *)&Task1Task_TCB,
                    (CPU_CHAR      *)"task1",
                    (OS_TASK_PTR    )task1,
                    (void          *)0,
                    (OS_PRIO        )TASK1_PRIO,
                    (CPU_STK       *)Task1Task_STK,
                    (CPU_STK_SIZE   )TASK1_STK_SIZE / 10,
                    (CPU_STK_SIZE   )TASK1_STK_SIZE,
                    (OS_MSG_QTY     )0,
                    (OS_TICK        )0,
                    (void          *)0,
                    (OS_OPT         )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                    (OS_ERR        *)&err);
    
    /* 创建Task2 */
    Task2Task_STK = (CPU_STK *)mymalloc(SRAMIN, TASK2_STK_SIZE * sizeof(CPU_STK));
    OSTaskCreate(   (OS_TCB        *)&Task2Task_TCB,
                    (CPU_CHAR      *)"task2",
                    (OS_TASK_PTR    )task2,
                    (void          *)0,
                    (OS_PRIO        )TASK2_PRIO,
                    (CPU_STK       *)Task2Task_STK,
                    (CPU_STK_SIZE   )TASK2_STK_SIZE / 10,
                    (CPU_STK_SIZE   )TASK2_STK_SIZE,
                    (OS_MSG_QTY     )0,
                    (OS_TICK        )0,
                    (void          *)0,
                    (OS_OPT         )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                    (OS_ERR        *)&err);
    
    /* 创建Task3 */
    Task3Task_STK = (CPU_STK *)mymalloc(SRAMIN, TASK3_STK_SIZE * sizeof(CPU_STK));
    OSTaskCreate(   (OS_TCB        *)&Task3Task_TCB,
                    (CPU_CHAR      *)"task3",
                    (OS_TASK_PTR    )task3,
                    (void          *)0,
                    (OS_PRIO        )TASK3_PRIO,
                    (CPU_STK       *)Task3Task_STK,
                    (CPU_STK_SIZE   )TASK3_STK_SIZE / 10,
                    (CPU_STK_SIZE   )TASK3_STK_SIZE,
                    (OS_MSG_QTY     )0,
                    (OS_TICK        )0,
                    (void          *)0,
                    (OS_OPT         )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                    (OS_ERR        *)&err);
    
    /* 删除Start Task */
    OSTaskDel((OS_TCB *)0, &err);
}

/**
 * @brief       task1
 * @param       p_arg : 传入参数(未用到)
 * @retval      无
 */
void task1(void *p_arg)
{
    OS_ERR err;
    
    OSTimeDly(500, OS_OPT_TIME_DLY, &err);
    while (1)
    {
        printf("task1 ready to pend mutex\r\n");
        OSMutexPend((OS_MUTEX  *)&mutex,
                    (OS_TICK    )0,
                    (OS_OPT     )OS_OPT_PEND_BLOCKING,
                    (CPU_TS    *)0,
                    (OS_ERR    *)&err);
        printf("task1 has pended mutex\r\n");
        printf("task1 runing\r\n");
        printf("task1 post mutex\r\n");
        OSMutexPost((OS_MUTEX  *)&mutex,
                    (OS_OPT     )OS_OPT_POST_NONE,
                    (OS_ERR    *)&err);
        OSTimeDly(100, OS_OPT_TIME_DLY, &err);
    }
}

/**
 * @brief       task2
 * @param       p_arg : 传入参数(未用到)
 * @retval      无
 */
void task2(void *p_arg)
{
    uint32_t task2_num = 0;
    OS_ERR err;
    
    OSTimeDly(200, OS_OPT_TIME_DLY, &err);
    while (1)
    {
        for (task2_num=0; task2_num<5; task2_num++)
        {
            printf("task2 runing\r\n");
            delay_ms(100);
        }
        OSTimeDly(1000, OS_OPT_TIME_DLY, &err);
    }
}

/**
 * @brief       task3
 * @param       p_arg : 传入参数(未用到)
 * @retval      无
 */
void task3(void *p_arg)
{
    uint32_t task3_num = 0;
    OS_ERR err;
    
    while (1)
    {
        printf("task3 ready to pend mutex\r\n");
        OSMutexPend((OS_MUTEX  *)&mutex,
                    (OS_TICK    )0,
                    (OS_OPT     )OS_OPT_PEND_BLOCKING,
                    (CPU_TS    *)0,
                    (OS_ERR    *)&err);
        printf("task3 has pend mutex\r\n");
        for (task3_num=0; task3_num<5; task3_num++)
        {
            printf("task3 runing\r\n");
            delay_ms(100);
        }
        printf("task3 post mutex\r\n");
        OSMutexPost((OS_MUTEX  *)&mutex,
                    (OS_OPT     )OS_OPT_POST_NONE,
                    (OS_ERR    *)&err);
        OSTimeDly(1000, OS_OPT_TIME_DLY, &err);
    }
}
