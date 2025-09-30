/**
 ****************************************************************************************************
 * @file        freertos.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.4
 * @date        2022-01-04
 * @brief       FreeRTOS 任务创建与删除实验(静态方法)
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

#include "freertos_demo.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/KEY/key.h"
/*FreeRTOS*********************************************************************************************/
#include "FreeRTOS.h"
#include "task.h"

/******************************************************************************************************/
/*FreeRTOS配置*/

static StackType_t  IdleTaskStack[configMINIMAL_STACK_SIZE];        /* 空闲任务任务堆栈 */
static StaticTask_t IdleTaskTCB;                                    /* 空闲任务控制块 */
static StackType_t  TimerTaskStack[configTIMER_TASK_STACK_DEPTH];   /* 定时器服务任务堆栈 */
static StaticTask_t TimerTaskTCB;                                   /* 定时器服务任务控制块 */

/**
 * @brief       获取空闲任务地任务堆栈和任务控制块内存，因为本例程使用的
                静态内存，因此空闲任务的任务堆栈和任务控制块的内存就应该
                有用户来提供，FreeRTOS提供了接口函数vApplicationGetIdleTaskMemory()
                实现此函数即可。
 * @param       ppxIdleTaskTCBBuffer:任务控制块内存
                ppxIdleTaskStackBuffer:任务堆栈内存
                pulIdleTaskStackSize:任务堆栈大小
 * @retval      无
 */
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, 
                                   StackType_t  **ppxIdleTaskStackBuffer, 
                                   uint32_t     *pulIdleTaskStackSize)
{
    *ppxIdleTaskTCBBuffer   = &IdleTaskTCB;
    *ppxIdleTaskStackBuffer = IdleTaskStack;
    *pulIdleTaskStackSize   = configMINIMAL_STACK_SIZE;
}

/**
 * @brief       获取定时器服务任务的任务堆栈和任务控制块内存
 * @param       ppxTimerTaskTCBBuffer:任务控制块内存
                ppxTimerTaskStackBuffer:任务堆栈内存
                pulTimerTaskStackSize:任务堆栈大小
 * @retval      无
 */
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
                                    StackType_t  **ppxTimerTaskStackBuffer,
                                    uint32_t     *pulTimerTaskStackSize)
{
    *ppxTimerTaskTCBBuffer  = &TimerTaskTCB;
    *ppxTimerTaskStackBuffer= TimerTaskStack;
    *pulTimerTaskStackSize  = configTIMER_TASK_STACK_DEPTH;
}

/* START_TASK 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define START_TASK_PRIO 1                   /* 任务优先级 */
#define START_STK_SIZE  128                 /* 任务堆栈大小 */
StackType_t StartTaskStack[START_STK_SIZE]; /* 任务堆栈 */
StaticTask_t            StartTaskTCB;       /* 任务控制块 */
TaskHandle_t            StartTask_Handler;  /* 任务句柄 */
void start_task(void *pvParameters);        /* 任务函数 */

/* TASK1 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK1_PRIO      2                   /* 任务优先级 */
#define TASK1_STK_SIZE  128                 /* 任务堆栈大小 */
StackType_t Task1TaskStack[TASK1_STK_SIZE]; /* 任务堆栈 */
StaticTask_t            Task1TaskTCB;       /* 任务控制块 */
TaskHandle_t            Task1Task_Handler;  /* 任务句柄 */
void task1(void *pvParameters);             /* 任务函数 */

/* TASK2 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK2_PRIO      3                   /* 任务优先级 */
#define TASK2_STK_SIZE  128                 /* 任务堆栈大小 */
StackType_t Task2TaskStack[TASK2_STK_SIZE]; /* 任务堆栈 */
StaticTask_t            Task2TaskTCB;       /* 任务控制块 */
TaskHandle_t            Task2Task_Handler;  /* 任务句柄 */
void task2(void *pvParameters);             /* 任务函数 */

/* TASK3 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK3_PRIO      4                   /* 任务优先级 */
#define TASK3_STK_SIZE  128                 /* 任务堆栈大小 */
StackType_t Task3TaskStack[TASK3_STK_SIZE]; /* 任务堆栈 */
StaticTask_t            Task3TaskTCB;       /* 任务控制块 */
TaskHandle_t            Task3Task_Handler;  /* 任务句柄 */
void task3(void *pvParameters);             /* 任务函数 */

/******************************************************************************************************/

/* LCD刷屏时使用的颜色 */
uint16_t lcd_discolor[11] = {WHITE, BLACK, BLUE, RED,
                             MAGENTA, GREEN, CYAN, YELLOW,
                             BROWN, BRRED, GRAY};

/**
 * @brief       FreeRTOS例程入口函数
 * @param       无
 * @retval      无
 */
