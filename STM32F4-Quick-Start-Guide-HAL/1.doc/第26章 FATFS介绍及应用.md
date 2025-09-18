# 第二十六章 FATFS介绍及应用

## 1. FATFS简介

FATFS 是一个完全免费开源的 FAT/exFAT 文件系统模块，专门为小型的嵌入式系统而设计。它完全用标准 C 语言（ANSI C C89） 编写，所以具有良好的硬件平台独立性，只需做简单的修改就可以移植到 8051、 PIC、 AVR、 ARM、 Z80、 RX 等系列单片机上。它支持 FATl2、 FATl6 和FAT32，支持多个存储媒介；有独立的缓冲区，可以对多个文件进行读／写，并特别对 8 位单片机和 16 位单片机做了优化。

最顶层是应用层，使用者无需理会FATFS的内部结构和复杂的FAT协议，只需要调用FATFS模块提供给用户的一系列应用接口函数，如 f_open， f_read， f_write 和 f_close 等，就可以像在PC 上读／写文件那样简单。

中间层 FATFS 模块，实现了 FAT 文件读／写协议。 FATFS 模块提供的是 ff.c 和 ff.h。除非有必要，使用者一般不用修改，使用时将头文件直接包含进去即可。

需要我们编写移植代码的是 FATFS 模块提供的底层接口，它包括存储媒介读／写接口（diskI/O）和供给文件创建修改时间的实时时钟。

![屏幕截图 2025-08-15 220052.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/15-22-00-58-屏幕截图%202025-08-15%20220052.png)

## 2. FATFS移植

### 2.1 可选配置项

FATFS 模块在移植的时候，我们一般只需要修改 2 个文件，即 ffconf.h 和 diskio.c。 FATFS模块的所有配置项都是存放在 ffconf.h 里面，我们可以通过配置里面的一些选项，来满足自己的需求。接下来我们介绍几个重要的配置选项。

1. FF_FS_TINY。这个选项在 R0.07 版本中开始出现，之前的版本都是以独立的 C 文件出现（FATFS 和 TinyFATFS），有了这个选项之后，两者整合在一起了，使用起来更方便。我们使用 FATFS，所以把这个选项定义为 0 即可。

2. FF _FS_READONLY。这个用来配置是不是只读，本章我们需要读写都用，所以这里设置为 0 即可。

3. FF _USE_STRFUNC。这个用来设置是否支持字符串类操作，比如 f_putc， f_puts 等，本章我们需要用到，故设置这里为 1。

4. FF _USE_MKFS。用来定时是否使能格式化，本章需要用到，所以设置这里为 1。

5. FF _USE_FASTSEEK。这个用来使能快速定位，我们设置为 1，使能快速定位。

6. FF _USE_LABEL。这个用来设置是否支持磁盘盘符（磁盘名字）读取与设置。我们设置为 1，使能，就可以通过相关函数读取或者设置磁盘的名字了。

7. FF _CODE_PAGE。这个用于设置语言类型，包括很多选项（见 FATFS 官网说明），我们这里设置为 936，即简体中文（GBK 码， 同一个文件夹下的 ffunicode.c 根据这个宏选择对应的语言设置）。

8. FF_USE_LFN。该选项用于设置是否支持长文件名（还需要_CODE_PAGE 支持），取值范围为 0~3。 0，表示不支持长文件名， 1~3 是支持长文件名，但是存储地方不一样，我们选择使用 3，通过 ff_memalloc 函数来动态分配长文件名的存储区域。

9. FF_VOLUMES。用于设置 FATFS 支持的逻辑设备数目，我们设置为 2，即支持 2 个设备。

10. FF_MAX_SS。扇区缓冲的最大值，一般设置为 512。11） FF_FS_EXFAT。 新版本增加的功能， 使用 exFAT 文件系统，用于支持超过 32Gb 的超大存储。 它们使用的是 exFAT 文件系统，使用它时必须要根据设置 FF_USE_LFN 这个参数的值以决定 exFATs 系统使用的内存来自堆栈还是静态数组。

### 2.2 移植步骤

1. 数据类型：在 integer.h 里面去定义好数据的类型。这里需要了解你用的编译器的数据类型，并根据编译器定义好数据类型

2. 配置：通过 ffconf.h 配置 FATFS 的相关功能，以满足你的需要。

3. 函数编写：打开 diskio.c，进行底层驱动编写，需要编写 5 个接口函数，如下图所示：

![屏幕截图 2025-08-15 220332.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/15-22-03-36-屏幕截图%202025-08-15%20220332.png)

首先是 disk_initialize 函数：

![屏幕截图 2025-08-15 220820.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/15-22-08-23-屏幕截图%202025-08-15%20220820.png)

```c
/**
 * @brief       初始化磁盘
 * @param       pdrv : 磁盘编号0~9
 * @retval      执行结果(参见FATFS, DSTATUS的定义)
 */
DSTATUS disk_initialize (
    BYTE pdrv       /* Physical drive number to identify the drive */
)
{
    uint8_t res = 0;

    switch (pdrv)
    {
        case SD_CARD:           /* SD卡 */
            res = sd_init();    /* SD卡初始化 */
            break;

        case EX_FLASH:          /* 外部flash */
            spi_flash_init(); 
            break;

        default:
            res = 1;
            break;
    }

    if (res)
    {
        return STA_NOINIT;
    }
    else
    {
        return 0; /* 初始化成功*/
    }
}
```

第二个函数是 disk_status 函数：

![屏幕截图 2025-08-15 220917.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/15-22-09-25-屏幕截图%202025-08-15%20220917.png)

![屏幕截图 2025-08-15 220940.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/15-22-09-45-屏幕截图%202025-08-15%20220940.png)

```c
/**
 * @brief       获得磁盘状态
 * @param       pdrv : 磁盘编号0~9
 * @retval      执行结果(参见FATFS, DSTATUS的定义)
 */
DSTATUS disk_status (
    BYTE pdrv       /* Physical drive number to identify the drive */
)
{
    return RES_OK;
}
```

第三个函数是 disk_read 函数：

![屏幕截图 2025-08-15 221019.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/15-22-10-23-屏幕截图%202025-08-15%20221019.png)

