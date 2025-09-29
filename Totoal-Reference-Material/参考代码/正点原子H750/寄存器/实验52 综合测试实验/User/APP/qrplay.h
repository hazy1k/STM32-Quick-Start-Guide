/**
 ****************************************************************************************************
 * @file        qrplay.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.1
 * @date        2022-05-26
 * @brief       APP-二维码识别&编码 代码
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

#ifndef __QRPLAY_H
#define __QRPLAY_H

#include "common.h"


#define QR_BACK_COLOR           0XA599      /* 背景色 */

/* 在qrplay.c里面定义 */
extern volatile uint8_t qr_dcmi_rgbbuf_sta; /* RGB BUF状态 */
extern uint16_t qr_dcmi_curline;            /* 摄像头输出数据,当前行编号 */

void qr_cursor_show(uint8_t csize);
void qr_dcmi_rx_callback(void);
void qr_decode_show_result(uint8_t* result);
void qr_decode_play(void);
void qr_encode_play(void);
void qr_play(void);

#endif