void freertos_demo(void)
{
    lcd_show_string(10, 10, 220, 32, 32, "STM32", RED);
    lcd_show_string(10, 47, 220, 24, 24, "Task Create & Del", RED);
    lcd_show_string(10, 76, 220, 16, 16, "ATOM@ALIENTEK", RED);
    
    lcd_draw_rectangle(5, 110, 115, 314, BLACK);
    lcd_draw_rectangle(125, 110, 234, 314, BLACK);
    lcd_draw_line(5, 130, 115, 130, BLACK);
    lcd_draw_line(125, 130, 234, 130, BLACK);
    lcd_show_string(15, 111, 110, 16, 16, "Task1: 000", BLUE);
    lcd_show_string(135, 111, 110, 16, 16, "Task2: 000", BLUE);
    
    StartTask_Handler = xTaskCreateStatic((TaskFunction_t   )start_task,        /* 任务函数 */
                                          (const char*      )"start_task",      /* 任务名称 */
                                          (uint32_t         )START_STK_SIZE,    /* 任务堆栈大小 */
                                          (void*            )NULL,              /* 传递给任务函数的参数 */
                                          (UBaseType_t      )START_TASK_PRIO,   /* 任务优先级 */
                                          (StackType_t*     )StartTaskStack,    /* 任务堆栈 */
                                          (StaticTask_t*    )&StartTaskTCB);    /* 任务控制块 */
    vTaskStartScheduler();
}

/**
 * @brief       start_task
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           /* 进入临界区 */
    /* 创建任务1 */
    Task1Task_Handler = xTaskCreateStatic((TaskFunction_t   )task1,         /* 任务函数 */
                                          (const char*      )"task1",       /* 任务名称 */
                                          (uint32_t         )TASK1_STK_SIZE,/* 任务堆栈大小 */
                                          (void*            )NULL,          /* 传递给任务函数的参数 */
                                          (UBaseType_t      )TASK1_PRIO,    /* 任务优先级 */
                                          (StackType_t*     )Task1TaskStack,/* 任务堆栈 */
                                          (StaticTask_t*    )&Task1TaskTCB);/* 任务控制块 */
    /* 创建任务2 */
    Task2Task_Handler = xTaskCreateStatic((TaskFunction_t   )task2,         /* 任务函数 */
                                          (const char*      )"task2",       /* 任务名称 */
                                          (uint32_t         )TASK2_STK_SIZE,/* 任务堆栈大小 */
                                          (void*            )NULL,          /* 传递给任务函数的参数 */
                                          (UBaseType_t      )TASK2_PRIO,    /* 任务优先级 */
                                          (StackType_t*     )Task2TaskStack,/* 任务堆栈 */
                                          (StaticTask_t*    )&Task2TaskTCB);/* 任务控制块 */
    /* 创建任务3 */
    Task3Task_Handler = xTaskCreateStatic((TaskFunction_t   )task3,         /* 任务函数 */
                                          (const char*      )"task3",       /* 任务名称 */
                                          (uint32_t         )TASK3_STK_SIZE,/* 任务堆栈大小 */
                                          (void*            )NULL,          /* 传递给任务函数的参数 */
                                          (UBaseType_t      )TASK3_PRIO,    /* 任务优先级 */
                                          (StackType_t*     )Task3TaskStack,/* 任务堆栈 */
                                          (StaticTask_t*    )&Task3TaskTCB);/* 任务控制块 */
    vTaskDelete(StartTask_Handler); /* 删除开始任务 */
    taskEXIT_CRITICAL();            /* 退出临界区 */
}

/**
 * @brief       task1
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void task1(void *pvParameters)
{
    uint32_t task1_num = 0;
    
    while (1)
    {
        lcd_fill(6, 131, 114, 313, lcd_discolor[++task1_num % 11]);
        lcd_show_xnum(71, 111, task1_num, 3, 16, 0x80, BLUE);
        
        vTaskDelay(500);
    }
}

/**
 * @brief       task2
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void task2(void *pvParameters)
{
    uint32_t task2_num = 0;
    
    while (1)
    {
        lcd_fill(126, 131, 233, 313, lcd_discolor[11 - (++task2_num % 11)]);
        lcd_show_xnum(191, 111, task2_num, 3, 16, 0x80, BLUE);
        
        vTaskDelay(500);
    }
}

/**
 * @brief       task3
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void task3(void *pvParameters)
{
    uint8_t key = 0;
    
    while (1)
    {
        key = key_scan(0);
        
        switch (key)
        {
            case KEY0_PRES:                     /* 删除任务1 */
            {
                if (Task1Task_Handler != NULL)
                {
                    vTaskDelete(Task1Task_Handler);
                    Task1Task_Handler = NULL;
                }
                break;
            }
            case KEY1_PRES:                     /* 删除任务2 */
            {
                if (Task2Task_Handler != NULL)
                {
                    vTaskDelete(Task2Task_Handler);
                    Task2Task_Handler = NULL;
                }
                break;
            }
            default:
            {
                break;
            }
        }
        
        vTaskDelay(10);
    }
}
