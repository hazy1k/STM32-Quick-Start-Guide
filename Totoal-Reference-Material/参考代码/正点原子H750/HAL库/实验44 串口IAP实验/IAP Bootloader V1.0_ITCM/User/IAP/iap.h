/**
 ****************************************************************************************************
 * @file        iap.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-05
 * @brief       IAP 代码
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

#ifndef __IAP_H
#define __IAP_H

#include "./SYSTEM/sys/sys.h"



typedef void (*iapfun)(void);                   /* 定义一个函数类型的参数 */
#define FLASH_APP1_ADDR         0x08004000      /* 第一个应用程序起始地址(存放在内部FLASH),保留0X08000000~0X08003FFF的空间为Bootloader使用(共16KB) */
#define QSPI_APP1_ADDR          0X90100000      /* 第一个应用程序QSPI起始地址(存放在QSPI FLASH),保留0X90000000~0X900FFFFF的空间为Bootloader使用(共1MB) */
#define SRAM_APP1_ADDR          0X24001000      /* 第一个S应用程序SRAM起始地址 */


void iap_load_app(uint32_t appxaddr);   /* 跳转到APP程序执行 */
void iap_write_appbin(uint32_t appxaddr,uint8_t *appbuf,uint32_t applen);   /* 在指定地址开始,写入bin */

#endif














