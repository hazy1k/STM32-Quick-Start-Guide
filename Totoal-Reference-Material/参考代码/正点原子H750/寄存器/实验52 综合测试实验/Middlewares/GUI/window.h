/**
 ****************************************************************************************************
 * @file        window.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.2
 * @date        2022-05-26
 * @brief       GUI-窗体 代码
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
 * V1.1 20140815
 * 1,修正window_creat函数和window_delete函数的小bug
 * 2,修改window_msg_box函数,新增时间参数
 * V1.1 20220526
 * 1, 修改注释方式
 * 2, 修改u8/u16/u32为uint8_t/uint16_t/uint32_t
 ****************************************************************************************************
 */

#ifndef __WINDOW_H
#define __WINDOW_H

#include "guix.h"
#include "button.h"


/* 默认的窗体caption背景色caption的颜色, 以及窗体背景色 */
#define WIN_CAPTION_UPC                 0X7D7C      /* 窗口CAPTION上部分填充色 */
#define WIN_CAPTION_DOWNC               0X5458      /* 窗口CAPTION下部分填充色 */
#define WIN_CAPTION_COLOR               0XFFFF      /* 窗口名颜色为白色 */
#define WIN_BODY_BKC                    0XD699      /* 窗口底色,0XEF5D */
#define WIN_CAPTION_HEIGHT              0X20        /* 默认的高度 */

/* 窗体上的关闭按钮相关信息定义 */
#define WIN_BTN_SIZE                    0X18        /* 正方形尺 寸边长(必须小于WIN_CAPTION_HEIGHT) */
#define WIN_BTN_OFFSIDE                 0X06        /* 按钮/标题偏离边框的像素 */
#define WIN_BTN_FUPC                    0XFFFF      /* 按钮松开时文字的颜色 */
#define WIN_BTN_FDOWNC                  0XDCD0      /* 按钮按下时文字的颜色 */
#define WIN_BTN_RIMC                    0XFE18      /* 边框颜色 */
#define WIN_BTN_TP1C                    0XF36D      /* 第一行的颜色 */
#define WIN_BTN_UPHC                    0XF36D      /* 上半部分颜色,0X6D3C */
#define WIN_BTN_DNHC                    0XC800      /* 下半部分颜色,0XAE5C */

/* 标准窗体边框颜色定义 */
#define STD_WIN_RIM_OUTC                0X3317      /* 外线颜色 */
#define STD_WIN_RIM_MIDC                0X859B      /* 中线颜色 */
#define STD_WIN_RIM_INC                 0X2212      /* 内线颜色 */

/* 圆边窗体边框颜色定义 */
#define ARC_WIN_RIM_COLOR               0XB595      /* 边线颜色 */


/* window结构体定义 */
typedef __PACKED_STRUCT
{
    uint16_t top;           /* window顶端坐标 */
    uint16_t left;          /* window左端坐标 */
    uint16_t width;         /* window宽度(包含滚动条的宽度) */
    uint16_t height;        /* window高度 */

    uint8_t id;             /* window */
    uint8_t type;           /**
                             * window类型
                             * [7]:0,没有关闭按钮.1,有关闭按钮
                             * [6]:0,不读取背景色.1,读取背景色.
                             * [5]:0,标题靠左.1,标题居中.
                             * [4:2]:保留
                             * [1:0]:0,标准的窗口(仿XP);1,圆边窗口(仿Andriod)
                             */

    uint8_t sta;            /* window状态 [7:0]:保留 */

    uint8_t *caption;       /* window名字 */
    uint8_t captionheight;  /* caption栏的高度 */
    uint8_t font;           /* window文字字体 */
    uint8_t arcwinr;        /* 圆角窗口的圆角的半径 */

    uint16_t captionbkcu;   /* caption的上半部分背景色 */
    uint16_t captionbkcd;   /* caption的下半部分背景色 */
    uint16_t captioncolor;  /* caption的颜色 */
    uint16_t windowbkc;     /* window的背景色 */

    uint16_t *bkctbl;       /* 背景色表(需要读取窗体背景色的时候用到) */
    _btn_obj *closebtn;     /* 串口关闭按钮 */
} _window_obj;

_window_obj *window_creat(uint16_t left, uint16_t top, uint16_t width, uint16_t height, uint8_t id, uint8_t type, uint8_t font);
void window_delete(_window_obj *window_del);
void window_draw(_window_obj *windowx);
uint8_t window_msg_box(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t *str, uint8_t *caption, uint8_t font, uint16_t color, uint8_t mode, uint16_t time);

void win_test(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t type, uint16_t cup, uint16_t cdown, uint8_t *caption);
#endif


















