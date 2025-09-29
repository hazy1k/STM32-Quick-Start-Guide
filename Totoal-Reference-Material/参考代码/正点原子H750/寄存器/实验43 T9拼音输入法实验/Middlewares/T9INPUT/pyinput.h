/**
 ****************************************************************************************************
 * @file        pyinput.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-05
 * @brief       拼音输入法 代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 STM32H750开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.0 20200405
 * 第一次发布
 *
 ****************************************************************************************************
 */

#ifndef __PYINPUT_H
#define __PYINPUT_H


#include "./SYSTEM/sys/sys.h"


/* 拼音码表与拼音的对应表 */
typedef struct
{
  char *py_input;    /* 输入的字符串 */
  char *py;          /* 对应的拼音 */
  char *pymb;        /* 码表 */
}py_index;

#define MAX_MATCH_PYMB      10          /* 最大匹配数 */

/* 拼音输入法 */
typedef struct
{
  uint8_t(*getpymb)(uint8_t *instr);    /* 字符串到码表获取函数 */
  py_index *pymb[MAX_MATCH_PYMB];       /* 码表存放位置 */
}pyinput;

extern pyinput t9;

uint8_t str_match(uint8_t*str1,uint8_t*str2);
uint8_t get_matched_pymb(uint8_t *strin,py_index **matchlist);
uint8_t get_pymb(uint8_t* str);
void test_py(uint8_t *inputstr);


#endif




















