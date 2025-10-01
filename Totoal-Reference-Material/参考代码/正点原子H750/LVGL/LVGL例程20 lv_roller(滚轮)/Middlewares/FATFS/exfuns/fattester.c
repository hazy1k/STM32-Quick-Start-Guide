/**
 ****************************************************************************************************
 * @file        fattester.c
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

#include "string.h"
#include "./MALLOC/malloc.h"
#include "./SYSTEM/usart/usart.h"
#include "./FATFS/exfuns/exfuns.h"
#include "./FATFS/exfuns/fattester.h"
#include "./BSP/SDMMC/sdmmc_sdcard.h"

/* 如果支持文件系统测试 ,则使能以下代码 */
#if USE_FATTESTER == 1

/* FATFS测试结构体
 * 主要包含了文件指针/文件信息/目录/读写缓存等信息, 方便fattester.c
 * 里面的测试函数使用. 当不需要使用文件系统测试功能时
 */
_m_fattester fattester;



/**
 * @brief       初始化文件系统测试(申请内存)
 *   @note      该函数必须在执行任何文件系统测试之前被调用一次.
 *              该函数只需要被成功调用一次即可,无需重复调用!!
 * @param       无
 * @retval      执行结果: 0, 成功; 1, 失败;
 */
uint8_t mf_init(void)
{
    fattester.file = (FIL *)mymalloc(SRAMIN, sizeof(FIL));      /* 为file申请内存 */
    fattester.fatbuf = (uint8_t *)mymalloc(SRAMIN, 512);        /* 为fattester.fatbuf申请内存 */

    if (fattester.file && fattester.fatbuf)
    {
        return 0;   /* 申请成功 */
    }
    else
    {
        mf_free();  /* 释放内存 */
        return 1;   /* 申请失败 */
    }
}

/**
 * @brief       释放文件系统测试申请的内存
 *   @note      调用完该函数以后, 文件系统测试功能将失效.
 * @param       无
 * @retval      无
 */
void mf_free(void)
{
    myfree(SRAMIN, fattester.file);
    myfree(SRAMIN, fattester.fatbuf);
}

/**
 * @brief       为磁盘注册工作区
 * @param       path : 磁盘路径，比如"0:"、"1:"
 * @param       mt   : 0，不立即注册(稍后注册); 1，立即注册
 * @retval      执行结果(参见FATFS, FRESULT的定义)
 */
uint8_t mf_mount(uint8_t *path, uint8_t mt)
{
    return f_mount(fs[1], (const TCHAR *)path, mt);
}

/**
 * @brief       打开文件
 * @param       path : 路径 + 文件名
 * @param       mode : 打开模式
 * @retval      执行结果(参见FATFS, FRESULT的定义)
 */
uint8_t mf_open(uint8_t *path, uint8_t mode)
{
    uint8_t res;
    res = f_open(fattester.file, (const TCHAR *)path, mode);    /* 打开文件 */
    return res;
}

/**
 * @brief       关闭文件
 * @param       无
 * @retval      执行结果(参见FATFS, FRESULT的定义)
 */
uint8_t mf_close(void)
{
    f_close(fattester.file);
    return 0;
}

/**
 * @brief       读出数据
 * @param       len : 读出的长度
 * @retval      执行结果(参见FATFS, FRESULT的定义)
 */
uint8_t mf_read(uint16_t len)
{
    uint16_t i, t;
    uint8_t res = 0;
    uint16_t tlen = 0;
    uint32_t br = 0;
    printf("\r\nRead fattester.file data is:\r\n");

    for (i = 0; i < len / 512; i++)
    {
        res = f_read(fattester.file, fattester.fatbuf, 512, &br);

        if (res)
        {
            printf("Read Error:%d\r\n", res);
            break;
        }
        else
        {
            tlen += br;

            for (t = 0; t < br; t++)printf("%c", fattester.fatbuf[t]);
        }
    }

    if (len % 512)
    {
        res = f_read(fattester.file, fattester.fatbuf, len % 512, &br);

        if (res)    /* 读数据出错了 */
        {
            printf("\r\nRead Error:%d\r\n", res);
        }
        else
        {
            tlen += br;

            for (t = 0; t < br; t++)printf("%c", fattester.fatbuf[t]);
        }
    }

    if (tlen)printf("\r\nReaded data len:%d\r\n", tlen);    /* 读到的数据长度 */

    printf("Read data over\r\n");
    return res;
}

/**
 * @brief       写入数据
 * @param       pdata : 数据缓存区
 * @param       len   : 写入长度
 * @retval      执行结果(参见FATFS, FRESULT的定义)
 */
uint8_t mf_write(uint8_t *pdata, uint16_t len)
{
    uint8_t res;
    uint32_t bw = 0;

    printf("\r\nBegin Write fattester.file...\r\n");
    printf("Write data len:%d\r\n", len);
    res = f_write(fattester.file, pdata, len, &bw);

    if (res)
    {
        printf("Write Error:%d\r\n", res);
    }
    else
    {
        printf("Writed data len:%d\r\n", bw);
    }

    printf("Write data over.\r\n");
    return res;
}

