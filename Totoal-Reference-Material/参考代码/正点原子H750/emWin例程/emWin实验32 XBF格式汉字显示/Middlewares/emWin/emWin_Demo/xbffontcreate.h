/**
 ****************************************************************************************************
 * @file        xbffontcreate.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-01-04
 * @brief       emWin的XBF格式汉字显示实验
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

#ifndef _XBFFONTCREATE_H
#define _XBFFONTCREATE_H
#include "./SYSTEM/sys/sys.h"
#include "GUI.h"

extern GUI_FONT XBF12_Font;
extern GUI_FONT XBF16_Font;
extern GUI_FONT XBF24_Font;
extern GUI_FONT XBF36_Font;

uint8_t Create_XBF12(uint8_t *fxpath); 
uint8_t Create_XBF16(uint8_t *fxpath); 
uint8_t Create_XBF24(uint8_t *fxpath); 
uint8_t Create_XBF36(uint8_t *fxpath);

#endif
