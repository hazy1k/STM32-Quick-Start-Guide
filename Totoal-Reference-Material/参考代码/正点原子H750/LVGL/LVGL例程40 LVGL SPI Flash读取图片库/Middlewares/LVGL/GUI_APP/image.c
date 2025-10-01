/**
 ****************************************************************************************************
 * @file        image.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-04
 * @brief       图片库 代码
 *              提供image_update_image和images_init用于图片库更新和初始化
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
 * 修改说明
 * V1.0 20200404
 * 第一次发布
 *
 ****************************************************************************************************
 */

#include "string.h" 
#include "image.h"
#include "./BSP/LCD/LCD.h"
#include "./MALLOC/malloc.h"
#include "./FATFS/source/ff.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/NORFLASH/norflash_ex.h"


/* 图片库区域占用的总扇区数大小(4个图片库+unigbk表+图片库信息=6302984字节,约占1539个25QXX扇区,一个扇区4K字节) */
#define IMAGESECSIZE         1539


/* 图片库存放起始地址
 * 从第701个扇区开始存放图片库
 * 前面701个扇区被code区和spb区占用了.
 * 701扇区后紧跟4个图片库+UNIGBK.BIN,总大小6.01M,1539个扇区,被图片库占用了,不能动!
 * 2240及以后扇区被文件系统占用
 */
#define IMAGEINFOADDR        701*4096

/* 用来保存图片库基本信息，地址，大小等 */
_image_info ftinfo;

/* 图片库存放在磁盘中的路径 */
char *const IMAGE_GBK_PATH[4] =
{
    "/PICTURE/LVGLBIN/atk05.BIN",
    "/PICTURE/LVGLBIN/atk06.BIN",
    "/PICTURE/LVGLBIN/atk07.BIN",
    "/PICTURE/LVGLBIN/money.BIN",
};

/* 更新时的提示信息 */
char *const IMAGE_UPDATE_REMIND_TBL[4] =
{
    "Updating atk05.BIN",
    "Updating atk06.BIN",
    "Updating atk07.BIN",
    "Updating money.BIN",
};

#define IMAGE_GBK_NUM           (int)(sizeof(IMAGE_GBK_PATH)/sizeof(IMAGE_GBK_PATH[0]))
#define IMAGE_UPDATE_REMIND_NUM (int)(sizeof(IMAGE_UPDATE_REMIND_TBL)/sizeof(IMAGE_UPDATE_REMIND_TBL[0]))
/**
 * @brief       显示当前图片更新进度
 * @param       x, y    : 坐标
 * @param       size    : 图片大小
 * @param       totsize : 整个文件大小
 * @param       pos     : 当前文件指针位置
 * @param       color   : 图片颜色
 * @retval      无
 */
static void images_progress_show(uint16_t x, uint16_t y, uint8_t size, uint32_t totsize, uint32_t pos, uint16_t color)
{
    float prog;
    uint8_t t = 0XFF;
    prog = (float)pos / totsize;
    prog *= 100;

    if (t != prog)
    {
        lcd_show_string(x + 3 * size / 2, y, 240, 320, size, "%", color);
        t = prog;

        if (t > 100)t = 100;

        lcd_show_num(x, y, t, 3, size, color);  /* 显示数值 */
    }
}

/**
 * @brief       更新某一个图片库
 * @param       x, y    : 提示信息的显示地址
 * @param       size    : 提示信息图片大小
 * @param       fpath   : 图片路径
 * @param       fx      : 更新的内容
 *   @arg                 0, atk01;
 *   @Arg                 1, atk02;
 *   @arg                 2, atk03;
 *   @arg                 3, atk04;
 *   @arg                 4, atk05;
 * @param       color   : 图片颜色
 * @retval      0, 成功; 其他, 错误代码;
 */