```c
/**
 * @brief       读扇区
 * @param       pdrv   : 磁盘编号0~9
 * @param       buff   : 数据接收缓冲首地址
 * @param       sector : 扇区地址
 * @param       count  : 需要读取的扇区数
 * @retval      执行结果(参见FATFS, DRESULT的定义)
 */
DRESULT disk_read (
    BYTE pdrv,      /* Physical drive number to identify the drive */
    BYTE *buff,     /* Data buffer to store read data */
    DWORD sector,   /* Sector address in LBA */
    UINT count      /* Number of sectors to read */
)
{
    uint8_t res = 0;

    if (!count) return RES_PARERR;   /* count不能等于0，否则返回参数错误 */

    switch (pdrv)
    {
        case SD_CARD:       /* SD卡 */
            res = sd_read_disk(buff, sector, count);

            while (res)     /* 读出错 */
            {
                //printf("sd rd error:%d\r\n", res);
                sd_init();  /* 重新初始化SD卡 */
                res = sd_read_disk(buff, sector, count);
            }

            break;

        case EX_FLASH:      /* 外部flash */
            for (; count > 0; count--)
            {
                flash_read(buff, SPI_FLASH_FATFS_BASE + sector * SPI_FLASH_SECTOR_SIZE, SPI_FLASH_SECTOR_SIZE);
                sector++;
                buff += SPI_FLASH_SECTOR_SIZE;
            }

            res = 0;
            break;

        default:
            res = 1;
    }

    /* 处理返回值，将返回值转成ff.c的返回值 */
    if (res == 0x00)
    {
        return RES_OK;
    }
    else
    {
        return RES_ERROR; 
    }
}
```

第四个函数是 disk_write 函数：

![屏幕截图 2025-08-15 221137.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/15-22-11-40-屏幕截图%202025-08-15%20221137.png)

```c
/**
 * @brief       写扇区
 * @param       pdrv   : 磁盘编号0~9
 * @param       buff   : 发送数据缓存区首地址
 * @param       sector : 扇区地址
 * @param       count  : 需要写入的扇区数
 * @retval      执行结果(参见FATFS, DRESULT的定义)
 */
DRESULT disk_write (
    BYTE pdrv,          /* Physical drive number to identify the drive */
    const BYTE *buff,   /* Data to be written */
    DWORD sector,       /* Sector address in LBA */
    UINT count          /* Number of sectors to write */
)
{
    uint8_t res = 0;

    if (!count) return RES_PARERR;  /* count不能等于0，否则返回参数错误 */

    switch (pdrv)
    {
        case SD_CARD:       /* SD卡 */
            res = sd_write_disk((uint8_t *)buff, sector, count);

            while (res)     /* 写出错 */
            {
                //printf("sd wr error:%d\r\n", res);
                sd_init();  /* 重新初始化SD卡 */
                res = sd_write_disk((uint8_t *)buff, sector, count);
            }

            break;

        case EX_FLASH:      /* 外部flash */
            for (; count > 0; count--)
            {
                flash_write((uint8_t *)buff, SPI_FLASH_FATFS_BASE + sector * SPI_FLASH_SECTOR_SIZE, SPI_FLASH_SECTOR_SIZE);
                sector++;
                buff += SPI_FLASH_SECTOR_SIZE;
            }

            res = 0;
            break;

        default:
            res = 1;
    }

    /* 处理返回值，将返回值转成ff.c的返回值 */
    if (res == 0x00)
    {
        return RES_OK;
    }
    else
    {
        return RES_ERROR; 
    }
}

```

第五个函数是 disk_ioctl 函数：

![屏幕截图 2025-08-15 221219.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/15-22-12-36-屏幕截图%202025-08-15%20221219.png)

![屏幕截图 2025-08-15 221229.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/15-22-12-39-屏幕截图%202025-08-15%20221229.png)

```c
/**
 * @brief       获取其他控制参数
 * @param       pdrv   : 磁盘编号0~9
 * @param       ctrl   : 控制代码
 * @param       buff   : 发送/接收缓冲区指针
 * @retval      执行结果(参见FATFS, DRESULT的定义)
 */
DRESULT disk_ioctl (
    BYTE pdrv,      /* Physical drive number (0..) */
    BYTE cmd,       /* Control code */
    void *buff      /* Buffer to send/receive control data */
)
{
    DRESULT res;

    if (pdrv == SD_CARD)    /* SD卡 */
    {
        switch (cmd)
        {
            case CTRL_SYNC:
                res = RES_OK;
                break;

            case GET_SECTOR_SIZE:
                *(DWORD *)buff = 512;
                res = RES_OK;
                break;

            case GET_BLOCK_SIZE:
                *(WORD *)buff = sdcard_info_handle.LogBlockSize;
                res = RES_OK;
                break;

            case GET_SECTOR_COUNT:
                *(DWORD *)buff = sdcard_info_handle.LogBlockNbr;
                res = RES_OK;
                break;

            default:
                res = RES_PARERR;
                break;
        }
    }
    else if (pdrv == EX_FLASH)  /* 外部FLASH */
    {
        switch (cmd)
        {
            case CTRL_SYNC:
                res = RES_OK;
                break;

            case GET_SECTOR_SIZE:
                *(WORD *)buff = SPI_FLASH_SECTOR_SIZE;
                res = RES_OK;
                break;

            case GET_BLOCK_SIZE:
                *(WORD *)buff = SPI_FLASH_BLOCK_SIZE;
                res = RES_OK;
                break;

            case GET_SECTOR_COUNT:
                *(DWORD *)buff = SPI_FLASH_SECTOR_COUNT;
                res = RES_OK;
                break;

            default:
                res = RES_PARERR;
                break;
        }
    }
    else
    {
        res = RES_ERROR;    /* 其他的不支持 */
    }
    
    return res;
}
```

以上五个函数，我们将在软件设计部分一一实现。通过以上 3 个步骤，我们就完成了对FATFS 的移植，就可以在我们的代码里面使用 FATFS 了。

## 3. FATFS应用示例

![屏幕截图 2025-08-15 221414.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/15-22-14-17-屏幕截图%202025-08-15%20221414.png)

### 3.1 fattester.h

