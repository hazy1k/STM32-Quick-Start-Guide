/**
 ****************************************************************************************************
 * @file        uc-os3_demo.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-03-12
 * @brief       emWin的GIF图片显示实验
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
#include "./BSP/LED/led.h"
#include "./BSP/KEY/key.h"
#include "./BSP/LCD/lcd.h"
#include "./TEXT/fonts.h"
#include "os.h"
#include "cpu.h"
#include "GUI.h"
#include "WM.h"
#include "DIALOG.h"
#include "EmWinHZFont.h"
#include "emwin_demo.h"


/******************************************************************************************/
/* UCOSIII任务设置 */

/* START_TASK 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define START_TASK_PRIO     10          /* 任务优先级 */
#define START_STK_SIZE      128         /* 任务堆栈大小 */
OS_TCB StartTaskTCB;                    /* 任务控制块 */
CPU_STK START_TASK_STK[START_STK_SIZE]; /* 任务堆栈 */
void start_task(void *p_arg);           /* 任务函数 */

/* USER_TASK 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define USER_TASK_PRIO     8            /* 任务优先级 */
#define USER_STK_SIZE      2048         /* 任务堆栈大小 */
OS_TCB User_TaskTCB;                    /* 任务控制块 */
CPU_STK USER_TASK_STK[USER_STK_SIZE];   /* 任务堆栈 */
void user_task(void *p_arg);
/* TOUCH_TASK 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TOUCH_TASK_PRIO     7           /* 任务优先级 */
#define TOUCH_STK_SIZE      512         /* 任务堆栈大小 */
OS_TCB Touch_TaskTCB;                   /* 任务控制块 */
CPU_STK TOUCH_TASK_STK[TOUCH_STK_SIZE]; /* 任务堆栈 */
void touch_task(void *p_arg);

/* LED_TASK 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define LED_TASK_PRIO     6            /* 任务优先级 */
#define LED_STK_SIZE      128          /* 任务堆栈大小 */
OS_TCB Led_TaskTCB;                    /* 任务控制块 */
CPU_STK LED_TASK_STK[LED_STK_SIZE];    /* 任务堆栈 */
void led_task(void *p_arg);

/* KEY_TASK 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define KEY_TASK_PRIO     5            /* 任务优先级 */
#define KEY_STK_SIZE      128          /* 任务堆栈大小 */
OS_TCB Key_TaskTCB;                    /* 任务控制块 */
CPU_STK KEY_TASK_STK[KEY_STK_SIZE];    /* 任务堆栈 */
void key_task(void *p_arg);

/* FONT_TASK 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define FONT_TASK_PRIO     3            /* 任务优先级 */
#define FONT_STK_SIZE      512          /* 任务堆栈大小 */
OS_TCB Font_TaskTCB;                    /* 任务控制块 */
CPU_STK FONT_TASK_STK[FONT_STK_SIZE];   /* 任务堆栈 */
void font_task(void *p_arg);

/* IMAGE_TASK 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define IMAGE_TASK_PRIO     4            /* 任务优先级 */
#define IMAGE_STK_SIZE      512          /* 任务堆栈大小 */
OS_TCB Image_TaskTCB;                    /* 任务控制块 */
CPU_STK IMAGE_TASK_STK[IMAGE_STK_SIZE];  /* 任务堆栈 */
void image_task(void *p_arg);

OS_SEM    Font_SEM;    /* 定义一个信号量，用于更新字体 */
OS_SEM    Imsge_SEM;   /* 定义一个信号量，用于截图 */

/******************************************************************************************/


/**
 * @brief       创建emWin任务
 * @param       无
 * @retval      无
 */
void uc_os3_demo(void)
{
    OS_ERR err;
    CPU_SR_ALLOC();
    OSInit(&err);          /* 初始化UCOSIII */
    CPU_CRITICAL_ENTER();  /* 进入临界区 */
    /* 创建开始任务 */
    OSTaskCreate((OS_TCB *     )&StartTaskTCB,       /* 任务控制块 */
                 (CPU_CHAR *   )"start task",        /* 任务名字 */
                 (OS_TASK_PTR  )start_task,          /* 任务函数 */
                 (void *       )0,                   /* 传递给任务函数的参数 */
                 (OS_PRIO      )START_TASK_PRIO,     /* 任务优先级 */
                 (CPU_STK *    )&START_TASK_STK[0],  /* 任务堆栈基地址 */
                 (CPU_STK_SIZE )START_STK_SIZE / 10, /* 任务堆栈深度限位 */
                 (CPU_STK_SIZE )START_STK_SIZE,      /* 任务堆栈大小 */
                 (OS_MSG_QTY   )0,                   /* 任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息 */
                 (OS_TICK      )0,                   /* 当使能时间片轮转时的时间片长度，为0时为默认长度 */
                 (void *       )0,                   /* 用户补充的存储区 */
                 (OS_OPT       )OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR, /* 任务选项 */
                 (OS_ERR *     )&err);  /* 存放该函数错误时的返回值 */

    CPU_CRITICAL_EXIT();  /* 退出临界区 */
    OSStart(&err);       /* 开启UCOSIII */
}

