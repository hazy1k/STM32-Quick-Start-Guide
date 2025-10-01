/**
 ****************************************************************************************************
 * @file        lv_mainstart.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.4
 * @date        2022-01-04
 * @brief       LVGL 智能家居实验
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
#include "lvgl/lvgl.h"


LV_IMG_DECLARE(light_contro_win)
LV_IMG_DECLARE(air_conditioning_control_win)
LV_IMG_DECLARE(curtain_control_wi)
LV_IMG_DECLARE(uase_win)
LV_IMG_DECLARE(music_win)
LV_IMG_DECLARE(lv_light)
LV_IMG_DECLARE(lv_light_bg)
LV_IMG_DECLARE(lv_leave_home)
LV_IMG_DECLARE(lv_Warm)
LV_IMG_DECLARE(lv_read)
LV_IMG_DECLARE(lv_home)
LV_IMG_DECLARE(lv_recreation)
LV_IMG_DECLARE(lv_cinema)


void lv_mainstart(void);


#endif
