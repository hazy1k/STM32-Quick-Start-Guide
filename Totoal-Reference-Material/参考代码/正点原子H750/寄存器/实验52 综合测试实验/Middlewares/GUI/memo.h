/**
 ****************************************************************************************************
 * @file        memo.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.2
 * @date        2022-05-26
 * @brief       GUI-文本框 代码
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
 * V1.1 20140814
 * 1,修改memo_draw_memo函数,添加模式控制,实现从头显示/从尾显示
 * V1.2 20220526
 * 1, 修改注释方式
 * 2, 修改u8/u16/u32为uint8_t/uint16_t/uint32_t
 ****************************************************************************************************
 */

#ifndef __MEMO_H
#define __MEMO_H

#include "guix.h"
#include "scrollbar.h"


/* memo默认的文字颜色及背景色 */
#define MEMO_DFT_TBKC       0XFFFF      /* 默认的文本背景色 */
#define MEMO_DFT_TXTC       0X0000      /* 默认的文本颜色 */

/* memo的滚动条宽度定义 */
#define MEMO_SCB_WIDTH      0X0E        /* 滚动条宽度为15个像素 */


/* memo结构体定义 */
typedef __PACKED_STRUCT
{
    uint16_t top;           /* memo顶端坐标 */
    uint16_t left;                       /* memo左端坐标 */
    uint16_t width;         /* memo宽度(包含滚动条的宽度) */
    uint16_t height;        /* memo高度 */

    uint8_t id;             /* memoID */
    uint8_t type;           /** 
                             * memo类型
                             * [7:1]:保留
                             * [0]:0,不允许编辑;1,允许编辑
                             */

    uint8_t sta;            /**
                             * memo状态
                             * [7]:0,memo没有检测到按下事件;1,有按下事件.
                             * [6]:临时数据有效标志
                             * [5]:滑动标志
                             * [4:2]:保留
                             * [1]:光标显示状态 0,未显示.1,显示.
                             * [0]:memo选中状态 0,未选中.1,选中.
                             */

    uint32_t textlen;       /* memo字符串text的长度 */
    uint8_t *text;          /* memo的内容,由textlen的大小来决定 */
    uint32_t *offsettbl;    /* 字符串偏移量表,记录每行开始的字符串在text里面的位置,大小为:scbv->totalitems */
    uint8_t font;           /* text文字字体 */

    uint16_t lin;           /* 光标所在的行位置(是指所有行) */
    uint16_t col;           /* 光标所在列位置(按宽度除以(font/2)计算) */

    uint16_t typos;         /* 临时的y坐标 */
    uint16_t txpos;         /* 临时的x坐标 */

    uint32_t memo_timer_old;/* 上一次时间 */

    _scrollbar_obj *scbv;   /* 垂直滚动条 */
    uint16_t textcolor;     /* 文字颜色 */
    uint16_t textbkcolor;   /* 文字背景色 */
} _memo_obj;

_memo_obj *memo_creat(uint16_t left, uint16_t top, uint16_t width, uint16_t height, uint8_t id, uint8_t type, uint8_t font, uint32_t textlen);
void memo_delete(_memo_obj *memo_del);
void memo_draw(_memo_obj *memox);


uint8_t memo_set_fontsize(_memo_obj *memox, uint8_t size);
uint16_t memo_get_cursorxpos(_memo_obj *memox);
void memo_read_cursorbkctbl(_memo_obj *memox);
void memo_draw_cursor(_memo_obj *memox, uint16_t color);
void memo_recover_cursorbkctbl(_memo_obj *memox);
void memo_show_cursor(_memo_obj *memox, uint8_t sta);
void memo_cursor_flash(_memo_obj *memox);
void memo_draw_text(_memo_obj *memox);
void memo_add_text(_memo_obj *memox, uint8_t *str);
void memo_draw_memo(_memo_obj *memox, uint8_t mode);
uint8_t memo_check(_memo_obj *memox, void *in_key);

//void memo_test(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t type, uint8_t sta, uint16_t textlen);

#endif

