```c
#ifndef __FATTESTER_H
#define __FATTESTER_H

#include "sys.h"
#include "ff.h"

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


uint8_t mf_mount(uint8_t* path, uint8_t mt);
uint8_t mf_open(uint8_t*path, uint8_t mode);
uint8_t mf_close(void);
uint8_t mf_read(uint16_t len);
uint8_t mf_write(uint8_t*dat, uint16_t len);
uint8_t mf_opendir(uint8_t* path);
uint8_t mf_closedir(void);
uint8_t mf_readdir(void);
uint8_t mf_scan_files(uint8_t * path);
uint32_t mf_showfree(uint8_t *drv);
uint8_t mf_lseek(uint32_t offset);
uint32_t mf_tell(void);
uint32_t mf_size(void);
uint8_t mf_mkdir(uint8_t*pname);
uint8_t mf_fmkfs(uint8_t* path, uint8_t opt, uint16_t au);
uint8_t mf_unlink(uint8_t *pname);
uint8_t mf_rename(uint8_t *oldname, uint8_t* newname);
void mf_getlabel(uint8_t *path);
void mf_setlabel(uint8_t *path); 
void mf_gets(uint16_t size);
uint8_t mf_putc(uint8_t c);
uint8_t mf_puts(uint8_t*c);

#endif
 
#endif

```

### 3.2 fattester.c

```c
#include "string.h"
#include "malloc.h"
#include "usart.h"
#include "exfuns.h"
#include "fattester.h"
#include "sdio.h"

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

```

### 3.3 exfuns.h

```c
#ifndef __EXFUNS_H
#define __EXFUNS_H

#include "sys.h"
#include "ff.h"

extern FATFS *fs[FF_VOLUMES];
extern FIL *file;
extern FIL *ftemp;
extern UINT br, bw;
extern FILINFO fileinfo;
extern DIR dir;
extern uint8_t *fatbuf;     /* SD卡数据缓存区 */



/* exfuns_file_type返回的类型定义
 * 根据表FILE_TYPE_TBL获得.在exfuns.c里面定义
 */
#define T_BIN       0X00    /* bin文件 */
#define T_LRC       0X10    /* lrc文件 */

#define T_NES       0X20    /* nes文件 */
#define T_SMS       0X21    /* sms文件 */

#define T_TEXT      0X30    /* .txt文件 */
#define T_C         0X31    /* .c文件 */
#define T_H         0X32    /* .h文件 */

#define T_WAV       0X40    /* WAV文件 */
#define T_MP3       0X41    /* MP3文件 */
#define T_OGG       0X42    /* OGG文件 */
#define T_FLAC      0X43    /* FLAC文件 */
#define T_AAC       0X44    /* AAC文件 */
#define T_WMA       0X45    /* WMA文件 */
#define T_MID       0X46    /* MID文件 */

#define T_BMP       0X50    /* bmp文件 */
#define T_JPG       0X51    /* jpg文件 */
#define T_JPEG      0X52    /* jpeg文件 */
#define T_GIF       0X53    /* gif文件 */

#define T_AVI       0X60    /* avi文件 */


uint8_t exfuns_init(void);                  /* 申请内存 */
uint8_t exfuns_file_type(char *fname);   /* 识别文件类型 */

uint8_t exfuns_get_free(uint8_t *pdrv, uint32_t *total, uint32_t *free);    /* 得到磁盘总容量和剩余容量 */
uint32_t exfuns_get_folder_size(uint8_t *fdname);   /* 得到文件夹大小 */
uint8_t *exfuns_get_src_dname(uint8_t *dpfn);
uint8_t exfuns_file_copy(uint8_t(*fcpymsg)(uint8_t *pname, uint8_t pct, uint8_t mode), uint8_t *psrc, uint8_t *pdst, uint32_t totsize, uint32_t cpdsize, uint8_t fwmode);       /* 文件复制 */
uint8_t exfuns_folder_copy(uint8_t(*fcpymsg)(uint8_t *pname, uint8_t pct, uint8_t mode), uint8_t *psrc, uint8_t *pdst, uint32_t *totsize, uint32_t *cpdsize, uint8_t fwmode);   /* 文件夹复制 */
#endif


```

### 3.4 exfuns.c