/**
 * @brief       打开目录
 * @param       path : 路径
 * @retval      执行结果(参见FATFS, FRESULT的定义)
 */
uint8_t mf_opendir(uint8_t *path)
{
    return f_opendir(&fattester.dir, (const TCHAR *)path);
}

/**
 * @brief       关闭目录
 * @param       无
 * @retval      执行结果(参见FATFS, FRESULT的定义)
 */
uint8_t mf_closedir(void)
{
    return f_closedir(&fattester.dir);
}

/**
 * @brief       打读取文件夹
 * @param       无
 * @retval      执行结果(参见FATFS, FRESULT的定义)
 */
uint8_t mf_readdir(void)
{
    uint8_t res;
    res = f_readdir(&fattester.dir, &fattester.fileinfo);   /* 读取一个文件的信息 */

    if (res != FR_OK)return res;    /* 出错了 */

    printf("\r\n fattester.dir info:\r\n");

    printf("fattester.dir.dptr:%d\r\n", fattester.dir.dptr);
    printf("fattester.dir.obj.id:%d\r\n", fattester.dir.obj.id);
    printf("fattester.dir.obj.sclust:%d\r\n", fattester.dir.obj.sclust);
    printf("fattester.dir.obj.objsize:%lld\r\n", fattester.dir.obj.objsize);
    printf("fattester.dir.obj.c_ofs:%d\r\n", fattester.dir.obj.c_ofs);
    printf("fattester.dir.clust:%d\r\n", fattester.dir.clust);
    printf("fattester.dir.sect:%d\r\n", fattester.dir.sect);
    printf("fattester.dir.blk_ofs:%d\r\n", fattester.dir.blk_ofs);

    printf("\r\n");
    printf("fattester.file Name is:%s\r\n", fattester.fileinfo.fname);
    printf("fattester.file Size is:%lld\r\n", fattester.fileinfo.fsize);
    printf("fattester.file data is:%d\r\n", fattester.fileinfo.fdate);
    printf("fattester.file time is:%d\r\n", fattester.fileinfo.ftime);
    printf("fattester.file Attr is:%d\r\n", fattester.fileinfo.fattrib);
    printf("\r\n");
    return 0;
}

/**
 * @brief       遍历文件
 * @param       path : 路径
 * @retval      执行结果(参见FATFS, FRESULT的定义)
 */
uint8_t mf_scan_files(uint8_t *path)
{
    FRESULT res;
    res = f_opendir(&fattester.dir, (const TCHAR *)path); /* 打开一个目录 */

    if (res == FR_OK)
    {
        printf("\r\n");

        while (1)
        {
            res = f_readdir(&fattester.dir, &fattester.fileinfo);   /* 读取目录下的一个文件 */

            if (res != FR_OK || fattester.fileinfo.fname[0] == 0)
            {
                break;  /* 错误了/到末尾了,退出 */
            }
            
            // if (fattester.fileinfo.fname[0] == '.') continue;    /* 忽略上级目录 */
            printf("%s/", path);    /* 打印路径 */
            printf("%s\r\n", fattester.fileinfo.fname); /* 打印文件名 */
        }
    }

    return res;
}

/**
 * @brief       显示剩余容量
 * @param       path : 路径(盘符)
 * @retval      剩余容量(字节)
 */
uint32_t mf_showfree(uint8_t *path)
{
    FATFS *fs1;
    uint8_t res;
    uint32_t fre_clust = 0, fre_sect = 0, tot_sect = 0;
    /* 得到磁盘信息及空闲簇数量 */
    res = f_getfree((const TCHAR *)path, (DWORD *)&fre_clust, &fs1);

    if (res == 0)
    {
        tot_sect = (fs1->n_fatent - 2) * fs1->csize;/* 得到总扇区数 */
        fre_sect = fre_clust * fs1->csize;          /* 得到空闲扇区数 */
#if FF_MAX_SS!=512
        tot_sect *= fs1->ssize / 512;
        fre_sect *= fs1->ssize / 512;
#endif

        if (tot_sect < 20480)   /* 总容量小于10M */
        {
            /* Print free space in unit of KB (assuming 512 bytes/sector) */
            printf("\r\n磁盘总容量:%d KB\r\n"
                   "可用空间:%d KB\r\n",
                   tot_sect >> 1, fre_sect >> 1);
        }
        else
        {
            /* Print free space in unit of KB (assuming 512 bytes/sector) */
            printf("\r\n磁盘总容量:%d MB\r\n"
                   "可用空间:%d MB\r\n",
                   tot_sect >> 11, fre_sect >> 11);
        }
    }

    return fre_sect;
}

/**
 * @brief       文件读写指针偏移
 * @param       offset : 相对首地址的偏移量
 * @retval      执行结果(参见FATFS, FRESULT的定义)
 */
