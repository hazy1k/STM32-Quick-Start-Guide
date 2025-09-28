#ifndef __USB_APP_H
#define __USB_APP_H	 
#include "sys.h"
#include "usbd_msc_core.h"
#include "usbh_hid_gamepad.h"
#include "usbd_usr.h"
#include "usbd_desc.h"
#include "usb_conf.h"
#include "usbh_usr.h" 
#include "usb_hcd_int.h"
#include "usb_dcd_int.h"
#include "string.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//USB-APP 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/7/20
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//*******************************************************************************
//修改信息
//无
////////////////////////////////////////////////////////////////////////////////// 	   


#define USBH_MSC_MODE	0		//USB HOST MSC模式
#define USBH_HID_MODE	1		//USB HOST HID模式
#define USBD_MSC_MODE	2		//USB DEVICE MSC模式

//USB APP控制结构体
typedef struct 
{
	u8 bDeviceState; 	//USB状态标记
						//bit0:表示电脑正在向SD卡写入数据
						//bit1:表示电脑正从SD卡读出数据
						//bit2:SD卡写数据错误标志位
						//bit3:SD卡读数据错误标志位
						//bit4:1,表示电脑有轮询操作(表明连接还保持着)
						//bit5:保留.
						//bit6:1,表示USB有设备插入;0,表示没有设备插入
						//bit7:1,表示USB已经连接;0,表示USB没有连接.
	u8 hdevclass; 		//USB HOST设备类型
						//1,U盘
						//2,鼠标
						//3,键盘
						//4,游戏手柄
	u8 mode; 			//USB工作模式:0,USB HOST MSC模式(默认模式,接U盘)
						//			  1,USB HOST HID模式(驱动鼠标键盘等)
						//			  2,USB Device MSC模式(USB读卡器)
}_usb_app;
extern _usb_app usbx;
extern USBH_HOST  USB_Host; 
extern USB_OTG_CORE_HANDLE  USB_OTG_Core_dev;

extern uint8_t *USBH_DataInBuffer;
extern uint8_t *USBH_DataOutBuffer;
extern uint8_t *USBH_CfgDesc; 
extern uint8_t *MSC_BOT_Data;	
	
void usbapp_init(void);			//初始化USB
void usbapp_pulling(void);
void usbapp_mode_stop(void);
void usbapp_mode_set(u8 mode);

#endif

















