/**
 ****************************************************************************************************
 * @file        usb_app.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-06-01
 * @brief       USB-APP 代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 STM32开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.0 20220601
 * 第一次发布
 *
 ****************************************************************************************************
 */

#ifndef __USB_APP_H
#define __USB_APP_H

#include "./SYSTEM/sys/sys.h"
#include "usbh_core.h"
#include "usbh_msc.h"
#include "usbh_hid.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_msc.h"
#include "usbd_storage.h"
#include "usbh_hid.h"
#include "usbh_msc.h"
#include "string.h"


#define USBH_MSC_MODE       0       /* USB HOST MSC模式 */
#define USBH_HID_MODE       1       /* USB HOST HID模式 */
#define USBD_MSC_MODE       2       /* USB DEVICE MSC模式 */

/* USB APP控制结构体 */
typedef struct
{
    uint8_t bDeviceState;   /**
                             * USB状态标记
                             * bit0:表示电脑正在向SD卡写入数据
                             * bit1:表示电脑正从SD卡读出数据
                             * bit2:SD卡写数据错误标志位
                             * bit3:SD卡读数据错误标志位
                             * bit4:1,表示电脑有轮询操作(表明连接还保持着)
                             * bit5:保留.
                             * bit6:1,表示USB有设备插入;0,表示没有设备插入
                             * bit7:1,表示USB已经连接;0,表示USB没有连接.
                             */
    
    uint8_t hdevclass;      /**
                             * USB HOST设备类型
                             * 1,U盘
                             * 2,鼠标
                             * 3,键盘
                             * 4,游戏手柄
                             */
    
    uint8_t mode;           /**
                             * USB工作模式:0,USB HOST MSC模式(默认模式,接U盘)
                             * 1,USB HOST HID模式(驱动鼠标键盘等)
                             * 2,USB Device MSC模式(USB读卡器)
                             */
} _usb_app;
extern _usb_app usbx;


extern USBH_HandleTypeDef  hUSBHost;    /* USB Host处理结构体 */
extern USBD_HandleTypeDef  hUSBDevice;  /* USB Device处理结构体 */

extern HCD_HandleTypeDef hhcd;          /* HCD句柄,在usbh_conf.c里面定义 */
extern PCD_HandleTypeDef hpcd;          /* PCD句柄,在usbd_conf.c里面定义 */
extern USBH_StatusTypeDef USBH_MSC_InterfaceDeInit  (USBH_HandleTypeDef *phost);    /* 在usbh_msc.c里面定义 */
extern USBH_StatusTypeDef USBH_HID_InterfaceDeInit (USBH_HandleTypeDef *phost );    /* 在usbh_hid.c里面定义 */
extern uint8_t  USBD_MSC_DeInit (USBD_HandleTypeDef *pdev, uint8_t cfgidx);         /* 在usbd_msc.c里面定义 */



void usbapp_init(void);     /* 初始化USB */
void usbapp_pulling(void);
void usbapp_mode_stop(void);
void usbapp_mode_set(uint8_t mode);

#endif

