uint8_t mf_lseek(uint32_t offset)
{
    return f_lseek(fattester.file, offset);
}

/**
 * @brief       读取文件当前读写指针的位置
 * @param       无
 * @retval      当前位置
 */
uint32_t mf_tell(void)
{
    return f_tell(fattester.file);
}

/**
 * @brief       读取文件大小
 * @param       无
 * @retval      文件大小
 */
uint32_t mf_size(void)
{
    return f_size(fattester.file);
}

/**
 * @brief       创建目录
 * @param       path : 目录路径 + 名字
 * @retval      执行结果(参见FATFS, FRESULT的定义)
 */
uint8_t mf_mkdir(uint8_t *path)
{
    return f_mkdir((const TCHAR *)path);
}

/**
 * @brief       格式化
 * @param       path : 磁盘路径，比如"0:"、"1:"
 * @param       opt  : 模式,FM_FAT,FM_FAT32,FM_EXFAT,FM_ANY等...
 * @param       au   : 簇大小
 * @retval      执行结果(参见FATFS, FRESULT的定义)
 */
uint8_t mf_fmkfs(uint8_t *path, uint8_t opt, uint16_t au)
{
    MKFS_PARM temp = {FM_ANY, 0, 0, 0, 0};
    temp.fmt = opt;     /* 文件系统格式,1：FM_FAT;2,FM_FAT32;4,FM_EXFAT; */
    temp.au_size = au;  /* 簇大小定义,0则使用默认簇大小 */
    return f_mkfs((const TCHAR *)path, &temp, 0, FF_MAX_SS);    /* 格式化,默认参数,workbuf,最少_MAX_SS大小 */
}

/**
 * @brief       删除文件/目录
 * @param       path : 文件/目录路径+名字
 * @retval      执行结果(参见FATFS, FRESULT的定义)
 */
uint8_t mf_unlink(uint8_t *path)
{
    return  f_unlink((const TCHAR *)path);
}

/**
 * @brief       修改文件/目录名字(如果目录不同,还可以移动文件哦!)
 * @param       oldname : 之前的名字
 * @param       newname : 新名字
 * @retval      执行结果(参见FATFS, FRESULT的定义)
 */
uint8_t mf_rename(uint8_t *oldname, uint8_t *newname)
{
    return  f_rename((const TCHAR *)oldname, (const TCHAR *)newname);
}

/**
 * @brief       获取盘符(磁盘名字)
 * @param       path : 磁盘路径，比如"0:"、"1:"
 * @retval      执行结果(参见FATFS, FRESULT的定义)
 */
void mf_getlabel(uint8_t *path)
{
    uint8_t buf[20];
    uint32_t sn = 0;
    uint8_t res;
    res = f_getlabel ((const TCHAR *)path, (TCHAR *)buf, (DWORD *)&sn);

    if (res == FR_OK)
    {
        printf("\r\n磁盘%s 的盘符为:%s\r\n", path, buf);
        printf("磁盘%s 的序列号:%X\r\n\r\n", path, sn);
    }
    else
    {
        printf("\r\n获取失败，错误码:%X\r\n", res);
    }
}

/**
 * @brief       设置盘符（磁盘名字），最长11个字符！！，支持数字和大写字母组合以及汉字等
 * @param       path : 磁盘号+名字，比如"0:ALIENTEK"、"1:OPENEDV"
 * @retval      执行结果(参见FATFS, FRESULT的定义)
 */
void mf_setlabel(uint8_t *path)
{
    uint8_t res;
    res = f_setlabel ((const TCHAR *)path);

    if (res == FR_OK)
    {
        printf("\r\n磁盘盘符设置成功:%s\r\n", path);
    }
    else printf("\r\n磁盘盘符设置失败，错误码:%X\r\n", res);
}

/**
 * @brief       从文件里面读取一段字符串
 * @param       size : 要读取的长度
 * @retval      执行结果(参见FATFS, FRESULT的定义)
 */
void mf_gets(uint16_t size)
{
    TCHAR *rbuf;
    rbuf = f_gets((TCHAR *)fattester.fatbuf, size, fattester.file);

    if (*rbuf == 0)return  ; /* 没有数据读到 */
    else
    {
        printf("\r\nThe String Readed Is:%s\r\n", rbuf);
    }
}

/**
 * @brief       写一个字符到文件(需要 FF_USE_STRFUNC >= 1)
 * @param       c : 要写入的字符
 * @retval      执行结果(参见FATFS, FRESULT的定义)
 */
uint8_t mf_putc(uint8_t c)
{
    return f_putc((TCHAR)c, fattester.file);
}

/**
 * @brief       写字符串到文件(需要 FF_USE_STRFUNC >= 1)
 * @param       str : 要写入的字符串
 * @retval      执行结果(参见FATFS, FRESULT的定义)
 */
uint8_t mf_puts(uint8_t *str)
{
    return f_puts((TCHAR *)str, fattester.file);
}

#endif












