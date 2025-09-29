/**
 ****************************************************************************************************
 * @file        usbh_hid_mouse.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-06
 * @brief       USB鼠标 驱动代码
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
 * V1.0 20200406
 * 第一次发布
 *
 ****************************************************************************************************
 */

#ifndef __USBH_HID_MOUSE_H
#define __USBH_HID_MOUSE_H

#include "usbh_hid.h"


/* 鼠标信息结构体*/
typedef struct _HID_MOUSE_Info
{
  uint8_t              x;           /* x轴增量（强制转换成signed char后使用）*/
  uint8_t              y;           /* y轴增量（强制转换成signed char后使用）*/
  uint8_t              z;           /* z轴增量（强制转换成signed char后使用）*/
  uint8_t              button;      /* 将buttons修改为button,存储按键状态*/
}
HID_MOUSE_Info_TypeDef;



USBH_StatusTypeDef USBH_HID_MouseInit(USBH_HandleTypeDef *phost);
HID_MOUSE_Info_TypeDef *USBH_HID_GetMouseInfo(USBH_HandleTypeDef *phost);
USBH_StatusTypeDef USBH_HID_MouseDecode(USBH_HandleTypeDef *phost);

#endif














