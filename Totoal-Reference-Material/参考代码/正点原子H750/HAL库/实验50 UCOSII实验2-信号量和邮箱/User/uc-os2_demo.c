/**
 ****************************************************************************************************
 * @file        uc-os2_demo.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-02-14
 * @brief       UCOSII实验2-信号量和邮箱 实验
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台：正点原子 Mini Pro H750开发板
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
#include "./BSP/LCD/lcd.h"
#include "./BSP/KEY/key.h"
#include "./BSP/BEEP/beep.h"
#include "./BSP/TOUCH/touch.h"
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


/* 触摸屏任务 任务 配置
 * 包括: 任务优先级 堆栈大小 等
 */
#define TOUCH_TASK_PRIO                 7       /* 优先级设置(越小优先级越高) */
#define TOUCH_STK_SIZE                  128     /* 堆栈大小 */

OS_STK TOUCH_TASK_STK[TOUCH_STK_SIZE];          /* 任务堆栈 */
void touch_task(void *p_arg);                   /* 任务函数 */


/* LED 任务 配置
 * 包括: 任务优先级 堆栈大小 等
 */
#define LED_TASK_PRIO                   6       /* 优先级设置(越小优先级越高) */
#define LED_STK_SIZE                    128     /* 堆栈大小 */

OS_STK LED_TASK_STK[LED_STK_SIZE];              /* 任务堆栈 */
void led_task(void *p_arg);                     /* 任务函数 */


/* 蜂鸣器 任务 配置
 * 包括: 任务优先级 堆栈大小 等
 */
#define BEEP_TASK_PRIO                  5       /* 优先级设置(越小优先级越高) */
#define BEEP_STK_SIZE                   128     /* 堆栈大小 */

OS_STK BEEP_TASK_STK[BEEP_STK_SIZE];            /* 任务堆栈 */
void beep_task(void *p_arg);                    /* 任务函数 */


/* 主 任务 配置
 * 包括: 任务优先级 堆栈大小 等
 */
#define MAIN_TASK_PRIO                  4       /* 优先级设置(越小优先级越高) */
#define MAIN_STK_SIZE                   512     /* 堆栈大小 */

OS_STK MAIN_TASK_STK[MAIN_STK_SIZE];            /* 任务堆栈 */
void main_task(void *p_arg);                    /* 任务函数 */


/* 按键扫描 任务 配置
 * 包括: 任务优先级 堆栈大小 等
 */
#define KEY_TASK_PRIO                   3       /* 优先级设置(越小优先级越高) */
#define KEY_STK_SIZE                    128     /* 堆栈大小 */

OS_STK KEY_TASK_STK[KEY_STK_SIZE];              /* 任务堆栈 */
void key_task(void *p_arg);                     /* 任务函数 */


/******************************************************************************************************/
OS_EVENT *msg_key;      /* 按键邮箱事件块指针 */
OS_EVENT *sem_beep;     /* 蜂鸣器信号量指针 */

void ucos_load_main_ui(void);
void lcd_draw_bline(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t size, uint16_t color);

/**
 * @brief       uC/OS-II例程入口函数
 * @param       无
 * @retval      无
 */
