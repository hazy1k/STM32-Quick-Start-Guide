/**
 ****************************************************************************************************
 * @file        edit.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.2
 * @date        2022-05-26
 * @brief       GUI-编辑框 代码
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
 * V1.1 20140825
 * 1, 修改edit_check函数加入模糊检测
 * V1.2 20220526
 * 1, 修改注释方式
 * 2, 修改u8/u16/u32为uint8_t/uint16_t/uint32_t
 ****************************************************************************************************
 */

#ifndef __EDIT_H
#define __EDIT_H

#include "guix.h"


/* 编辑框边框颜色定义 */
#define EDIT_RIM_LTOC   0XA535      /* 左上外线颜色 */
#define EDIT_RIM_LTIC   0X8431      /* 左上内线颜色 */
#define EDIT_RIM_RBOC   0XFFFF      /* 右下外线颜色 */
#define EDIT_RIM_RBIC   0XDEFC      /* 右下内线颜色 */
#define EDIT_BACK_COLOR 0XFFFF      /* 编辑框背景色 */

/* 编辑框文本颜色定义 */
#define EDIT_DFT_TBKC   0XFFFF      /* 默认的文本背景色 */
#define EDIT_DFT_TXTC   0X0000      /* 默认的文本颜色 */


/* EDIT结构体定义 */
typedef __PACKED_STRUCT
{
    uint16_t top;                   /* 编辑框顶端坐标 */
    uint16_t left;                  /* 编辑框左端坐标 */
    uint16_t width;                 /* 宽度(比字符串的长度多6个点) */
    uint16_t height;                /* 高度(比字体高度高6个点) */

    uint8_t id;                     /* 编辑框ID */
    
    uint8_t type;                   /**
                                     * 编辑框类型
                                     * [7:3]:保留
                                     * [2]:0,带边框;1,不带边框(边框采用背景色)
                                     * [1]:0,不显示光标;1,显示光标;
                                     * [0]:
                                     *     0,标准edit,满之不能再写入.
                                     *     1,自动清除edit,满之后自动清除,回到原位.
                                     */

    uint8_t sta;                    /**
                                     * 编辑框状态
                                     * [7:1]:保留
                                     * [1]:光标显示状态 0,未显示.1,显示.
                                     * [0]:编辑框选中状态 0,未选中.1,选中.
                                     */

    uint8_t *text;                  /* 编辑框的内容,长度<=width-6 */
    uint8_t font;                   /* text文字字体 */
    uint16_t cursorpos;             /* 光标的位置((font/2)*x坐标) */
    uint32_t edit_timer_old;        /* 上一次时间 */

    uint16_t textcolor;             /* 文字颜色 */
    uint16_t textbkcolor;           /* 文字背景色 */
} _edit_obj;

_edit_obj *edit_creat(uint16_t left, uint16_t top, uint16_t width, uint16_t height, uint8_t id, uint8_t type, uint8_t font);
void edit_delete(_edit_obj *edit_del);
uint16_t edit_get_cursorxpos(_edit_obj *editx);
void edit_draw(_edit_obj *editx);
void edit_read_cursorbkctbl(_edit_obj *editx);
void edit_draw_cursor(_edit_obj *editx, uint16_t color);
void edit_recover_cursorbkctbl(_edit_obj *editx);
void edit_show_cursor(_edit_obj *editx, uint8_t sta);
void edit_cursor_flash(_edit_obj *editx);
void edit_draw_text(_edit_obj *editx);
void edit_add_text(_edit_obj *editx, uint8_t *str);
void edit_draw(_edit_obj *editx);
uint8_t edit_check(_edit_obj *editx, void *in_key);

//void edit_test(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t type, uint8_t sta, uint8_t *text);

#endif
