```c
#include "string.h"
#include "malloc.h"
#include "usart.h"
#include "exfuns.h"
#include "fattester.h"

#define FILE_MAX_TYPE_NUM       7       /* 最多FILE_MAX_TYPE_NUM个大类 */
#define FILE_MAX_SUBT_NUM       7       /* 最多FILE_MAX_SUBT_NUM个小类 */

/* 文件类型列表 */
char *const FILE_TYPE_TBL[FILE_MAX_TYPE_NUM][FILE_MAX_SUBT_NUM] =
{
    {"BIN"},            /* BIN文件 */
    {"LRC"},            /* LRC文件 */
    {"NES", "SMS"},     /* NES/SMS文件 */
    {"TXT", "C", "H"},  /* 文本文件 */
    {"WAV", "MP3", "OGG", "FLAC", "AAC", "WMA", "MID"},   /* 支持的音乐文件 */
    {"BMP", "JPG", "JPEG", "GIF"},  /* 图片文件 */
    {"AVI"},            /* 视频文件 */
};
    
/******************************************************************************************/
/* 公共文件区, 使用malloc的时候 */

/* 逻辑磁盘工作区(在调用任何FATFS相关函数之前,必须先给fs申请内存) */
FATFS *fs[FF_VOLUMES];  

/******************************************************************************************/


/**
 * @brief       为exfuns申请内存
 * @param       无
 * @retval      0, 成功; 1, 失败.
 */
uint8_t exfuns_init(void)
{
    uint8_t i;
    uint8_t res = 0;

    for (i = 0; i < FF_VOLUMES; i++)
    {
        fs[i] = (FATFS *)mymalloc(SRAMIN, sizeof(FATFS));   /* 为磁盘i工作区申请内存 */

        if (!fs[i])break;
    }
    
#if USE_FATTESTER == 1  /* 如果使能了文件系统测试 */
    res = mf_init();    /* 初始化文件系统测试(申请内存) */
#endif
    
    if (i == FF_VOLUMES && res == 0)
    {
        return 0;   /* 申请有一个失败,即失败. */
    }
    else 
    {
        return 1;
    }
}

/**
 * @brief       将小写字母转为大写字母,如果是数字,则保持不变.
 * @param       c : 要转换的字母
 * @retval      转换后的字母,大写
 */
uint8_t exfuns_char_upper(uint8_t c)
{
    if (c < 'A')return c;   /* 数字,保持不变. */

    if (c >= 'a')
    {
        return c - 0x20;    /* 变为大写. */
    }
    else
    {
        return c;           /* 大写,保持不变 */
    }
}

/**
 * @brief       报告文件的类型
 * @param       fname : 文件名
 * @retval      文件类型
 *   @arg       0XFF , 表示无法识别的文件类型编号.
 *   @arg       其他 , 高四位表示所属大类, 低四位表示所属小类.
 */
uint8_t exfuns_file_type(char *fname)
{
    uint8_t tbuf[5];
    char *attr = 0;   /* 后缀名 */
    uint8_t i = 0, j;

    while (i < 250)
    {
        i++;

        if (*fname == '\0')break;   /* 偏移到了最后了. */

        fname++;
    }

    if (i == 250)return 0XFF;   /* 错误的字符串. */

    for (i = 0; i < 5; i++)     /* 得到后缀名 */
    {
        fname--;

        if (*fname == '.')
        {
            fname++;
            attr = fname;
            break;
        }
    }

    if (attr == 0)return 0XFF;

    strcpy((char *)tbuf, (const char *)attr);       /* copy */

    for (i = 0; i < 4; i++)tbuf[i] = exfuns_char_upper(tbuf[i]);    /* 全部变为大写 */

    for (i = 0; i < FILE_MAX_TYPE_NUM; i++)         /* 大类对比 */
    {
        for (j = 0; j < FILE_MAX_SUBT_NUM; j++)     /* 子类对比 */
        {
            if (*FILE_TYPE_TBL[i][j] == 0)break;    /* 此组已经没有可对比的成员了. */

            if (strcmp((const char *)FILE_TYPE_TBL[i][j], (const char *)tbuf) == 0) /* 找到了 */
            {
                return (i << 4) | j;
            }
        }
    }

    return 0XFF;    /* 没找到 */
}

/**
 * @brief       获取磁盘剩余容量
 * @param       pdrv : 磁盘编号("0:"~"9:")
 * @param       total: 总容量 (KB)
 * @param       free : 剩余容量 (KB)
 * @retval      0, 正常; 其他, 错误代码
 */
uint8_t exfuns_get_free(uint8_t *pdrv, uint32_t *total, uint32_t *free)
{
    FATFS *fs1;
    uint8_t res;
    uint32_t fre_clust = 0, fre_sect = 0, tot_sect = 0;
    
    /* 得到磁盘信息及空闲簇数量 */
    res = (uint32_t)f_getfree((const TCHAR *)pdrv, (DWORD *)&fre_clust, &fs1);

    if (res == 0)
    {
        tot_sect = (fs1->n_fatent - 2) * fs1->csize;    /* 得到总扇区数 */
        fre_sect = fre_clust * fs1->csize;              /* 得到空闲扇区数 */
#if FF_MAX_SS!=512  /* 扇区大小不是512字节,则转换为512字节 */
        tot_sect *= fs1->ssize / 512;
        fre_sect *= fs1->ssize / 512;
#endif
        *total = tot_sect >> 1;     /* 单位为KB */
        *free = fre_sect >> 1;      /* 单位为KB */
    }

    return res;
}

/**
 * @brief       文件复制
 *   @note      将psrc文件,copy到pdst.
 *              注意: 文件大小不要超过4GB.
 *
 * @param       fcpymsg : 函数指针, 用于实现拷贝时的信息显示
 *                  pname:文件/文件夹名
 *                  pct:百分比
 *                  mode:
 *                      bit0 : 更新文件名
 *                      bit1 : 更新百分比pct
 *                      bit2 : 更新文件夹
 *                      其他 : 保留
 *                  返回值: 0, 正常; 1, 强制退出;
 *
 * @param       psrc    : 源文件
 * @param       pdst    : 目标文件
 * @param       totsize : 总大小(当totsize为0的时候,表示仅仅为单个文件拷贝)
 * @param       cpdsize : 已复制了的大小.
 * @param       fwmode  : 文件写入模式
 *   @arg       0: 不覆盖原有的文件
 *   @arg       1: 覆盖原有的文件
 *
 * @retval      执行结果
 *   @arg       0   , 正常
 *   @arg       0XFF, 强制退出
 *   @arg       其他, 错误代码
 */
uint8_t exfuns_file_copy(uint8_t(*fcpymsg)(uint8_t *pname, uint8_t pct, uint8_t mode), uint8_t *psrc, uint8_t *pdst, 
                                      uint32_t totsize, uint32_t cpdsize, uint8_t fwmode)
{
    uint8_t res;
    uint16_t br = 0;
    uint16_t bw = 0;
    FIL *fsrc = 0;
    FIL *fdst = 0;
    uint8_t *fbuf = 0;
    uint8_t curpct = 0;
    unsigned long long lcpdsize = cpdsize;
    
    fsrc = (FIL *)mymalloc(SRAMIN, sizeof(FIL));    /* 申请内存 */
    fdst = (FIL *)mymalloc(SRAMIN, sizeof(FIL));
    fbuf = (uint8_t *)mymalloc(SRAMIN, 8192);

    if (fsrc == NULL || fdst == NULL || fbuf == NULL)
    {
        res = 100;  /* 前面的值留给fatfs */
    }
    else
    {
        if (fwmode == 0)
        {
            fwmode = FA_CREATE_NEW;     /* 不覆盖 */
        }
        else 
        {
            fwmode = FA_CREATE_ALWAYS;  /* 覆盖存在的文件 */
        }
        
        res = f_open(fsrc, (const TCHAR *)psrc, FA_READ | FA_OPEN_EXISTING);        /* 打开只读文件 */

        if (res == 0)res = f_open(fdst, (const TCHAR *)pdst, FA_WRITE | fwmode);    /* 第一个打开成功,才开始打开第二个 */

        if (res == 0)           /* 两个都打开成功了 */
        {
            if (totsize == 0)   /* 仅仅是单个文件复制 */
            {
                totsize = fsrc->obj.objsize;
                lcpdsize = 0;
                curpct = 0;
            }
            else
            {
                curpct = (lcpdsize * 100) / totsize;            /* 得到新百分比 */
            }
            
            fcpymsg(psrc, curpct, 0X02);                        /* 更新百分比 */

            while (res == 0)    /* 开始复制 */
            {
                res = f_read(fsrc, fbuf, 8192, (UINT *)&br);    /* 源头读出512字节 */

                if (res || br == 0)break;

                res = f_write(fdst, fbuf, (UINT)br, (UINT *)&bw);/* 写入目的文件 */
                lcpdsize += bw;

                if (curpct != (lcpdsize * 100) / totsize)       /* 是否需要更新百分比 */
                {
                    curpct = (lcpdsize * 100) / totsize;

                    if (fcpymsg(psrc, curpct, 0X02))            /* 更新百分比 */
                    {
                        res = 0XFF;                             /* 强制退出 */
                        break;
                    }
                }

                if (res || bw < br)break;
            }

            f_close(fsrc);
            f_close(fdst);
        }
    }

    myfree(SRAMIN, fsrc); /* 释放内存 */
    myfree(SRAMIN, fdst);
    myfree(SRAMIN, fbuf);
    return res;
}

/**
 * @brief       得到路径下的文件夹
 *   @note      即把路径全部去掉, 只留下文件夹名字.
 * @param       pname : 详细路径 
 * @retval      0   , 路径就是个卷标号.
 *              其他, 文件夹名字首地址
 */
uint8_t *exfuns_get_src_dname(uint8_t *pname)
{
    uint16_t temp = 0;

    while (*pname != 0)
    {
        pname++;
        temp++;
    }

    if (temp < 4)return 0;

    while ((*pname != 0x5c) && (*pname != 0x2f))pname--;    /* 追述到倒数第一个"\"或者"/"处 */

    return ++pname;
}

/**
 * @brief       得到文件夹大小
 *   @note      注意: 文件夹大小不要超过4GB.
 * @param       pname : 详细路径 
 * @retval      0   , 文件夹大小为0, 或者读取过程中发生了错误.
 *              其他, 文件夹大小
 */
uint32_t exfuns_get_folder_size(uint8_t *fdname)
{
#define MAX_PATHNAME_DEPTH  512 + 1     /* 最大目标文件路径+文件名深度 */
    uint8_t res = 0;
    DIR *fddir = 0;         /* 目录 */
    FILINFO *finfo = 0;     /* 文件信息 */
    uint8_t *pathname = 0;  /* 目标文件夹路径+文件名 */
    uint16_t pathlen = 0;   /* 目标路径长度 */
    uint32_t fdsize = 0;

    fddir = (DIR *)mymalloc(SRAMIN, sizeof(DIR));   /* 申请内存 */
    finfo = (FILINFO *)mymalloc(SRAMIN, sizeof(FILINFO));

    if (fddir == NULL || finfo == NULL)res = 100;

    if (res == 0)
    {
        pathname = mymalloc(SRAMIN, MAX_PATHNAME_DEPTH);

        if (pathname == NULL)res = 101;

        if (res == 0)
        {
            pathname[0] = 0;
            strcat((char *)pathname, (const char *)fdname);     /* 复制路径 */
            res = f_opendir(fddir, (const TCHAR *)fdname);      /* 打开源目录 */

            if (res == 0)   /* 打开目录成功 */
            {
                while (res == 0)   /* 开始复制文件夹里面的东东 */
                {
                    res = f_readdir(fddir, finfo);                  /* 读取目录下的一个文件 */

                    if (res != FR_OK || finfo->fname[0] == 0)break; /* 错误了/到末尾了,退出 */

                    if (finfo->fname[0] == '.')continue;            /* 忽略上级目录 */

                    if (finfo->fattrib & 0X10)   /* 是子目录(文件属性,0X20,归档文件;0X10,子目录;) */
                    {
                        pathlen = strlen((const char *)pathname);   /* 得到当前路径的长度 */
                        strcat((char *)pathname, (const char *)"/");/* 加斜杠 */
                        strcat((char *)pathname, (const char *)finfo->fname);   /* 源路径加上子目录名字 */
                        //printf("\r\nsub folder:%s\r\n",pathname);      /* 打印子目录名 */
                        fdsize += exfuns_get_folder_size(pathname);     /* 得到子目录大小,递归调用 */
                        pathname[pathlen] = 0;                          /* 加入结束符 */
                    }
                    else
                    {
                        fdsize += finfo->fsize; /* 非目录,直接加上文件的大小 */
                    }
                }
            }

            myfree(SRAMIN, pathname);
        }
    }

    myfree(SRAMIN, fddir);
    myfree(SRAMIN, finfo);

    if (res)
    {
        return 0;
    }
    else 
    {
        return fdsize;
    }
}

/**
 * @brief       文件夹复制
 *   @note      将psrc文件夹, 拷贝到pdst文件夹.
 *              注意: 文件大小不要超过4GB.
 *
 * @param       fcpymsg : 函数指针, 用于实现拷贝时的信息显示
 *                  pname:文件/文件夹名
 *                  pct:百分比
 *                  mode:
 *                      bit0 : 更新文件名
 *                      bit1 : 更新百分比pct
 *                      bit2 : 更新文件夹
 *                      其他 : 保留
 *                  返回值: 0, 正常; 1, 强制退出;
 *
 * @param       psrc    : 源文件夹
 * @param       pdst    : 目标文件夹
 *   @note      必须形如"X:"/"X:XX"/"X:XX/XX"之类的. 且要确认上一级文件夹存在
 *
 * @param       totsize : 总大小(当totsize为0的时候,表示仅仅为单个文件拷贝)
 * @param       cpdsize : 已复制了的大小.
 * @param       fwmode  : 文件写入模式
 *   @arg       0: 不覆盖原有的文件
 *   @arg       1: 覆盖原有的文件
 *
 * @retval      执行结果
 *   @arg       0   , 正常
 *   @arg       0XFF, 强制退出
 *   @arg       其他, 错误代码
 */
uint8_t exfuns_folder_copy(uint8_t(*fcpymsg)(uint8_t *pname, uint8_t pct, uint8_t mode), uint8_t *psrc, uint8_t *pdst, 
                           uint32_t *totsize, uint32_t *cpdsize, uint8_t fwmode)
{
#define MAX_PATHNAME_DEPTH 512 + 1  /* 最大目标文件路径+文件名深度 */
    uint8_t res = 0;
    DIR *srcdir = 0;    /* 源目录 */
    DIR *dstdir = 0;    /* 源目录 */
    FILINFO *finfo = 0; /* 文件信息 */
    uint8_t *fn = 0;    /* 长文件名 */

    uint8_t *dstpathname = 0;   /* 目标文件夹路径+文件名 */
    uint8_t *srcpathname = 0;   /* 源文件夹路径+文件名 */

    uint16_t dstpathlen = 0;    /* 目标路径长度 */
    uint16_t srcpathlen = 0;    /* 源路径长度 */


    srcdir = (DIR *)mymalloc(SRAMIN, sizeof(DIR));  /* 申请内存 */
    dstdir = (DIR *)mymalloc(SRAMIN, sizeof(DIR));
    finfo = (FILINFO *)mymalloc(SRAMIN, sizeof(FILINFO));

    if (srcdir == NULL || dstdir == NULL || finfo == NULL)res = 100;

    if (res == 0)
    {
        dstpathname = mymalloc(SRAMIN, MAX_PATHNAME_DEPTH);
        srcpathname = mymalloc(SRAMIN, MAX_PATHNAME_DEPTH);

        if (dstpathname == NULL || srcpathname == NULL)res = 101;

        if (res == 0)
        {
            dstpathname[0] = 0;
            srcpathname[0] = 0;
            strcat((char *)srcpathname, (const char *)psrc);    /* 复制原始源文件路径 */
            strcat((char *)dstpathname, (const char *)pdst);    /* 复制原始目标文件路径 */
            res = f_opendir(srcdir, (const TCHAR *)psrc);       /* 打开源目录 */

            if (res == 0)   /* 打开目录成功 */
            {
                strcat((char *)dstpathname, (const char *)"/"); /* 加入斜杠 */
                fn = exfuns_get_src_dname(psrc);

                if (fn == 0)   /* 卷标拷贝 */
                {
                    dstpathlen = strlen((const char *)dstpathname);
                    dstpathname[dstpathlen] = psrc[0];  /* 记录卷标 */
                    dstpathname[dstpathlen + 1] = 0;    /* 结束符 */
                }
                else strcat((char *)dstpathname, (const char *)fn); /* 加文件名 */

                fcpymsg(fn, 0, 0X04);   /* 更新文件夹名 */
                res = f_mkdir((const TCHAR *)dstpathname);  /* 如果文件夹已经存在,就不创建.如果不存在就创建新的文件夹. */

                if (res == FR_EXIST)res = 0;

                while (res == 0)   /* 开始复制文件夹里面的东东 */
                {
                    res = f_readdir(srcdir, finfo);         /* 读取目录下的一个文件 */

                    if (res != FR_OK || finfo->fname[0] == 0)break; /* 错误了/到末尾了,退出 */

                    if (finfo->fname[0] == '.')continue;    /* 忽略上级目录 */

                    fn = (uint8_t *)finfo->fname;           /* 得到文件名 */
                    dstpathlen = strlen((const char *)dstpathname); /* 得到当前目标路径的长度 */
                    srcpathlen = strlen((const char *)srcpathname); /* 得到源路径长度 */

                    strcat((char *)srcpathname, (const char *)"/"); /* 源路径加斜杠 */

                    if (finfo->fattrib & 0X10)  /* 是子目录(文件属性,0X20,归档文件;0X10,子目录;) */
                    {
                        strcat((char *)srcpathname, (const char *)fn);  /* 源路径加上子目录名字 */
                        res = exfuns_folder_copy(fcpymsg, srcpathname, dstpathname, totsize, cpdsize, fwmode);   /* 拷贝文件夹 */
                    }
                    else     /* 非目录 */
                    {
                        strcat((char *)dstpathname, (const char *)"/"); /* 目标路径加斜杠 */
                        strcat((char *)dstpathname, (const char *)fn);  /* 目标路径加文件名 */
                        strcat((char *)srcpathname, (const char *)fn);  /* 源路径加文件名 */
                        fcpymsg(fn, 0, 0X01);       /* 更新文件名 */
                        res = exfuns_file_copy(fcpymsg, srcpathname, dstpathname, *totsize, *cpdsize, fwmode);  /* 复制文件 */
                        *cpdsize += finfo->fsize;   /* 增加一个文件大小 */
                    }

                    srcpathname[srcpathlen] = 0;    /* 加入结束符 */
                    dstpathname[dstpathlen] = 0;    /* 加入结束符 */
                }
            }

            myfree(SRAMIN, dstpathname);
            myfree(SRAMIN, srcpathname);
        }
    }

    myfree(SRAMIN, srcdir);
    myfree(SRAMIN, dstdir);
    myfree(SRAMIN, finfo);
    return res;
}

```

