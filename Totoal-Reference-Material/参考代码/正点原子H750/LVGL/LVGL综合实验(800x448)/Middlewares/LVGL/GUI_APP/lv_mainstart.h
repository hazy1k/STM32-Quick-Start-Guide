/**
 ****************************************************************************************************
 * @file        lv_mainstart.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-03-23
 * @brief       LVGL综合 实验
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
 
#ifndef __LV_MAINSTART_H
#define __LV_MAINSTART_H
#include "lvgl.h"
#include "./FATFS/exfuns/exfuns.h"


 /* 返回控制器 */
typedef struct
{
    void (*lv_general_win_create)(void); /* 初始化返回控制器 */
    lv_obj_t* parent;
    lv_event_cb_t lv_back_event;
    uint8_t ov5640_flags;
} lv_m_general;

#define BACK_BTN_TITLE    LV_SYMBOL_LEFT" Back\n"   /* 返回按键问题内容 */

extern lv_m_general lv_general_dev;     /* 返回控制器 */


void lv_general_win_create(void);
int lv_clz(unsigned int  app_readly_list[]);
void lv_mainstart(void);

#endif
