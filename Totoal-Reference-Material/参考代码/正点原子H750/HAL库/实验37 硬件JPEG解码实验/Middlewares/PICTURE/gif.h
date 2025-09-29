/**
 ****************************************************************************************************
 * @file        gif.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-04
 * @brief       图片解码-gif解码 代码
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
 * V1.0 20200404
 * 第一次发布
 *
 ****************************************************************************************************
 */

#ifndef __GIF_H
#define __GIF_H

#include "./SYSTEM/sys/sys.h"
#include "./FATFS/source/ff.h"

/******************************************************************************************/
/* 用户配置区 */

#define GIF_USE_MALLOC          1       /* 定义是否使用malloc,这里我们选择使用malloc */

/******************************************************************************************/


#define LCD_MAX_LOG_COLORS      256
#define MAX_NUM_LWZ_BITS        12


#define GIF_INTRO_TERMINATOR    ';'    /* 0X3B   GIF文件结束符 */
#define GIF_INTRO_EXTENSION     '!'     /* 0X21 */
#define GIF_INTRO_IMAGE         ','     /* 0X2C */

#define GIF_COMMENT             0xFE
#define GIF_APPLICATION         0xFF
#define GIF_PLAINTEXT           0x01
#define GIF_GRAPHICCTL          0xF9

typedef struct
{
    uint8_t    aBuffer[258];                    /*  Input buffer for data block */
    short aCode  [(1 << MAX_NUM_LWZ_BITS)];     /*  This array stores the LZW codes for the compressed strings */
    uint8_t    aPrefix[(1 << MAX_NUM_LWZ_BITS)];/*  Prefix character of the LZW code */
    uint8_t    aDecompBuffer[3000];             /*  Decompression buffer. The higher the compression, the more bytes are needed in the buffer */
    uint8_t   *sp;                              /*  Pointer into the decompression buffer */
    int   CurBit;
    int   LastBit;
    int   GetDone;
    int   LastByte;
    int   ReturnClear;
    int   CodeSize;
    int   SetCodeSize;
    int   MaxCode;
    int   MaxCodeSize;
    int   ClearCode;
    int   EndCode;
    int   FirstCode;
    int   OldCode;
} LZW_INFO;

/* 逻辑屏幕描述块 */
typedef __PACKED_STRUCT
{
    uint16_t width;     /* GIF宽度 */
    uint16_t height;    /* GIF高度 */
    uint8_t flag;       /* 标识符  1:3:1:3=全局颜色表标志(1):颜色深度(3):分类标志(1):全局颜色表大小(3) */
    uint8_t bkcindex;   /* 背景色在全局颜色表中的索引(仅当存在全局颜色表时有效) */
    uint8_t pixratio;   /* 像素宽高比 */
} LogicalScreenDescriptor;


/* 图像描述块 */
typedef __PACKED_STRUCT
{
    uint16_t xoff;      /* x方向偏移 */
    uint16_t yoff;      /* y方向偏移 */
    uint16_t width;     /* 宽度 */
    uint16_t height;    /* 高度 */
    uint8_t flag;       /* 标识符  1:1:1:2:3=局部颜色表标志(1):交织标志(1):保留(2):局部颜色表大小(3) */
} ImageScreenDescriptor;

/* 图像描述 */
typedef __PACKED_STRUCT
{
    LogicalScreenDescriptor gifLSD; /* 逻辑屏幕描述块 */
    ImageScreenDescriptor gifISD;   /* 图像描述快 */
    uint16_t colortbl[256];         /* 当前使用颜色表 */
    uint16_t bkpcolortbl[256];      /* 备份颜色表.当存在局部颜色表时使用 */
    uint16_t numcolors;             /* 颜色表大小 */
    uint16_t delay;                 /* 延迟时间 */
    LZW_INFO *lzw;                  /* LZW信息 */
} gif89a;

extern uint8_t g_gif_decoding;      /* GIF正在解码标记 */

/* GIF编解码接口函数 */
void gif_quit(void);    /* 退出当前解码 */
uint8_t gif_getinfo(FIL *file, gif89a *gif);    /* 获取GIF信息 */
uint8_t gif_decode(const char *filename, uint16_t x, uint16_t y, uint16_t width, uint16_t height);/* 在指定区域解码一个GIF文件 */

#endif




