### 3.5 主函数测试

```c
#include "bsp_init.h"
#include "exfuns.h"
#include "sdio.h"

int main(void)
{
  uint32_t total, free;
  uint8_t i = 0;
  uint8_t res =0;
  bsp_init();
  LCD_ShowString(30,70,200,16,16,"FATFS-Test");
  while(1);
  while(sd_init())
  {
    LCD_ShowString(30,150,200,16,16,"SD Card Error!");
    delay_ms(500);
    LCD_ShowString(30,150,200,16,16,"Please Check! ");
    delay_ms(500);
  }
  exfuns_init(); // 为fatfs相关变量申请内存
  f_mount(fs[0],"0:",1); // 挂载SD卡
  res = f_mount(fs[1],"1:",1); // 挂载flash
  if(res = 0x0D)
  {
    LCD_ShowString(30,150,200,16,16,"Flash Disk Formatting...");
    res = f_mkfs("1:",0,0,FF_MAX_SS);
    if(res == 0)
    {
      f_setlabel((const TCHAR*)"1:FLASH");
      LCD_ShowString(30,150,200,16,16,"Flash Disk Format Finish");
    }
    else
    {
      LCD_ShowString(30,150,200,16,16,"Flash Disk Format Error ");
    }
    delay_ms(1000);
  }
  while(exfuns_get_free("0",&total,&free)) // 获取SD卡剩余空间
  {
    LCD_ShowString(30,150,200,16,16,"SD Card Fatfs Error!");
    delay_ms(200);
    LED_TOGGLE(LED0_GPIO_Pin);
  }
  LCD_ShowString(30,150,200,16,16,"FATFS OK!");
  LCD_ShowString(30,170,200,16,16,"SD Total Size:     MB");
  LCD_ShowString(30,190,200,16,16,"SD Free Size:      MB");
  while(1)
  {
    i++;
    delay_ms(200);
    LED_TOGGLE(LED1_GPIO_Pin);
  }

}
```

