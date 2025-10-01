/**
 ****************************************************************************************************
 * @file        fattester.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-03
 * @brief       FATFS 测试代码
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
 * V1.0 20200403
 * 第一次发布
 *
 ****************************************************************************************************
 */

#ifndef __FATTESTER_H
#define __FATTESTER_H

#include "./SYSTEM/sys/sys.h"
#include "./FATFS/source/ff.h"


/* 定义是否支持文件系统测试功能
 * 1, 支持(开启);
 * 0, 不支持(关闭);
 */
#define USE_FATTESTER         1




/* 如果支持文件系统测试 ,则使能以下代码 */
#if USE_FATTESTER == 1
 
/* FATFS 测试用结构体 */
typedef struct
{
    FIL *file;          /* 文件结构体指针1 */
    FILINFO fileinfo;   /* 文件信息 */
    DIR dir;            /* 目录 */
    uint8_t *fatbuf;    /* 读写缓存 */
    uint8_t initflag;   /* 初始化标志 */
} _m_fattester;

extern _m_fattester fattester;      /* FATFS测试结构体 */


uint8_t mf_init(void);              /* 初始化文件系统测试 */
void mf_free(void);                 /* 释放文件系统测试时申请的内存 */


uint8_t mf_mount(uint8_t* path,uint8_t mt);
uint8_t mf_open(uint8_t*path,uint8_t mode);
uint8_t mf_close(void);
uint8_t mf_read(uint16_t len);
uint8_t mf_write(uint8_t*dat,uint16_t len);
uint8_t mf_opendir(uint8_t* path);
uint8_t mf_closedir(void);
uint8_t mf_readdir(void);
uint8_t mf_scan_files(uint8_t * path);
uint32_t mf_showfree(uint8_t *drv);
uint8_t mf_lseek(uint32_t offset);
uint32_t mf_tell(void);
uint32_t mf_size(void);
uint8_t mf_mkdir(uint8_t*pname);
uint8_t mf_fmkfs(uint8_t* path,uint8_t opt,uint16_t au);
uint8_t mf_unlink(uint8_t *pname);
uint8_t mf_rename(uint8_t *oldname,uint8_t* newname);
void mf_getlabel(uint8_t *path);
void mf_setlabel(uint8_t *path); 
void mf_gets(uint16_t size);
uint8_t mf_putc(uint8_t c);
uint8_t mf_puts(uint8_t*c);

#endif

 
#endif





