static uint8_t images_update_imagex(uint16_t x, uint16_t y, uint8_t size, uint8_t *fpath, uint8_t fx, uint16_t color)
{
    uint32_t flashaddr = 0;
    FIL *fftemp;
    uint8_t *tempbuf;
    uint8_t res;
    uint16_t bread;
    uint32_t offx = 0;
    uint8_t rval = 0;
    fftemp = (FIL *)mymalloc(SRAMIN, sizeof(FIL));  /* 分配内存 */

    if (fftemp == NULL)rval = 1;

    tempbuf = mymalloc(SRAMIN, 4096);               /* 分配4096个字节空间 */

    if (tempbuf == NULL)rval = 1;

    res = f_open(fftemp, (const TCHAR *)fpath, FA_READ);

    if (res)rval = 2;   /* 打开文件失败 */

    if (rval == 0)
    {
        switch (fx)
        {
            case 0: /* 更新atk01.BIN */
                ftinfo.lvgl_atk01addr = IMAGEINFOADDR + sizeof(ftinfo);                  /* 信息头之后，紧跟atk02 */
                ftinfo.lvgl_atk01size = fftemp->obj.objsize;                             /* atk01大小 */
                flashaddr = ftinfo.lvgl_atk01addr;
                break;
            case 1: /* 更新atk02.BIN */
                ftinfo.lvgl_atk02addr = ftinfo.lvgl_atk01addr + ftinfo.lvgl_atk01size;    /* 信息头之后，紧跟atk03 */
                ftinfo.lvgl_atk02size = fftemp->obj.objsize;                              /* atk02大小 */
                flashaddr = ftinfo.lvgl_atk02addr;
                break;
            case 2: /* 更新atk03.BIN */
                ftinfo.lvgl_atk03addr = ftinfo.lvgl_atk02addr + ftinfo.lvgl_atk02size;    /* 信息头之后，紧跟money */
                ftinfo.lvgl_atk03size = fftemp->obj.objsize;                              /* atk03大小 */
                flashaddr = ftinfo.lvgl_atk03addr;
                break;
            case 3: /* 更新money.BIN */
                ftinfo.lvgl_moneyaddr = ftinfo.lvgl_atk03addr + ftinfo.lvgl_atk03size;    /* 信息头之后 */
                ftinfo.lvgl_moneysize = fftemp->obj.objsize;                              /* money大小 */
                flashaddr = ftinfo.lvgl_moneyaddr;
                break;
        }

        while (res == FR_OK)   /* 死循环执行 */
        {
            res = f_read(fftemp, tempbuf, 4096, (UINT *)&bread);    /* 读取数据 */

            if (res != FR_OK)break;     /* 执行错误 */

            norflash_ex_write(tempbuf, offx + flashaddr, bread);    /* 从0开始写入bread个数据 */
            offx += bread;
            images_progress_show(x, y, size, fftemp->obj.objsize, offx, color);    /* 进度显示 */

            if (bread != 4096)break;    /* 读完了. */
        }

        f_close(fftemp);
    }

    myfree(SRAMIN, fftemp);     /* 释放内存 */
    myfree(SRAMIN, tempbuf);    /* 释放内存 */
    return res;
}

/**
 * @brief       更新图片文件
 *   @note      所有图片库一起更新(UNIGBK,GBK12,GBK16,GBK24,GBK32)
 * @param       x, y    : 提示信息的显示地址
 * @param       size    : 提示信息图片大小
 * @param       src     : 图片库来源磁盘
 *   @arg                 "0:", SD卡;
 *   @Arg                 "1:", FLASH盘
 *   @arg                 "2:", U盘
 * @param       color   : 图片颜色
 * @retval      0, 成功; 其他, 错误代码;
 */
