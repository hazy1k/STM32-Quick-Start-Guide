/**
 ****************************************************************************************************
 * @file        ffsystem.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-01-14
 * @brief       FATFS底层(ffsystem) 驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 STM32F103开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.0 20220114
 * 第一次发布
 *
 ****************************************************************************************************
 */

#include "./MALLOC/malloc.h"
#include "./SYSTEM/sys/sys.h"
#include "./FATFS/source/ff.h"
#include "./SYSTEM/usart/usart.h"
#include "ucos_ii.h"
#include "calendar.h"

volatile uint8_t cnt0 = 0;
volatile uint8_t cnt1 = 0;


OS_CPU_SR cpu_sr = 0;


/**
 * @brief       进入临界区
 * @param       fs   : FATFS指针
 * @retval      无
 */
void ff_enter(FATFS *fs)
{
    if (cnt0)
    {
        printf("in shit:%d\r\n", cnt0);
    }

    if (fs->pdrv != 2)
    {
        OS_ENTER_CRITICAL();    /* 进入临界区(无法被中断打断) */
        cnt0++;
    }
    else
    {
        OSSchedLock();          /* 阻止ucos调度 */
        cnt1++;
    }
}

/**
 * @brief       退出临界区
 * @param       fs   : FATFS指针
 * @retval      无
 */
void ff_leave(FATFS *fs)
{
    if (cnt0)
    {
        cnt0--;
        OS_EXIT_CRITICAL();     /* 退出临界区(可以被中断打断) */
    }

    if (cnt1)
    {
        cnt1--;
        OSSchedUnlock();        /* 开启ucos调度 */
    }
}

/**
 * @brief       获得时间
 * @param       mf  : 内存首地址
 * @retval      时间
 *   @note      时间编码规则如下:
 *              User defined function to give a current time to fatfs module
 *              31-25: Year(0-127 org.1980), 24-21: Month(1-12), 20-16: Day(1-31)
 *              15-11: Hour(0-23), 10-5: Minute(0-59), 4-0: Second(0-29 *2)
 */
DWORD get_fattime (void)
{
    uint32_t time = 0;
    calendar_get_date(&calendar);
    calendar_get_time(&calendar);

    if (calendar.w_year < 1980)calendar.w_year = 1980;

    time = (calendar.w_year - 1980) << 25; //年份
    time |= (calendar.w_month) << 21;	//月份
    time |= (calendar.w_date) << 16;	//日期
    time |= (calendar.hour) << 11;		//时
    time |= (calendar.min) << 5;		//分
    time |= (calendar.sec / 2);			//秒
    return time;
}

/**
 * @brief       动态分配内存
 * @param       size : 要分配的内存大小(字节)
 * @retval      分配到的内存首地址.
 */
void *ff_memalloc (UINT size)
{
    return (void *)mymalloc(SRAMIN, size);
}

/**
 * @brief       释放内存
 * @param       mf  : 内存首地址
 * @retval      无
 */
void ff_memfree (void *mf)
{
    myfree(SRAMIN, mf);
}

















