/**
 ****************************************************************************************************
 * @file        freertos.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.4
 * @date        2022-01-04
 * @brief       FreeRTOS 队列集模拟事件标志位实验
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
#include "queue.h"

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

/* TASK2 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK2_PRIO      3                   /* 任务优先级 */
#define TASK2_STK_SIZE  128                 /* 任务堆栈大小 */
TaskHandle_t            Task2Task_Handler;  /* 任务句柄 */
void task2(void *pvParameters);             /* 任务函数 */

static QueueSetHandle_t xQueueSet;          /* 定义一个队列集 */
QueueHandle_t           xQueue1;            /* 定义队列1 */
QueueHandle_t           xQueue2;            /* 定义队列2 */

#define QUEUE_LENGTH    1                   /* 队列支持的消息个数 */
#define QUEUE_ITEM_SIZE sizeof(uint32_t)    /* 队列中每条消息的大小 */
#define QUEUESET_LENGTH (2 * QUEUE_LENGTH)  /* 队列集支持的消息个数 */

#define EVENTBIT_0      (1 << 0)            /* 事件位 */
#define EVENTBIT_1      (1 << 1)
#define EVENTBIT_ALL    (EVENTBIT_0 | EVENTBIT_1)

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
    lcd_show_string(10, 47, 220, 24, 24, "Queue Event Group", RED);
    lcd_show_string(10, 76, 220, 16, 16, "ATOM@ALIENTEK", RED);
    
    lcd_draw_rectangle(5, 130, 234, 314, BLACK);        /* 画矩形 */
    lcd_show_string(30, 110, 220, 16, 16, "Event Group Value: 0", BLUE);
    
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
    /* 创建队列集 */
    xQueueSet = xQueueCreateSet(QUEUESET_LENGTH);
    /* 创建队列 */
    xQueue1 = xQueueCreate(QUEUESET_LENGTH, QUEUE_ITEM_SIZE);
    xQueue2 = xQueueCreate(QUEUESET_LENGTH, QUEUE_ITEM_SIZE);
    /* 将队列添加到队列集中 */
    xQueueAddToSet(xQueue1, xQueueSet);
    xQueueAddToSet(xQueue2, xQueueSet);
    /* 创建任务1 */
    xTaskCreate((TaskFunction_t )task1,
                (const char*    )"task1",
                (uint16_t       )TASK1_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK1_PRIO,
                (TaskHandle_t*  )&Task1Task_Handler);
    /* 创建任务2 */
    xTaskCreate((TaskFunction_t )task2,
                (const char*    )"task2",
                (uint16_t       )TASK2_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK2_PRIO,
                (TaskHandle_t*  )&Task2Task_Handler);
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
    uint8_t key         = 0;
    uint8_t eventbit_0  = EVENTBIT_0;
    uint8_t eventbit_1  = EVENTBIT_1;
    
    while (1)
    {
        key = key_scan(0);
        
        switch (key)
        {
            case KEY0_PRES:                                     /* 队列1发送消息 */
            {
                xQueueSend(xQueue1, &eventbit_0, portMAX_DELAY);
                break;
            }
            case KEY1_PRES:                                     /* 队列2发送消息 */
            {
                xQueueSend(xQueue2, &eventbit_1, portMAX_DELAY);
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

/**
 * @brief       task2
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void task2(void *pvParameters)
{
    uint32_t                event_val       = 0;
    uint32_t                event_recv      = 0;
    QueueSetMemberHandle_t  activate_member = NULL;
    uint32_t                task2_num       = 0;
    
    while (1)
    {
        activate_member = xQueueSelectFromSet(xQueueSet, portMAX_DELAY);/* 等待队列集中的队列接收到消息 */
        
        xQueueReceive(activate_member, &event_recv, portMAX_DELAY);     /* 接收队列中的消息 */
        event_val |= event_recv;                                        /* 接收到的消息存入事件中 */
        lcd_show_xnum(182, 110, event_val, 1, 16, 0, BLUE);             /* 将事件值显示在LCD上 */
        
        if (event_val == EVENTBIT_ALL)                                  /* 所有事件都发生 */
        {
            event_val = 0;
            lcd_fill(6, 131, 233, 313, lcd_discolor[++task2_num % 11]); /* LCD区域刷新 */
        }
    }
}
