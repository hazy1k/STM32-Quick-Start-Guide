/**
 ****************************************************************************************************
 * @file        usbh_diskio.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-06
 * @brief       usbh diskio 驱动代码
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

#ifndef __USBH_DISKIO_H
#define __USBH_DISKIO_H

#include "usbh_core.h"
#include "usbh_msc.h"
#include "./FATFS/source/diskio.h"



#define USB_DEFAULT_BLOCK_SIZE      512


DSTATUS USBH_initialize(void);
DSTATUS USBH_status(void);
DRESULT USBH_read(BYTE *buff, DWORD sector, UINT count);
DRESULT USBH_write(const BYTE *buff,DWORD sector,UINT count);
DRESULT USBH_ioctl(BYTE cmd,void *buff);

#endif 
