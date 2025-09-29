/**
 ****************************************************************************************************
 * @file        nesplay.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.2
 * @date        2022-05-26
 * @brief       APP-NES模拟器 代码
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
 * V1.1 20160627
 * 新增对SMS游戏的支持
 * V1.2 20220526
 * 1, 修改注释方式
 * 2, 修改u8/u16/u32为uint8_t/uint16_t/uint32_t
 ****************************************************************************************************
 */

#ifndef __NESPLAY_H
#define __NESPLAY_H

#include "common.h"


extern uint8_t nesruning ;  /* 退出NES的标志 */
extern uint8_t frame_cnt;   /* 统计帧数 */
 
void nes_clock_set(uint8_t PLL);
void load_nes(uint8_t* path);
uint8_t nes_play(void);

#endif




