## 4. API总结

### 4.1 FATFS 模块结构

#### 4.1.1 核心对象

```c
FATFS fs; // 文件系统对象
FIL file; // 文件对象
DIR dir; // 目录对象
FILINFO fileinfo; // 文件信息结构体
```

#### 4.1.2 文件信息结构

```c
typedef struct {
 DWORD fsize; // 文件大小
 WORD fdate; // 修改日期
 WORD ftime; // 修改时间
 BYTE fattrib; // 文件属性
 TCHAR fname[13]; // 短文件名
#if _USE_LFN
 TCHAR* lfname; // 长文件名指针
 int lfsize; // 长文件名缓冲区大小
#endif
} FILINFO;
```

### 4.2 文件系统管理 API

#### 4.2.1 挂载/卸载文件系统

```c
// 挂载文件系统
FRESULT f_mount(FATFS* fs, const TCHAR* path, BYTE opt);

// 卸载文件系统
FRESULT f_unmount(const TCHAR* path);
```

**使用示例**:

```c
FATFS fs; // 文件系统对象

// 挂载SD卡 (路径为 "0:")
FRESULT res = f_mount(&fs, "0:", 1);
if (res != FR_OK) {
 printf("Mount error: %d\n", res);
}

// 使用后卸载
f_unmount("0:");
```

