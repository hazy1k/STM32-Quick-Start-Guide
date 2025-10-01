/**
 ****************************************************************************************************
 * @file        lv_qr.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-03-23
 * @brief       LVGL 驱动
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 Mini Pro H750开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 ****************************************************************************************************
 */
 
#ifndef __LV_QR_H
#define __LV_QR_H
#include "lvgl.h"


enum {
    LV_KEYBOARD_MODE_CAPITAL_ENGLISH,    /* 大写英文键盘模式 */
    LV_KEYBOARD_MODE_LOW_ENGLISH,        /* 小写英文键盘模式 */
    LV_KEYBOARD_MODE_NUM,                /* 数字键盘模式 */
    LV_KEYBOARD_MODE_T9,                 /* T9模式 */
};
typedef uint8_t lv_keyboard_mode_t;

typedef struct
{
    lv_obj_t *lv_t9_btnm;        /* 用于输入字符的按键 */
    lv_obj_t *roller_py;         /* 匹配拼音选择 */
    lv_obj_t *btnm_imsel;        /* 输入法选择 */
    lv_obj_t *lv_select_btnm;    /* 汉字选择 */
    lv_obj_t *lv_lower_btnm2;    /* 小写英文键盘 */
    lv_obj_t *lv_number_btnm1;   /* 数字键盘 */
    lv_obj_t *lv_english_btnm1;  /* 大写英文键盘 */
  
    struct
    {
        const char *map[20];     /* 总共16个btn，3个换行，一个结束 */
        char mapbuf[20][10];     /* 每个btn最多10个字节 */
        size_t pyindex;          /* 当前lv_select_btnm中显示的汉字对应的pylist和chslist的索引 */
        size_t chsindex;         /* lv_select_btnm中显示的汉字在chslist[pyindex]中的起始索引 */
        size_t chscnt;           /* chslist[pyindex]中所有的汉字的个数 */
        const char *pylist[10];  /* 和t9inbuf匹配的拼音列表 */
        const char *chslist[10]; /* 和pylist对应的汉字列表 */
    }lv_keydata;                 /* 用于设置lv_select_btnm的数据 */


    lv_keyboard_mode_t mode;
    char t9inbuf[10];

} lv_keybtm_t;

extern lv_keybtm_t *ext;

void lv_qr_windowm(void);

#endif
