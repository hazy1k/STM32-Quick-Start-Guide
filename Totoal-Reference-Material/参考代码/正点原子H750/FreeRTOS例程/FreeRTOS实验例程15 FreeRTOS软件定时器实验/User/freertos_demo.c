/**
 ****************************************************************************************************
 * @file        freertos.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.4
 * @date        2022-01-04
 * @brief       FreeRTOS 软件定时器实验
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
#include "timers.h"

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

TimerHandle_t           Timer1Timer_Handler;/* 定时器1句柄 */
TimerHandle_t           Timer2Timer_Handler;/* 定时器2句柄 */

void Timer1Callback(TimerHandle_t xTimer);  /* 定时器1超时回调函数 */
void Timer2Callback(TimerHandle_t xTimer);  /* 定时器2超时回调函数 */

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
    lcd_show_string(10, 47, 220, 24, 24, "Timer", RED);
    lcd_show_string(10, 76, 220, 16, 16, "ATOM@ALIENTEK", RED);
    
    lcd_draw_rectangle(5, 110, 115, 314, BLACK);
    lcd_draw_rectangle(125, 110, 234, 314, BLACK);
    lcd_draw_line(5, 130, 115, 130, BLACK);
    lcd_draw_line(125, 130, 234, 130, BLACK);
    lcd_show_string(15, 111, 110, 16, 16, "Timer1: 000", BLUE);
    lcd_show_string(135, 111, 110, 16, 16, "Timer2: 000", BLUE);
    
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
    /* 定时器1创建为周期定时器 */
    Timer1Timer_Handler = xTimerCreate((const char*  )"Timer1",                 /* 定时器名 */
                                      (TickType_t   )1000,                      /* 定时器超时时间 */
                                      (UBaseType_t  )pdTRUE,                    /* 周期定时器 */
                                      (void*        )1,                         /* 定时器ID */
                                      (TimerCallbackFunction_t)Timer1Callback); /* 定时器回调函数 */
    /* 定时器2创建为单次定时器 */
    Timer2Timer_Handler = xTimerCreate((const char*  )"Timer2",                 /* 定时器名 */
                                     (TickType_t    )1000,                      /* 定时器超时时间 */
                                     (UBaseType_t   )pdFALSE,                   /* 单次定时器 */
                                     (void*         )2,                         /* 定时器ID */
                                     (TimerCallbackFunction_t)Timer2Callback);  /* 定时器回调函数 */
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
    uint8_t key = 0;
    
    while (1)
    {
        if ((Timer1Timer_Handler != NULL) && (Timer2Timer_Handler != NULL))
        {
            key = key_scan(0);
            
            switch (key)
            {
                case KEY0_PRES:
                {
                    xTimerStart((TimerHandle_t  )Timer1Timer_Handler,   /* 待启动的定时器句柄 */
                                (TickType_t     )portMAX_DELAY);        /* 等待系统启动定时器的最大时间 */
                    xTimerStart((TimerHandle_t  )Timer2Timer_Handler,   /* 待启动的定时器句柄 */
                                (TickType_t     )portMAX_DELAY);        /* 等待系统启动定时器的最大时间 */
                    break;
                }
                case KEY1_PRES:
                {
                    xTimerStop((TimerHandle_t  )Timer1Timer_Handler,    /* 待停止的定时器句柄 */
                                (TickType_t     )portMAX_DELAY);        /* 等待系统停止定时器的最大时间 */
                    xTimerStop((TimerHandle_t  )Timer2Timer_Handler,    /* 待停止的定时器句柄 */
                                (TickType_t     )portMAX_DELAY);        /* 等待系统停止定时器的最大时间 */
                    break;
                }
                default:
                {
                    break;
                }
            }
        }
        
        vTaskDelay(10);
    }
}

/**
 * @brief       Timer1超时回调函数
 * @param       xTimer : 传入参数(未用到)
 * @retval      无
 */
void Timer1Callback(TimerHandle_t xTimer)
{
    static uint32_t timer1_num = 0;
    
    lcd_fill(6, 131, 114, 313, lcd_discolor[++timer1_num % 11]);    /* LCD区域刷新 */
    lcd_show_xnum(79, 111, timer1_num, 3, 16, 0x80, BLUE);          /* 显示定时器1超时次数 */
}

/**
 * @brief       Timer2超时回调函数
 * @param       xTimer : 传入参数(未用到)
 * @retval      无
 */
void Timer2Callback(TimerHandle_t xTimer)
{
    static uint32_t timer2_num = 0;
    
    lcd_fill(126, 131, 233, 313, lcd_discolor[++timer2_num % 11]);  /* LCD区域刷新 */
    lcd_show_xnum(199, 111, timer2_num, 3, 16, 0x80, BLUE);         /* 显示定时器2超时次数 */
}