void uc_os2_demo(void)
{
    ucos_load_main_ui();                    /* 加载主界面 */
    
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

    msg_key = OSMboxCreate((void *)0);  /* 创建消息邮箱 */
    sem_beep = OSSemCreate(0);          /* 创建信号量 */
    
    OSStatInit();                       /* 开启统计任务 */
    /* 根据配置的节拍频率配置SysTick */
    cnts = (CPU_INT32U)(HAL_RCC_GetSysClockFreq() / OS_TICKS_PER_SEC);
    OS_CPU_SysTickInit(cnts);
    
    OS_ENTER_CRITICAL();                /* 进入临界区(关闭中断) */
    
    /* 触摸任务 */
    OSTaskCreateExt((void(*)(void *) )touch_task,
                    (void *          )0,
                    (OS_STK *        )&TOUCH_TASK_STK[TOUCH_STK_SIZE - 1],
                    (INT8U          )TOUCH_TASK_PRIO,
                    (INT16U         )TOUCH_TASK_PRIO,
                    (OS_STK *        )&TOUCH_TASK_STK[0],
                    (INT32U         )TOUCH_STK_SIZE,
                    (void *          )0,
                    (INT16U         )OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR | OS_TASK_OPT_SAVE_FP);
    /* LED任务 */
    OSTaskCreateExt((void(*)(void *) )led_task,
                    (void *          )0,
                    (OS_STK *        )&LED_TASK_STK[LED_STK_SIZE - 1],
                    (INT8U          )LED_TASK_PRIO,
                    (INT16U         )LED_TASK_PRIO,
                    (OS_STK *        )&LED_TASK_STK[0],
                    (INT32U         )LED_STK_SIZE,
                    (void *          )0,
                    (INT16U         )OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR | OS_TASK_OPT_SAVE_FP);
    /* 蜂鸣器任务 */
    OSTaskCreateExt((void(*)(void *) )beep_task,
                    (void *          )0,
                    (OS_STK *        )&BEEP_TASK_STK[BEEP_STK_SIZE - 1],
                    (INT8U          )BEEP_TASK_PRIO,
                    (INT16U         )BEEP_TASK_PRIO,
                    (OS_STK *        )&BEEP_TASK_STK[0],
                    (INT32U         )BEEP_STK_SIZE,
                    (void *          )0,
                    (INT16U         )OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR | OS_TASK_OPT_SAVE_FP);
    /* 主任务 */
    OSTaskCreateExt((void(*)(void *) )main_task,
                    (void *          )0,
                    (OS_STK *        )&MAIN_TASK_STK[MAIN_STK_SIZE - 1],
                    (INT8U          )MAIN_TASK_PRIO,
                    (INT16U         )MAIN_TASK_PRIO,
                    (OS_STK *        )&MAIN_TASK_STK[0],
                    (INT32U         )MAIN_STK_SIZE,
                    (void *          )0,
                    (INT16U         )OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR | OS_TASK_OPT_SAVE_FP);
    /* 按键任务 */
    OSTaskCreateExt((void(*)(void *) )key_task,
                    (void *          )0,
                    (OS_STK *        )&KEY_TASK_STK[KEY_STK_SIZE - 1],
                    (INT8U          )KEY_TASK_PRIO,
                    (INT16U         )KEY_TASK_PRIO,
                    (OS_STK *        )&KEY_TASK_STK[0],
                    (INT32U         )KEY_STK_SIZE,
                    (void *          )0,
                    (INT16U         )OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR | OS_TASK_OPT_SAVE_FP);
    OS_EXIT_CRITICAL();             /* 退出临界区(开中断) */
    OSTaskSuspend(START_TASK_PRIO); /* 挂起开始任务 */
}
/**
 * @brief       LED任务
 * @param       p_arg : 传入参数(未用到)
 * @retval      无
 */
void led_task(void *p_arg)
{
    uint8_t t;

    while (1)
    {
        t++;
        OSTimeDly(10);

        if (t == 8)LED0(1); /* LED0灭 */

        if (t == 100)       /* LED0亮 */
        {
            t = 0;
            LED0(0);
        }
    }
}

/**
 * @brief       蜂鸣器任务
 * @param       p_arg : 传入参数(未用到)
 * @retval      无
 */
void beep_task(void *p_arg)
{
    uint8_t err;

    while (1)
    {
        OSSemPend(sem_beep, 0, &err);   /* 请求信号量 */
        BEEP(1);        /* 打开蜂鸣器 */
        OSTimeDly(60);
        BEEP(0);        /* 关闭蜂鸣器 */
        OSTimeDly(940);
    }
}

/**
 * @brief       触摸屏任务
 * @param       p_arg : 传入参数(未用到)
 * @retval      无
 */
void touch_task(void *p_arg)
{
    uint32_t cpu_sr;
    uint16_t lastpos[2];    /* 最后一次的数据 */

    while (1)
    {
        tp_dev.scan(0);

        if (tp_dev.sta & TP_PRES_DOWN)  /* 触摸屏被按下 */
        {
            if (tp_dev.x[0] < lcddev.width && tp_dev.y[0] < lcddev.height && tp_dev.y[0] > 120)
            {
                if (lastpos[0] == 0XFFFF)
                {
                    lastpos[0] = tp_dev.x[0];
                    lastpos[1] = tp_dev.y[0];
                }

                OS_ENTER_CRITICAL();    /* 进入临界段,防止其他任务,打断LCD操作,导致液晶乱序 */
                lcd_draw_bline(lastpos[0], lastpos[1], tp_dev.x[0], tp_dev.y[0], 2, RED);   /* 画线 */
                OS_EXIT_CRITICAL();
                lastpos[0] = tp_dev.x[0];
                lastpos[1] = tp_dev.y[0];
            }
        }
        else
        {
            lastpos[0] = 0XFFFF;
            OSTimeDly(10);   /* 没有按键按下的时候 */
        }
    }
}

/**
 * @brief       主任务
 * @param       p_arg : 传入参数(未用到)
 * @retval      无
 */
