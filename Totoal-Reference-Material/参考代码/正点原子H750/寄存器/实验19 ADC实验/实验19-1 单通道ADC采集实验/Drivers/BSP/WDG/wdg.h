/**
 ****************************************************************************************************
 * @file        wdg.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.1
 * @date        2020-03-13
 * @brief       看门狗 驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 STM32H750开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.0 20200312
 * 第一次发布
 * V1.1 20200312
 * 新增wwdg_init和wwdg_set_counter函数
 *
 ****************************************************************************************************
 */

#ifndef __WDG_H
#define __WDG_H

#include "./SYSTEM/sys/sys.h"


void iwdg_init(uint8_t prer, uint16_t rlr);
void iwdg_feed(void);
void wwdg_init(uint8_t tr, uint8_t wr, uint8_t fprer);
void wwdg_set_counter(uint8_t cnt);

#endif




