#### 4.2.2 格式化存储设备

```c
FRESULT f_mkfs(const TCHAR* path, BYTE opt, DWORD au, void* work, UINT len);
```

**参数说明**:

- `path`: 逻辑驱动器号 (如 "0:")

- `opt`: 格式化选项 (`FM_FAT`, `FM_FAT32`, `FM_EXFAT`)

- `au`: 分配单元大小 (0表示自动)

- `work`: 工作缓冲区

- `len`: 缓冲区大小

**示例**:

```c
uint8_t work_buffer[4096]; // 格式化所需工作缓冲区

// 格式化SD卡为FAT32
res = f_mkfs("0:", FM_FAT32, 0, work_buffer, sizeof(work_buffer));
if (res == FR_OK) {
 printf("Format successful!\n");
}
```

#### 4.2.3 获取可用空间

```c
FRESULT f_getfree(const TCHAR* path, DWORD* nclst, FATFS** fatfs);
```

**使用示例**:

```c
FRESULT f_getfree(const TCHAR* path, DWORD* nclst, FATFS** fatfs);
```

### 4.3 文件操作 API

#### 4.3.1 打开/创建文件

```c
FRESULT f_open(FIL* fp, const TCHAR* path, BYTE mode);
```

**打开模式**:

| 模式               | 描述            |
| ---------------- | ------------- |
| FA_READ          | 读访问           |
| FA_WRITE         | 写访问           |
| FA_OPEN_EXISTING | 打开现有文件        |
| FA_CREATE_NEW    | 创建新文件 (存在则失败) |
| FA_CREATE_ALWAYS | 创建或覆盖         |
| FA_OPEN_ALWAYS   | 打开或创建         |
| FA_OPEN_APPEND   | 打开并在末尾追加      |

**示例**:

```c
FIL file;

// 以读模式打开文件
res = f_open(&file, "0:/data.txt", FA_READ);
if (res != FR_OK) {
 printf("Open error: %d\n", res);
}

// 创建新文件并写入
res = f_open(&file, "0:/newfile.txt", FA_CREATE_ALWAYS | FA_WRITE);
```

#### 4.3.2 关闭文件

```c
FRESULT f_close(FIL* fp);
```

**示例**:

```c
f_close(&file); // 操作完成后必须关闭文件
```

#### 4.3.3 读写文件

```c
// 读取文件
FRESULT f_read(FIL* fp, void* buff, UINT btr, UINT* br);

// 写入文件
FRESULT f_write(FIL* fp, const void* buff, UINT btw, UINT* bw);
```

**示例**:

```c
char buffer[128];
UINT bytes_read;

// 读取文件内容
res = f_read(&file, buffer, sizeof(buffer), &bytes_read);
if (res == FR_OK) {
 printf("Read %d bytes: %.*s\n", bytes_read, bytes_read, buffer);
}

// 写入数据
const char* text = "Hello FATFS!";
UINT bytes_written;
res = f_write(&file, text, strlen(text), &bytes_written);
```

#### 4.3.4 文件定位

```c
// 移动读写指针
FRESULT f_lseek(FIL* fp, FSIZE_t ofs);

// 截断文件
FRESULT f_truncate(FIL* fp);

// 刷新缓存
FRESULT f_sync(FIL* fp);
```

**示例**:

```c
// 跳转到文件末尾追加数据
f_lseek(&file, f_size(&file));

// 截断文件到当前大小
f_truncate(&file);

// 确保数据写入物理设备
f_sync(&file);
```

#### 4.3.5 文件管理

```c
// 删除文件
FRESULT f_unlink(const TCHAR* path);

// 重命名/移动文件
FRESULT f_rename(const TCHAR* old_name, const TCHAR* new_name);

// 获取文件状态
FRESULT f_stat(const TCHAR* path, FILINFO* fno);
```

**示例**:

```c
// 删除文件
f_unlink("0:/oldfile.txt");

// 重命名文件
f_rename("0:/temp.txt", "0:/data.txt");

// 检查文件是否存在
FILINFO info;
if (f_stat("0:/config.ini", &info) == FR_OK) {
 printf("File size: %lu bytes\n", info.fsize);
}
```

### 4.4 目录操作 API

#### 4.4.1 打开/关闭目录

