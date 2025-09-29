/**
 ****************************************************************************************************
 * @file        gyroscope.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.1
 * @date        2022-05-26
 * @brief       APP-陀螺仪测试 代码
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

#ifndef __GYROSCOPE_H
#define __GYROSCOPE_H

#include "common.h"

extern uint8_t *const gyro_remind_tbl[2][GUI_LANGUAGE_NUM];

typedef int32_t s32;
typedef int16_t s16;
typedef int8_t  s8;

uint8_t gyro_play(void);

#endif







