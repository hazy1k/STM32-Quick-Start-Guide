/**
 ****************************************************************************************************
 * @file        calculator.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.1
 * @date        2022-05-26
 * @brief       APP-科学计算器 代码
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

#ifndef __CALCULATOR_H
#define __CALCULATOR_H

#include "common.h"


/* 各图标/图片路径 */
extern uint8_t *const CALC_UP_BTN_TBL[29];      /* 松开时按键图标路径 */
extern uint8_t *const CALC_DOWN_BTN_TBL[29];    /* 按下时按键图标路径 */


typedef __PACKED_STRUCT
{
    uint16_t xoff;      /* x方向偏移:2,10,10 */
    uint16_t yoff;      /* y方向偏移:10,20,50 */
    uint16_t width;     /* 宽度 */
    uint16_t height;    /* 高度 */
    uint8_t xdis;       /* 数据区域x方向间隔大小:2,4,2 */
    uint8_t ydis;       /* 数据区域y方向间隔大小:5,11,19 */
    uint8_t fsize;      /* 主结果字体大小:28,36,60 */
} _calcdis_obj;
extern _calcdis_obj *cdis;

uint8_t calc_play(void);
uint8_t calc_show_res(_calcdis_obj *calcdis, double res);
void calc_input_fresh(_calcdis_obj *calcdis, uint8_t *calc_sta, uint8_t *inbuf, uint8_t len);
void calc_ctype_show(_calcdis_obj *calcdis, uint8_t ctype);
void calc_show_flag(_calcdis_obj *calcdis, uint8_t fg);
void calc_show_exp(_calcdis_obj *calcdis, short exp);
uint8_t calc_exe(_calcdis_obj *calcdis, double *x1, double *x2, uint8_t *buf, uint8_t ctype, uint8_t *calc_sta);
void calc_show_inbuf(_calcdis_obj *calcdis, uint8_t *buf);
void calc_load_ui(_calcdis_obj *calcdis);

#endif