/**
 * @brief       start_task
 * @param       p_arg : 传入参数(未用到)
 * @retval      无
 */
void start_task(void *p_arg)
{
    OS_ERR err;
    CPU_SR_ALLOC();
    p_arg = p_arg;
    CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&err);     /* 统计任务 */
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN        /* 如果使能了测量中断关闭时间 */
    CPU_IntDisMeasMaxCurReset();
#endif

#if OS_CFG_SCHED_ROUND_ROBIN_EN       /* 当使用时间片轮转的时候 */
    /* 使能时间片轮转调度功能,时间片长度为1个系统时钟节拍，既1*5=5ms */
    OSSchedRoundRobinCfg(DEF_ENABLED, 1, &err);
#endif
    
    __HAL_RCC_CRC_CLK_ENABLE();         /* 使能CRC时钟 */
    GUI_Init();                         /* STemWin 初始化 */ \
  
    CPU_CRITICAL_ENTER();    /* 进入临界区 */

    /* 创建一个更新字体信号量 */
    OSSemCreate ((OS_SEM*   )&Font_SEM,
                 (CPU_CHAR* )"Font_SEM",
                 (OS_SEM_CTR)0,
                 (OS_ERR*   )&err);

    /* 创建一个截图信号量 */
    OSSemCreate ((OS_SEM*   )&Imsge_SEM,
                 (CPU_CHAR* )"IMAGE_SEM",
                 (OS_SEM_CTR)0,
                 (OS_ERR*   )&err);
                 
    /* 创建User任务 */
    OSTaskCreate((OS_TCB *    )&User_TaskTCB,
                 (CPU_CHAR *  )"User task",
                 (OS_TASK_PTR )user_task,
                 (void *      )0,
                 (OS_PRIO     )USER_TASK_PRIO,
                 (CPU_STK *   )&USER_TASK_STK[0],
                 (CPU_STK_SIZE)USER_STK_SIZE / 10,
                 (CPU_STK_SIZE)USER_STK_SIZE,
                 (OS_MSG_QTY  )0,
                 (OS_TICK     )0,
                 (void *      )0,
                 (OS_OPT      )OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,
                 (OS_ERR *    )&err);
                 
    /* 创建TOUCH任务 */
    OSTaskCreate((OS_TCB *    )&Touch_TaskTCB,
                 (CPU_CHAR *  )"Touch task",
                 (OS_TASK_PTR )touch_task,
                 (void *      )0,
                 (OS_PRIO     )TOUCH_TASK_PRIO,
                 (CPU_STK *   )&TOUCH_TASK_STK[0],
                 (CPU_STK_SIZE)TOUCH_STK_SIZE / 10,
                 (CPU_STK_SIZE)TOUCH_STK_SIZE,
                 (OS_MSG_QTY  )0,
                 (OS_TICK     )0,
                 (void *      )0,
                 (OS_OPT      )OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,
                 (OS_ERR *    )&err);

    /* 创建LED任务 */
    OSTaskCreate((OS_TCB *    )&Led_TaskTCB,
                 (CPU_CHAR *  )"Led task",
                 (OS_TASK_PTR )led_task,
                 (void *      )0,
                 (OS_PRIO     )LED_TASK_PRIO,
                 (CPU_STK *   )&LED_TASK_STK[0],
                 (CPU_STK_SIZE)LED_STK_SIZE / 10,
                 (CPU_STK_SIZE)LED_STK_SIZE,
                 (OS_MSG_QTY  )0,
                 (OS_TICK     )0,
                 (void *      )0,
                 (OS_OPT      )OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,
                 (OS_ERR *    )&err);
                 
    /* 创建KEY任务 */
    OSTaskCreate((OS_TCB *    )&Key_TaskTCB,
                 (CPU_CHAR *  )"Keyd task",
                 (OS_TASK_PTR )key_task,
                 (void *      )0,
                 (OS_PRIO     )KEY_TASK_PRIO,
                 (CPU_STK *   )&KEY_TASK_STK[0],
                 (CPU_STK_SIZE)KEY_STK_SIZE / 10,
                 (CPU_STK_SIZE)KEY_STK_SIZE,
                 (OS_MSG_QTY  )0,
                 (OS_TICK     )0,
                 (void *      )0,
                 (OS_OPT      )OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,
                 (OS_ERR *    )&err);
                 
    /* 创建FONT任务 */
    OSTaskCreate((OS_TCB *    )&Font_TaskTCB,
                 (CPU_CHAR *  )"Font task",
                 (OS_TASK_PTR )font_task,
                 (void *      )0,
                 (OS_PRIO     )FONT_TASK_PRIO,
                 (CPU_STK *   )&FONT_TASK_STK[0],
                 (CPU_STK_SIZE)FONT_STK_SIZE / 10,
                 (CPU_STK_SIZE)FONT_STK_SIZE,
                 (OS_MSG_QTY  )0,
                 (OS_TICK     )0,
                 (void *      )0,
                 (OS_OPT      )OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,
                 (OS_ERR *    )&err);
                 
    /* 创建IMAGE任务 */
    OSTaskCreate((OS_TCB *    )&Image_TaskTCB,
                 (CPU_CHAR *  )"Image task",
                 (OS_TASK_PTR )image_task,
                 (void *      )0,
                 (OS_PRIO     )IMAGE_TASK_PRIO,
                 (CPU_STK *   )&IMAGE_TASK_STK[0],
                 (CPU_STK_SIZE)IMAGE_STK_SIZE / 10,
                 (CPU_STK_SIZE)IMAGE_STK_SIZE,
                 (OS_MSG_QTY  )0,
                 (OS_TICK     )0,
                 (void *      )0,
                 (OS_OPT      )OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,
                 (OS_ERR *    )&err);

    CPU_CRITICAL_EXIT();           /* 退出临界区 */
    OSTaskDel((OS_TCB *)0, &err); /* 删除start_task任务自身 */
}

