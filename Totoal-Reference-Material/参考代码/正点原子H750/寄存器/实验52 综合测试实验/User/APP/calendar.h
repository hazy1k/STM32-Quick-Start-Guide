/**
 ****************************************************************************************************
 * @file        calendar.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.1
 * @date        2022-05-26
 * @brief       APP-日历 代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.1 20220526
 * 1, 修改注释方式
 * 2, 修改u8/u16/u32为uint8_t/uint16_t/uint32_t
 ****************************************************************************************************
 */

#ifndef __CALENDAR_H
#define __CALENDAR_H

#include "./BSP/RTC/rtc.h"
#include "./SYSTEM/sys/sys.h"
#include "os.h"
#include "common.h"


/* 时间结构体 */
typedef struct
{
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
    /* 公历日月年周 */
    uint16_t w_year;
    uint8_t  w_month;
    uint8_t  w_date;
    uint8_t  week;  /* 1~7,代表周1~周日 */
} _calendar_obj;
extern _calendar_obj calendar;  /* 日历结构体 */

/* 闹钟结构体 */
typedef __PACKED_STRUCT
{
    uint8_t  weekmask;      /* 闹钟响铃掩码 bit0~bit6,代表周日~周6 */
    uint8_t  ringsta;       /**
                             * 闹铃状态及铃声标志
                             * [7]:0,无闹铃;1,闹铃中;
                             * [6:3]:保留
                             * [2:0]:闹钟铃声类型
                             */
    uint8_t  hour;          /* 闹铃小时 */
    uint8_t  min;           /* 闹铃分钟 */
    uint8_t saveflag;       /* 保存标志,0X0A,保存过了;其他,还从未保存 */
} _alarm_obj;

extern _alarm_obj alarm;    /* 闹钟结构体 */
extern uint8_t *const calendar_week_table[GUI_LANGUAGE_NUM][7]; /* 外部定义的星期表 */

void calendar_alarm_init(_alarm_obj *alarmx, _calendar_obj *calendarx);
void calendar_alarm_ring(uint8_t type);
void calendar_get_time(_calendar_obj *calendarx);
void calendar_get_date(_calendar_obj *calendarx);
void calendar_date_refresh(void);
void calendar_read_para(_alarm_obj *alarm);
void calendar_save_para(_alarm_obj *alarm);
uint8_t calendar_alarm_msg(uint16_t x, uint16_t y);
void calendar_circle_clock_drawpanel(uint16_t x, uint16_t y, uint16_t size, uint16_t d);
void calendar_circle_clock_showtime(uint16_t x, uint16_t y, uint16_t size, uint16_t d, uint8_t hour, uint8_t min, uint8_t sec);
uint8_t calendar_play(void);

#endif












