/**
 ****************************************************************************************************
 * @file        iconviewdemo.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-01-04
 * @brief       emWin的CONVIEW部件使用实验
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

#ifndef _ICONVIEWDEMO_H
#define _ICONVIEWDEMO_H
#include "./SYSTEM/sys/sys.h"
#include "DIALOG.h"


extern WM_HWIN BagAPPWin;           /* BagAPP窗口 */
extern WM_HWIN BookAPPWin;          /* BookAPP窗口 */
extern WM_HWIN BookshAPPWin;        /* BookshAPP窗口 */
extern WM_HWIN BrowerAPPWin;        /* BrowerAPP窗口 */

void CreateBagAPP(void);            /* 创建BagAPP */
void CreateBookAPP(void);           /* 创建BookAPP */
void CreateBookshAPP(void);         /* 创建BookshAPP */
void CreateBrowerAPP(void);         /* 创建BrowerAPP */
#endif

