/**
 ****************************************************************************************************
 * @file        progressbar.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.2
 * @date        2022-05-26
 * @brief       GUI-进度条 代码
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
 * V1.1 20140722
 * 1,修改progressbar_check函数加入模糊检测
 * V1.2 20220526
 * 1, 修改注释方式
 * 2, 修改u8/u16/u32为uint8_t/uint16_t/uint32_t
 ****************************************************************************************************
 */

#ifndef __PROGRESSBAR_H
#define __PROGRESSBAR_H

#include "guix.h"


/* 进度条上的符号的默认颜色 */
#define PRGB_DFT_BKACOLOR       0XEF7D      /* 背景色A颜色 */
#define PRGB_DFT_BKBCOLOR       0XE73C      /* 背景色B颜色 */
#define PRGB_DFT_FILLACOLOR     0X0FBB      /* 填充色A颜色 */
#define PRGB_DFT_FILLBCOLOR     0X071F      /* 填充色B颜色 */
#define PRGB_DFT_RIMCOLOR       0X4A49      /* 边框颜色 */
#define PRGB_DFT_BTNCOLOR       0X0000      /* 浮动条/文字颜色,黑色 */

/* 进度条type */
#define PRGB_TYPE_HOR           0X00        /* 水平的 */
#define PRGB_TYPE_VER           0X80        /* 垂直的 */
#define PRGB_TYPE_FEN           0X40        /* 浮标是否显示(百分进度条的时候,表示百分数是否显示) */
#define PRGB_TYPE_PEN           0X20        /* 进度柱是否显示 */
#define PRGB_TYPE_STD           0X00        /* 标准进度条 */
#define PRGB_TYPE_TEXT          0X01        /* 百分数进度条 */
#define PRGB_BTN_DOWN           0X01        /* 中间按钮被按下了 */


/* 进度条结构体定义 */
typedef __PACKED_STRUCT
{
    uint16_t top;           /* 进度条顶端坐标 */
    uint16_t left;          /* 进度条左端坐标 */
    uint16_t width;         /* 宽度(至少大于40) */
    uint16_t height;        /* 高度(至少大于2) */

    uint8_t type;           /**
                             * 类型标记字
                             * [bit7]:方向,0,水平;1,垂直;
                             * [bit6]:0,不显示浮标;1,显示浮标;
                             * [bit5]:0,不显示进度柱,1,显示进度柱(其实就是颜色是否填充,是否跟着浮标填充颜色.)
                             * [bit4:2]:保留;
                             * [bit1:0]:0,标准进度条;1,百分数进度条;2,3,保留.
                             */
    
    uint8_t sta;            /* 进度条状态 bit0:进度条被按下 */
    uint8_t id;             /* 进度条id */

    uint32_t totallen;      /* 总长度 */
    uint32_t curpos;        /* 当前位置 */
    uint16_t prgbarlen;     /* 滚条长度 由用户设定.仅在标准滑动条时有效 */

    uint16_t inbkcolora;    /* 内部背景颜色 */
    uint16_t inbkcolorb;    /* 内部背景颜色 */
    uint16_t infcolora;     /* 内部填充颜色a */
    uint16_t infcolorb;     /* 内部填充颜色b */
    uint16_t btncolor;      /* 浮标填充色(标准prg时为内部滑动点的颜色,百分数prg时,为字体颜色,扩展时,无效) */
    uint16_t rimcolor;      /* 边框颜色 */
} _progressbar_obj;


_progressbar_obj *progressbar_creat(uint16_t left, uint16_t top, uint16_t width, uint16_t height, uint8_t type);
uint8_t progressbar_check(_progressbar_obj *prgbx, void *in_key);
void progressbar_delete(_progressbar_obj *progressbar_del);
void progressbar_hook(_progressbar_obj *prgbx);
void progressbar_draw_progressbar(_progressbar_obj *prgbx);
void progressbar_setpos(_progressbar_obj *prgbx);
void progressbar_draw_sign(_progressbar_obj *prgbx, uint16_t x, uint16_t y);
void progressbar_show_percnt(uint16_t x, uint16_t y, uint8_t dir, uint8_t num, uint16_t color);

#endif