/**
 * @brief       执行emwin例程
 * @param       p_arg : 传入参数(未用到)
 * @retval      无
 */
void user_task(void *p_arg)
{
    p_arg = p_arg;
    emwin_main();  /* 执行emwin例程 */

    while (1)
    {
        GUI_Delay(100);
    }
}

/**
 * @brief       emwin触摸
 * @param       p_arg : 传入参数(未用到)
 * @retval      无
 */
void touch_task(void *p_arg)
{
    p_arg = p_arg;
    GUI_CURSOR_Show();
    
    while(1)
    {
        GUI_TOUCH_Exec();
        GUI_Delay(5);
    }
}

 
/**
 * @brief       代表系统在运行中
 * @param       p_arg : 传入参数(未用到)
 * @retval      无
 */
void led_task(void *p_arg)
{
    OS_ERR err;
    p_arg = p_arg;

    while (1)
    {
        LED0_TOGGLE();
        OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_PERIODIC,&err);  /* 延时1s */
    }
}

/**
 * @brief       截图信号量/更新字体
 * @param       p_arg : 传入参数(未用到)
 * @retval      无
 */
void key_task(void *p_arg)
{
    OS_ERR err;
    p_arg = p_arg;
    uint8_t key;

    while (1)
    {
        key = key_scan(0);

        switch(key)
        {
            case KEY0_PRES:
              printf("发送截图信号量\r\n");
              OSSemPost (&Imsge_SEM, OS_OPT_POST_1, &err);   /* 发送截图信号量 */
              break;
            
            case WKUP_PRES:
              printf("更新字体信号量\r\n");
              OSSemPost (&Font_SEM, OS_OPT_POST_1, &err);   /* 发送更新字体信号量 */
              break;
        }

        OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_PERIODIC,&err);  /* 延时10ms */

    }
}

/**
 * @brief       更新字库
 * @param       p_arg : 传入参数(未用到)
 * @retval      无
 */
void font_task(void *p_arg)
{
    OS_ERR err;
    p_arg = p_arg;

    while (1)
    {
        OSSemPend(&Font_SEM, 0,OS_OPT_PEND_BLOCKING,0,&err); /* 请求截图信号量 */
        GUI_COLOR forecolor, bkcolor;
        forecolor = GUI_GetColor();   /* 获取当前前景色 */
        bkcolor = GUI_GetBkColor();   /* 获取当前背景色 */
        OSSchedLock(&err);
        lcd_show_string(10,50,250,30,16,"Font Updataing,Please Wait...",RED);
        fonts_update_font(10,70,16,(uint8_t *)"0:", RED);
        lcd_show_string(10,50,280,30,16,"Font Updata finshed,Please Restart!",RED);
        OSSchedUnlock(&err);
        GUI_SetColor(forecolor);   /* 恢复前景色 */
        GUI_SetBkColor(bkcolor);   /* 恢复背景色 */
        OSSched();
    }
}

/**
 * @brief       截图
 * @param       p_arg : 传入参数(未用到)
 * @retval      无
 */
void image_task(void *p_arg)
{
    OS_ERR err;
    p_arg = p_arg;

    while (1)
    {
        OSSemPend(&Imsge_SEM, 0,OS_OPT_PEND_BLOCKING,0,&err); /* 请求截图信号量 */
        OSSchedLock(&err);
        LED1(0);
        emwin_create_gifpicture((uint8_t *)"0:/SCREENSHORT/屏幕截图0.bmp",0,0,lcddev.width,lcddev.height); /* 创建BMP截图 */
        LED1(1);
        OSSchedUnlock(&err);
        OSSched();
    }
}
