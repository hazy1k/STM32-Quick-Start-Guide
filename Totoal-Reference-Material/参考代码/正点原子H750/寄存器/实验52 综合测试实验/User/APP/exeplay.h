/**
 ****************************************************************************************************
 * @file        exeplay.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.1
 * @date        2022-05-26
 * @brief       APP-运行器 代码
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
 * V1.1 20220526
 * 1, 修改注释方式
 * 2, 修改u8/u16/u32为uint8_t/uint16_t/uint32_t
 ****************************************************************************************************
 */

#ifndef __EXEPLAY_H
#define __EXEPLAY_H

#include "common.h"


/** 
 * APP相关信息设置
 * 思路:把BIN文件先存放到外部QSPI FLASH，地址从flash内部的：500*4096开始，最多占用450KB
 * 存放好BIN以后，设置标志位（后背寄存器,BK1），然后产生一次软复位，软复位之后,系统判断标
 * 志位，如果需要运行app，则先把标志位清空，然后复制外部QSPI FLASH的APP代码到内部sram,最
 * 后跳转到app的起始地址,开始运行app代码.
 * 注意:
 * 1,默认设置APP的尺寸最大为EXEPLAY_APP_SIZE字节.
 * 2,APP_SIZE必须小于EXEPLAY_APP_SIZE。
 */

#define EXEPLAY_APP_SIZE    448*1024                /* app代码的最大尺寸.这里为448K字节 */
#define EXEPLAY_APP_BASE    0x24001000              /* app执行代码的目的地址,也就是将要运行的代码存放的地址 */
#define	EXEPLAY_SRC_BASE    0X90000000+500*4096     /* app执行代码的源地址,也就是软复位之前,app代码存放的地址 */
                                                    /* 存储在SPILCD所占用的区域，如果是4.3的屏幕，则需重启的 */
                                                    /* 时候需要更新SPB背景，如果是2.8和3.5寸屏，则不用. */

typedef  void (*dummyfun)(void);    /* 定义一个函数类型 */
extern dummyfun jump2app;           /* 假函数,让PC指针跑到新的main函数去 */
void exeplay_write_appmask(uint16_t val);
void exeplay_app_check(void);
uint8_t exe_play(void);

#endif
