uint8_t images_update_image(uint16_t x, uint16_t y, uint8_t size, uint8_t *src, uint16_t color)
{
    uint8_t *pname;
    uint32_t *buf;
    uint8_t res = 0;
    uint16_t i, j;
    FIL *fftemp;
    uint8_t rval = 0;
    res = 0XFF;
    ftinfo.imageok = 0XFF;
    pname = mymalloc(SRAMIN, 100);  /* 申请100字节内存 */
    buf = mymalloc(SRAMIN, 4096);   /* 申请4K字节内存 */
    fftemp = (FIL *)mymalloc(SRAMIN, sizeof(FIL));  /* 分配内存 */

    if (buf == NULL || pname == NULL || fftemp == NULL)
    {
        myfree(SRAMIN, fftemp);
        myfree(SRAMIN, pname);
        myfree(SRAMIN, buf);
        return 5;   /* 内存申请失败 */
    }

    for (i = 0; i < IMAGE_GBK_NUM; i++) /* 先查找文件atk01,atk02,atk03,money是否正常 */
    {
        strcpy((char *)pname, (char *)src);                  /* copy src内容到pname */
        strcat((char *)pname, (char *)IMAGE_GBK_PATH[i]);    /* 追加具体文件路径 */
        res = f_open(fftemp, (const TCHAR *)pname, FA_READ); /* 尝试打开 */

        if (res)
        {
            rval |= 1 << 7; /* 标记打开文件失败 */
            break;          /* 出错了,直接退出 */
        }
    }

    myfree(SRAMIN, fftemp); /* 释放内存 */

    if (rval == 0)          /* 图片库文件都存在. */
    {
        lcd_show_string(x, y, 240, 320, size, "Erasing sectors... ", color);    /* 提示正在擦除扇区 */

        for (i = 0; i < IMAGESECSIZE; i++)   /* 先擦除图片库区域,提高写入速度 */
        {
            images_progress_show(x + 20 * size / 2, y, size, IMAGESECSIZE, i, color);    /* 进度显示 */
            norflash_ex_read((uint8_t *)buf, ((IMAGEINFOADDR / 4096) + i) * 4096, 4096); /* 读出整个扇区的内容 */

            for (j = 0; j < 1024; j++)          /* 校验数据 */
            {
                if (buf[j] != 0XFFFFFFFF)break; /* 需要擦除 */
            }

            if (j != 1024)
            {
                norflash_ex_erase_sector((IMAGEINFOADDR / 4096) + i); /* 需要擦除的扇区 */
            }
        }

        for (i = 0; i < IMAGE_UPDATE_REMIND_NUM; i++) /* 依次更新atk01,atk02,atk03,money */
        {
            lcd_show_string(x, y, 240, 320, size, IMAGE_UPDATE_REMIND_TBL[i], color);
            strcpy((char *)pname, (char *)src);              /* copy src内容到pname */
            strcat((char *)pname, (char *)IMAGE_GBK_PATH[i]);/* 追加具体文件路径 */
            res = images_update_imagex(x + 20 * size / 2, y, size, pname, i, color);    /* 更新图片库 */

            if (res)
            {
                myfree(SRAMIN, buf);
                myfree(SRAMIN, pname);
                return 1 + i;
            }
        }

        /* 全部更新好了 */
        ftinfo.imageok = 0XAA;
        norflash_ex_write((uint8_t *)&ftinfo, IMAGEINFOADDR, sizeof(ftinfo));    /* 保存图片库信息 */
    }

    myfree(SRAMIN, pname);  /* 释放内存 */
    myfree(SRAMIN, buf);
    return rval;            /* 无错误. */
}

/**
 * @brief       初始化图片
 * @param       无
 * @retval      0, 图片库完好; 其他, 图片库丢失;
 */
uint8_t images_init(void)
{
    uint8_t t = 0;

    while (t < 10)  /* 连续读取10次,都是错误,说明确实是有问题,得更新图片库了 */
    {
        t++;
        norflash_ex_read((uint8_t *)&ftinfo, IMAGEINFOADDR, sizeof(ftinfo)); /* 读出ftinfo结构体数据 */

        if (ftinfo.imageok == 0XAA)
        {
            break;
        }
        
        delay_ms(20);
    }

    if (ftinfo.imageok != 0XAA)
    {
        return 1;
    }
    
    return 0;
}