void main_task(void *p_arg)
{
    uint32_t key = 0;
    uint8_t err;
    uint8_t semmask = 0;
    uint8_t tcnt = 0;

    while (1)
    {
        key = (uint32_t)OSMboxPend(msg_key, 10, &err);

        switch (key)
        {
            case KEY0_PRES: /* 控制DS1,并清除触摸区域 */
                LED1_TOGGLE();
                lcd_fill(0, 121, lcddev.width - 1, lcddev.height - 1, WHITE);
                break;
 
            case KEY1_PRES: /* 发送信号量 */
                semmask = 1;
                OSSemPost(sem_beep);
                break;

            case WKUP_PRES: /* 校准 */
                OSTaskSuspend(TOUCH_TASK_PRIO); /* 挂起触摸屏任务 */

                if ((tp_dev.touchtype & 0X80) == 0)
                {
                    tp_adjust();
                }
                
                OSTaskResume(TOUCH_TASK_PRIO);  /* 解挂 */
                ucos_load_main_ui();            /* 重新加载主界面 */
                break;
        }

        if (semmask || sem_beep->OSEventCnt)    /* 需要显示sem */
        {
            lcd_show_xnum(192, 50, sem_beep->OSEventCnt, 3, 16, 0X80, BLUE);    /* 显示信号量的值 */

            if (sem_beep->OSEventCnt == 0)semmask = 0;  /* 停止更新 */
        }

        if (tcnt == 10) /* 0.6秒更新一次CPU使用率 */
        {
            tcnt = 0;
            lcd_show_xnum(192, 30, OSCPUUsage, 3, 16, 0, BLUE); /* 显示CPU使用率 */
        }

        tcnt++;
        OSTimeDly(10);
    }
}

/**
 * @brief       按键扫描任务
 * @param       p_arg : 传入参数(未用到)
 * @retval      无
 */
void key_task(void *p_arg)
{
    uint32_t key;

    while (1)
    {
        key = key_scan(0);

        if (key)OSMboxPost(msg_key, (void *)key); /* 发送消息 */

        OSTimeDly(10);
    }
}

/**
 * @brief       加载主界面
 * @param       无
 * @retval      无
 */
void ucos_load_main_ui(void)
{
    lcd_clear(WHITE);   /* 清屏 */
    lcd_show_string(30, 10, 200, 16, 16, "STM32", RED);
    lcd_show_string(30, 30, 200, 16, 16, "UCOSII TEST2", RED);
    lcd_show_string(30, 50, 200, 16, 16, "ATOM@ALIENTEK", RED);
    lcd_show_string(30, 75, 200, 16, 16, "KEY0:LED1 AND CLEAR", RED);
    lcd_show_string(30, 95, 200, 16, 16, "KEY1:BEEP  KEY_UP:ADJUST", RED);
    lcd_show_string(80, 210, 200, 16, 16, "Touch Area", RED);
    lcd_draw_line(0, 120, lcddev.width - 1, 120, RED);
    lcd_draw_line(0, 70, lcddev.width - 1, 70, RED);
    lcd_draw_line(150, 0, 150, 70, RED);
    
    lcd_show_string(160, 30, 200, 16, 16, "CPU:   %", BLUE);
    lcd_show_string(160, 50, 200, 16, 16, "SEM:000", BLUE);
}

/**
 * @brief       画粗线
 * @param       x1,y1: 起点坐标
 * @param       x2,y2: 终点坐标
 * @param       size : 线条粗细程度
 * @param       color: 线的颜色
 * @retval      无
 */
void lcd_draw_bline(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t size, uint16_t color)
{
    uint16_t t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, row, col;

    if (x1 < size || x2 < size || y1 < size || y2 < size)return;

    delta_x = x2 - x1;  /* 计算坐标增量 */
    delta_y = y2 - y1;
    row = x1;
    col = y1;

    if (delta_x > 0)incx = 1;       /* 设置单步方向 */
    else if (delta_x == 0)incx = 0; /* 垂直线 */
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }

    if (delta_y > 0)
    {
        incy = 1;
    }
    else if (delta_y == 0)
    {
        incy = 0;   /* 水平线 */
    }
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }

    if ( delta_x > delta_y)distance = delta_x; /* 选取基本增量坐标轴 */
    else distance = delta_y;

    for (t = 0; t <= distance + 1; t++ )    /* 画线输出 */
    {
        lcd_fill_circle(row, col, size, color); /* 画点 */
        xerr += delta_x ;
        yerr += delta_y ;

        if (xerr > distance)
        {
            xerr -= distance;
            row += incx;
        }

        if (yerr > distance)
        {
            yerr -= distance;
            col += incy;
        }
    }
}

