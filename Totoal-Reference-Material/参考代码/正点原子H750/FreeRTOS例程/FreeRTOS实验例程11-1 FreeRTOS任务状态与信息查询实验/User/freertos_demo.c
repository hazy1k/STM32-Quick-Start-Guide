/**
 ****************************************************************************************************
 * @file        freertos.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.4
 * @date        2022-01-04
 * @brief       FreeRTOS 任务状态与信息查询实验
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
#include "./SYSTEM/usart/usart.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/KEY/key.h"
#include "./MALLOC/malloc.h"
#include "string.h"
/*FreeRTOS*********************************************************************************************/
#include "FreeRTOS.h"
#include "task.h"

/******************************************************************************************************/
/*FreeRTOS配置*/

/* START_TASK 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define START_TASK_PRIO 1                   /* 任务优先级 */
#define START_STK_SIZE  128                 /* 任务堆栈大小 */
TaskHandle_t            StartTask_Handler;  /* 任务句柄 */
void start_task(void *pvParameters);        /* 任务函数 */

/* TASK1 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK1_PRIO      2                   /* 任务优先级 */
#define TASK1_STK_SIZE  128                 /* 任务堆栈大小 */
TaskHandle_t            Task1Task_Handler;  /* 任务句柄 */
void task1(void *pvParameters);             /* 任务函数 */

/******************************************************************************************************/

/**
 * @brief       FreeRTOS例程入口函数
 * @param       无
 * @retval      无
 */
void freertos_demo(void)
{
    lcd_show_string(10, 10, 220, 32, 32, "STM32", RED);
    lcd_show_string(10, 47, 220, 24, 24, "Task Info Query", RED);
    lcd_show_string(10, 76, 220, 16, 16, "ATOM@ALIENTEK", RED);
    
    xTaskCreate((TaskFunction_t )start_task,            /* 任务函数 */
                (const char*    )"start_task",          /* 任务名称 */
                (uint16_t       )START_STK_SIZE,        /* 任务堆栈大小 */
                (void*          )NULL,                  /* 传入给任务函数的参数 */
                (UBaseType_t    )START_TASK_PRIO,       /* 任务优先级 */
                (TaskHandle_t*  )&StartTask_Handler);   /* 任务句柄 */
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
    xTaskCreate((TaskFunction_t )task1,
                (const char*    )"task1",
                (uint16_t       )TASK1_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK1_PRIO,
                (TaskHandle_t*  )&Task1Task_Handler);
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
    uint32_t        i               = 0;
    UBaseType_t     task_num        = 0;
    TaskStatus_t    *status_array   = NULL;
    TaskHandle_t    task_handle     = NULL;
    TaskStatus_t    *task_info      = NULL;
    eTaskState      task_state      = eInvalid;
    char            *task_state_str = NULL;
    char            *task_info_buf  = NULL;
    
    /* 第一步：函数uxTaskGetSystemState()的使用 */
    printf("/********第一步：函数uxTaskGetSystemState()的使用**********/\r\n");
    task_num = uxTaskGetNumberOfTasks();                            /* 获取系统任务数量 */
    status_array = mymalloc(SRAMIN, task_num * sizeof(TaskStatus_t));
    task_num = uxTaskGetSystemState((TaskStatus_t* )status_array,   /* 任务状态信息buffer */
                                    (UBaseType_t   )task_num,       /* buffer大小 */
                                    (uint32_t*     )NULL);          /* 不获取任务运行时间信息 */
    printf("任务名\t\t优先级\t\t任务编号\r\n");
    for (i=0; i<task_num; i++)
    {
        printf("%s\t%s%ld\t\t%ld\r\n",
                status_array[i].pcTaskName,
                strlen(status_array[i].pcTaskName) > 7 ? "": "\t",
                status_array[i].uxCurrentPriority,
                status_array[i].xTaskNumber);
    }
    myfree(SRAMIN, status_array);
    printf("/**************************结束***************************/\r\n");
    printf("按下KEY0键继续!\r\n\r\n\r\n");
    while (key_scan(0) != KEY0_PRES)
    {
        vTaskDelay(10);
    }
    
    /* 第二步：函数vTaskGetInfo()的使用 */
    printf("/************第二步：函数vTaskGetInfo()的使用**************/\r\n");
    task_info = mymalloc(SRAMIN, sizeof(TaskStatus_t));
    task_handle = xTaskGetHandle("task1");                          /* 获取任务句柄 */
    vTaskGetInfo((TaskHandle_t  )task_handle,                       /* 任务句柄 */
                 (TaskStatus_t* )task_info,                         /* 任务信息buffer */
                 (BaseType_t    )pdTRUE,                            /* 允许统计任务堆栈历史最小值 */
                 (eTaskState    )eInvalid);                         /* 获取任务运行状态 */
    printf("任务名:\t\t\t%s\r\n", task_info->pcTaskName);
    printf("任务编号:\t\t%ld\r\n", task_info->xTaskNumber);
    printf("任务壮态:\t\t%d\r\n", task_info->eCurrentState);
    printf("任务当前优先级:\t\t%ld\r\n", task_info->uxCurrentPriority);
    printf("任务基优先级:\t\t%ld\r\n", task_info->uxBasePriority);
    printf("任务堆栈基地址:\t\t0x%p\r\n", task_info->pxStackBase);
    printf("任务堆栈历史剩余最小值:\t%d\r\n", task_info->usStackHighWaterMark);
    myfree(SRAMIN, task_info);
    printf("/**************************结束***************************/\r\n");
    printf("按下KEY0键继续!\r\n\r\n\r\n");
    while (key_scan(0) != KEY0_PRES)
    {
        vTaskDelay(10);
    }
    /* 第三步：函数eTaskGetState()的使用  */
    printf("/***********第三步：函数eTaskGetState()的使用*************/\r\n");
    task_state_str = mymalloc(SRAMIN, 10);
    task_handle = xTaskGetHandle("task1");
    task_state = eTaskGetState(task_handle);                        /* 获取任务运行状态 */
    sprintf(task_state_str, task_state == eRunning ? "Runing" :
                            task_state == eReady ? "Ready" :
                            task_state == eBlocked ? "Blocked" :
                            task_state == eSuspended ? "Suspended" :
                            task_state == eDeleted ? "Deleted" :
                            task_state == eInvalid ? "Invalid" :
                                                     "");
    printf("任务状态值: %d，对应状态为: %s\r\n", task_state, task_state_str);
    myfree(SRAMIN, task_state_str);
    printf("/**************************结束***************************/\r\n");
    printf("按下KEY0键继续!\r\n\r\n\r\n");
    while (key_scan(0) != KEY0_PRES)
    {
        vTaskDelay(10);
    }
    
    /* 第四步：函数vTaskList()的使用 */
    printf("/*************第四步：函数vTaskList()的使用*************/\r\n");
    task_info_buf = mymalloc(SRAMIN, 500);
    vTaskList(task_info_buf);                                       /* 获取所有任务的信息 */
    printf("任务名\t\t状态\t优先级\t剩余栈\t任务序号\r\n");
    printf("%s\r\n", task_info_buf);
    myfree(SRAMIN, task_info_buf);
    printf("/************************实验结束***************************/\r\n");
    
    while (1)
    {
        vTaskDelay(10);
    }
}
