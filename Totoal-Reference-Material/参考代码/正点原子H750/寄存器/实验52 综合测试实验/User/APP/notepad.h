/**
 ****************************************************************************************************
 * @file        notepad.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.1
 * @date        2022-05-26
 * @brief       APP-记事本 代码
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

#ifndef __NOTEPAD_H
#define __NOTEPAD_H

#include "common.h"


/* 定义最大的编辑长度.也就是新建文本文件的大小.或者打开一个旧文件后最大允许增加的长度 */
#define     NOTEPAD_EDIT_LEN    2048


void notepad_new_pathname(uint8_t *pname);
uint8_t notepad_play(void);

#endif























