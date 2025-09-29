/**
 ****************************************************************************************************
 * @file        t9input.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.2
 * @date        2022-05-26
 * @brief       T9拼音输入法 代码
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
 * V1.1 20160325
 * 修正t9_creat的bug,高分辨率屏幕也可以使用了.
 * V1.1 20220526
 * 1, 修改注释方式
 * 2, 修改u8/u16/u32为uint8_t/uint16_t/uint32_t
 ****************************************************************************************************
 */

#ifndef __T9INPUT_H
#define __T9INPUT_H

#include "guix.h"
#include "button.h"
#include "./T9INPUT/pyinput.h"
#include "./ATKNCR/atk_ncr.h"


/* 标准按钮边框颜色定义 */
#define EDIT_RIM_LTOC   0XA535      /* 左上外线颜色 */
#define EDIT_RIM_LTIC   0X8431      /* 左上内线颜色 */
#define EDIT_RIM_RBOC   0XFFFF      /* 右下外线颜色 */
#define EDIT_RIM_RBIC   0XDEFC      /* 右下内线颜色 */
#define EDIT_BACK_COLOR 0XFFFF      /* 编辑框背景色 */

#define EDIT_DFT_TBKC   0XFFFF      /* 默认的文本背景色 */
#define EDIT_DFT_TXTC   0X0000      /* 默认的文本颜色 */


#define T9_SIGNTBL_SIZE 5

#define T9_OUTSEL_BKC   0XFFFF      /* 输出选择背景色 */
#define T9_OUTSEL_SELC  0X0000      /* 输出选中背景色 */
#define T9_BTN_EBC      0XD6BA      /* 按钮失效颜色 */
#define T9_BTN_DBC      0XFFFF      /* 按钮有效颜色 */


#define T9_PANEL_BKC    0X4A6A      /* 输入法背景色 */

/* EDIT结构体定义 */
typedef __PACKED_STRUCT
{
    uint16_t top;       /* 按钮顶端坐标 */
    uint16_t left;      /* 按钮左端坐标 */
    uint16_t width;     /* 宽度(固定240) */
    uint16_t height;    /* 高度(固定134) */
    uint8_t mode;       /**
                         * 输入框类型
                         * [7:5]:0,保留
                         * [4:3]:(左侧纵向按钮状态)0,输入法切换状态;1,汉字输入状态;2,标点符号状态;
                         * [2:0]:0,拼音;1,123;2,abc;3,ABC,4,手写.
                         */
    uint8_t sta;        /**
                         * 输入法状态
                         * [7]:0,横向上一页按钮无效.1,横向上一页按钮有效
                         * [6]:0,横向下一页按钮无效.1,横向下一页按钮有效
                         * [5:2]:保留
                         * [0]:0,没有任何输入数据
                         */
    uint8_t signpage;   /* 当前字符页面 */
    uint8_t pynum;      /* 拼音总匹配数 */

    _btn_obj *mbtn[12]; /* 12个主按钮 */
    _btn_obj *hbtn[8];  /* 横向8个选择按钮 */
    _btn_obj *vbtn[5];  /* 纵向5个选择按钮 */

    py_index *pymb[MAX_MATCH_PYMB]; /* 码表存放位置 */
    uint8_t pypage;             /* 当前拼音所在页 */

    uint8_t *disstr;            /* 横向显示的字符串指针 */
    uint8_t dispagenum;         /* 字符串总页数 */
    uint8_t dispagecur;         /* 当前页 */
    uint8_t disbuf[6][3];       /* 横向按钮的caption缓存 */

    uint8_t instr[7];           /* 输入字符串,最大为6个字节+结束符1个.用于拼音输入和手写识别 */
    uint8_t outstr[5];          /* 输出字符,最大为2个字节*2+结束符1个,本来只要3个字节,因为标点符号可能成对.所以需要5个字节 */

    atk_ncr_point pbuf[200];    /* 识别输入最大点数 */
    uint8_t ncr_status;         /**
                                 * 识别状态
                                 * [7]:0,没有输入.1,有输入;
                                 * [6:0]:计时器,用于记录连续多久没有输入
                                 */
    uint8_t ncr_tcnt;           /* 记录的触摸按键次数 */
    uint32_t ncr_timer_old;     /* gui定时器的上一次时间值 */

    uint16_t ncr_top;
    uint16_t ncr_left;
    uint16_t ncr_width;
    uint16_t ncr_height;
} _t9_obj;

_t9_obj *t9_creat(uint16_t left, uint16_t top, uint16_t width, uint16_t height, uint8_t mode);
void t9_delete(_t9_obj *t9_del);

void t9_ncr_clr(_t9_obj *t9x);
uint8_t t9_check(_t9_obj *t9x, void *in_key);
void t9_draw_mbtn(_t9_obj *t9x);
void t9_draw_hbtn(_t9_obj *t9x);
void t9_draw_vbtn(_t9_obj *t9x);
void t9_draw(_t9_obj *t9x);
void t9_hbtn_updc(_t9_obj *t9x, uint8_t *str);

void t9_set_inputtype(_t9_obj *t9x, uint8_t type);
uint8_t t9_keyup_scan(_t9_obj *t9x);
uint8_t t9_id2num(uint8_t id);
void t9_clrhbtn_caption(_t9_obj *t9x, uint8_t btnx);
void t9_load_sign(_t9_obj *t9x);
void t9_drawvbtn_rim(_t9_obj *t9x, uint16_t rimcolor);
void t9_sign_process(_t9_obj *t9x, uint8_t id);
void t9_back_process(_t9_obj *t9x);
void t9_space_process(_t9_obj *t9x);
void t9_enter_process(_t9_obj *t9x);
void t9_hbtn_process(_t9_obj *t9x, uint8_t id);
void t9_vbtn_process(_t9_obj *t9x, uint8_t id);
void t9_pinyin_process(_t9_obj *t9x, uint8_t id);
void t9_process(_t9_obj *t9x);
void t9_test_task(void);
void t9_test(uint16_t x, uint16_t y, uint8_t mode);
void t9_tsdel(void);
void t9_tsdraw(void);

#endif

















