/**
 ****************************************************************************************************
 * @file        lv_action.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-03-23
 * @brief       LVGL GIF
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

#ifndef __LV_ACTION_H
#define __LV_ACTION_H
#include "lvgl.h"

extern uint8_t ov5640_flags;

void lv_rgb565_show_back(void);
void lv_rgb565_test(void);
void sw_sdcard_mode(void);
void sw_ov5640_mode(void);

#endif
