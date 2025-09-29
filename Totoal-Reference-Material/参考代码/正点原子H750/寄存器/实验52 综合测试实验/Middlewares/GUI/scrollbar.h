/**
 ****************************************************************************************************
 * @file        scrollbar.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.1
 * @date        2022-05-26
 * @brief       GUI-滚动条 代码
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
 * V1.0 20140722
 * 第一次发布
 * V1.1 20220526
 * 1, 修改注释方式
 * 2, 修改u8/u16/u32为uint8_t/uint16_t/uint32_t
 ****************************************************************************************************
 */

#ifndef __SCROLLBAR_H
#define __SCROLLBAR_H

#include "guix.h"


/* 滚动条上的符号的颜色 */
#define SCROLLBAR_SIGN_RCOLOR   0X0000      /* 符号松开颜色为黑色 */
#define SCROLLBAR_SIGN_PCOLOR   0XFFFF      /* 符号按下颜色为白色 */

/* 滚动条尺寸设定 */
#define SCROLLBAR_PART_LEN      16          /* 两端按钮的长度(即宽度) */
#define SCROLLBAR_MIN_THICK     10          /* 最小的厚度(即高度) */

/* 方向定义 */
#define SCROLLBAR_DIR_HOR       0X00        /* 水平scrollbar */
#define SCROLLBAR_DIR_VER       0X80        /* 垂直scrollbar */

/* 图标编号 */
#define SCROLLBAR_SIGN_UP       0X00        /* 上图标 */
#define SCROLLBAR_SIGN_DOWN     0X01        /* 下图标 */
#define SCROLLBAR_SIGN_LEFT     0X02        /* 左图标 */
#define SCROLLBAR_SIGN_RIGHT    0X03        /* 右图标 */
#define SCROLLBAR_SIGN_HOR      0X04        /* 水平图标 */
#define SCROLLBAR_SIGN_VER      0X05        /* 垂直图标 */

/* 按钮定义 */
#define SCROLLBAR_BTN_LTP       0X01        /* 左端/顶部按钮被按下 */
#define SCROLLBAR_BTN_MID       0X02        /* 中间按下 */
#define SCROLLBAR_BTN_RBP       0X04        /* 右端/底部按钮被按下 */

/* 滚动条类型定义 */
#define SCROLLBAR_TYPE_STD      0X00
#define SCROLLBAR_TYPE_NBT      0X01

/* 滚动条默认颜色 */
#define SCLB_DFT_INBKCOLOR      0XFFFF      /* 填充色A颜色 */
#define SCLB_DFT_BTNCOLOR       0X2DDA      /* 0X031F, 填充色B颜色 */
#define SCLB_DFT_RIMCOLOR       0X4A49      /* 边框颜色 */


/* 滚动条结构体定义 */
typedef __PACKED_STRUCT
{
    uint16_t top;           /* 按钮顶端坐标 */
    uint16_t left;          /* 按钮左端坐标 */
    uint16_t width;         /* 宽度(至少大于40) */
    uint16_t height;        /* 高度(至少大于2) */
    
    uint8_t type;           /* 类型标记字节 [bit7]:方向,0,水平;1,垂直;bit[6:2]:保留;[bit1:0]:按钮类型,0,标准滚动条;1,没有端按钮的滚动条;2,3,保留. */
    uint8_t sta;            /* 按钮状态 bit0:左端/顶部按钮;bit1:中间按钮;bit2:右端/底部按钮. */
    uint8_t id;             /* 滚动条id */

    uint16_t totalitems;    /* 总条目数 */
    uint16_t itemsperpage;  /* 每页显示的条目数 */
    uint16_t topitem;       /* 最顶/左端的条目 */
    uint16_t scbbarlen;     /* 滚条长度 */

    uint16_t inbkcolor;     /* 内部颜色 */
    uint16_t btncolor;      /* 符号按钮填充色 */
    uint16_t rimcolor;      /* 边框颜色 */
} _scrollbar_obj;


_scrollbar_obj *scrollbar_creat(uint16_t left, uint16_t top, uint16_t width, uint16_t height, uint8_t type);
void scrollbar_delete(_scrollbar_obj *scrollbar_del);
void scrollbar_draw_sign(uint16_t x, uint16_t y, uint16_t signcolor, uint8_t signx);
void scrollbar_draw_btn(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t bkcolor, uint16_t outcolor, uint16_t signcolor, uint8_t signx);
void scrollbar_setpos(_scrollbar_obj *scbx);                    /* 设置滑动条位置 */
void scrollbar_draw_scrollbar(_scrollbar_obj *scbx);            /* 画滑动条 */
uint8_t scrollbar_check(_scrollbar_obj *scbx, void *in_key);    /* 滑动条按键处理函数 */
void scrollbar_hook(_scrollbar_obj *scbx);                      /* 钩子函数 */

void test_scb(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t type, uint16_t icur, uint16_t incolor, uint16_t btncolor, uint16_t outcolor);

#endif
