```c
// 打开目录
FRESULT f_opendir(DIR* dp, const TCHAR* path);

// 关闭目录
FRESULT f_closedir(DIR* dp);
```

#### 4.4.2 读取目录内容

```c
FRESULT f_readdir(DIR* dp, FILINFO* fno);
```

**示例**:

```c
DIR dir;
FILINFO fileinfo;

// 打开根目录
res = f_opendir(&dir, "0:/");
if (res == FR_OK) {
 // 遍历目录内容
 while (f_readdir(&dir, &fileinfo) == FR_OK && fileinfo.fname[0]) {
 printf("%s %8lu\n", fileinfo.fname, fileinfo.fsize);
 }
 f_closedir(&dir);
}
```

#### 4.4.3 创建/删除目录

```c
// 创建目录
FRESULT f_mkdir(const TCHAR* path);

// 删除目录
FRESULT f_unlink(const TCHAR* path); // 同样用于删除目录
```

**示例**:

```c
// 创建新目录
f_mkdir("0:/logs");

// 删除空目录
f_unlink("0:/temp_dir");
```

### 4.5 高级功能

#### 4.5.1 长文件名支持 (LFN)

在 `ffconf.h` 中启用:

```c
#define _USE_LFN 2 // 0:禁用, 1:静态缓冲区, 2:栈缓冲区
#define _MAX_LFN 255 // 最大长文件名长度
```

**使用示例**:

```c
FILINFO info;
TCHAR lfn_buffer[_MAX_LFN + 1];

info.lfname = lfn_buffer;
info.lfsize = sizeof(lfn_buffer);

f_readdir(&dir, &info);
printf("Long name: %s\n", lfn_buffer);
```

#### 4.5.2 多卷管理

```c
// 设置逻辑驱动器号
FRESULT f_setlabel(const TCHAR* label);

// 获取卷标
FRESULT f_getlabel(const TCHAR* path, TCHAR* label, DWORD* vsn);
```

**示例**:

```c
// 设置卷标
f_setlabel("0:MY_SD_CARD");

// 获取卷标
TCHAR label[12];
DWORD vsn;
f_getlabel("0:", label, &vsn);
```

#### 4.5.3 文件系统时间戳

在 `ffconf.h` 中启用:

```c
#define _FS_NORTC 0 // 不使用RTC
#define _NORTC_MON 1 // 默认月份
#define _NORTC_MDAY 1 // 默认日
#define _NORTC_YEAR 2023 // 默认年
```

**设置时间函数**:

```c
DWORD get_fattime(void) {
    // 从RTC获取时间 (格式: YYYY-MM-DD HH:MM:SS)
    RTC_DateTypeDef date;
    RTC_TimeTypeDef time;
    HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);
    
    // 转换为FATFS时间格式
    return ((date.Year + 20) << 25) | 
           (date.Month << 21) | 
           (date.Date << 16) | 
           (time.Hours << 11) | 
           (time.Minutes << 5) | 
           (time.Seconds >> 1);
}
```

### 4.6 错误处理

#### 4.6.1 常见错误代码

| 错误代码               | 描述        |
| ------------------ | --------- |
| FR_OK              | 成功        |
| FR_DISK_ERR        | 底层磁盘错误    |
| FR_INT_ERR         | FATFS内部错误 |
| FR_NOT_READY       | 存储设备未就绪   |
| FR_NO_FILE         | 文件不存在     |
| FR_NO_PATH         | 路径不存在     |
| FR_INVALID_NAME    | 无效文件名     |
| FR_DENIED          | 访问被拒绝     |
| FR_EXIST           | 文件已存在     |
| FR_INVALID_OBJECT  | 无效文件对象    |
| FR_WRITE_PROTECTED | 写保护       |
| FR_INVALID_DRIVE   | 无效驱动器号    |
| FR_NOT_ENABLED     | 文件系统未挂载   |
| FR_NO_FILESYSTEM   | 无有效文件系统   |
| FR_TIMEOUT         | 操作超时      |
| FR_LOCKED          | 文件被锁定     |
| FR_NOT_ENOUGH_CORE | 内存不足      |

#### 4.6.2 错误处理函数

```c
// 获取错误信息字符串
const char* f_errmsg(FRESULT res) {
 switch(res) {
 case FR_OK: return "Succeeded";
 case FR_DISK_ERR: return "Disk error";
 case FR_INT_ERR: return "Internal error";
 // ... 其他错误
 default: return "Unknown error";
 }
}

// 使用示例
if (f_open(&file, "0:/data.txt", FA_READ) != FR_OK) {
 printf("Error: %s\n", f_errmsg(res));
}
```

### 4.7 性能优化技巧

#### 4.7.1 增大文件缓冲区

```c
// 在ffconf.h中调整
#define _MAX_SS 512 // 最大扇区大小
#define _MAX_SS 4096 // 某些SD卡支持4K扇区

// 使用更大的文件缓冲区
FIL file;
uint8_t file_buffer[2048]; // 2KB缓冲区

f_open(&file, "0:/largefile.bin", FA_READ);
f_setbuf(&file, file_buffer); // 设置自定义缓冲区
```

#### 4.7.2 启用快速搜索

```c
// 在ffconf.h中启用
#define _USE_FIND 1

// 使用示例
DIR dir;
FILINFO info;
char pattern[] = "*.TXT"; // 搜索所有TXT文件

f_findfirst(&dir, &info, "0:/", pattern);
while (f_findnext(&dir, &info) == FR_OK && info.fname[0]) {
 printf("Found: %s\n", info.fname);
}
f_closedir(&dir);
```

#### 4.7.3 使用直接数据传输 (避免缓存)

```c
// 直接读写扇区
FRESULT f_read_skip_buf(FIL* fp, void* buff, UINT btr, UINT* br);
FRESULT f_write_skip_buf(FIL* fp, const void* buff, UINT btw, UINT* bw);
```

#### 4.7.4 禁用不需要的功能

在 `ffconf.h` 中:

```c
#define _FS_READONLY 0 // 0:读写, 1:只读
#define _FS_MINIMIZE 0 // 优化级别 (0-3)
#define _USE_STRFUNC 0 // 禁用字符串函数
#define _USE_MKFS 1 // 启用格式化功能
#define _USE_CHMOD 1 // 启用属性控制
```

---


