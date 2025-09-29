/**
 ****************************************************************************************************
 * @file        piclib.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-04
 * @brief       图片解码库 代码
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

#ifndef __PICLIB_H
#define __PICLIB_H

#include "./BSP/LCD/lcd.h"
#include "./MALLOC/malloc.h"
#include "./SYSTEM/sys/sys.h"
#include "./FATFS/exfuns/exfuns.h"

#include "./PICTURE/bmp.h"
#include "./PICTURE/gif.h"
#include "./PICTURE/tjpgd.h"


#define PIC_FORMAT_ERR      0x27    /* 格式错误 */
#define PIC_SIZE_ERR        0x28    /* 图片尺寸错误 */
#define PIC_WINDOW_ERR      0x29    /* 窗口设定错误 */
#define PIC_MEM_ERR         0x11    /* 内存错误 */

/* 判断 TRUE 和 FALSE 是否已经定义了?, 没有则要定义! */
#ifndef TRUE
#define TRUE    1
#endif
#ifndef FALSE
#define FALSE   0
#endif

/* 图片显示物理层接口 */
/* 在移植的时候,必须由用户自己实现这几个函数 */
typedef struct
{
    /* uint32_t read_point(uint16_t x,uint16_t y) 读点函数 */
    uint32_t(*read_point)(uint16_t, uint16_t);
    
    /* void draw_point(uint16_t x,uint16_t y,uint32_t color) 画点函数 */
    void(*draw_point)(uint16_t, uint16_t, uint32_t);
    
    /* void fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint32_t color) 单色填充函数 */
    void(*fill)(uint16_t, uint16_t, uint16_t, uint16_t, uint32_t);
    
    /* void draw_hline(uint16_t x0,uint16_t y0,uint16_t len,uint16_t color) 画水平线函数 */
    void(*draw_hline)(uint16_t, uint16_t, uint16_t, uint16_t);
    
    /* void piclib_fill_color(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint16_t *color) 颜色填充 */
    void(*fillcolor)(uint16_t, uint16_t, uint16_t, uint16_t, uint16_t *);
} _pic_phy;

extern _pic_phy pic_phy;


/* 图像信息 */
typedef struct
{
    uint16_t lcdwidth;      /* LCD的宽度 */
    uint16_t lcdheight;     /* LCD的高度 */
    uint32_t ImgWidth;      /* 图像的实际宽度和高度 */
    uint32_t ImgHeight;

    uint32_t Div_Fac;       /* 缩放系数 (扩大了8192倍的) */

    uint32_t S_Height;      /* 设定的高度和宽度 */
    uint32_t S_Width;

    uint32_t S_XOFF;        /* x轴和y轴的偏移量 */
    uint32_t S_YOFF;

    uint32_t staticx;       /* 当前显示到的ｘｙ坐标 */
    uint32_t staticy;
} _pic_info;

extern _pic_info picinfo;   /* 图像信息 */


/* 图片解码库 接口函数 */
void piclib_mem_free (void *paddr);     /* 释放内存 */
void *piclib_mem_malloc (uint32_t size);/* 申请内存 */

void piclib_init(void);                 /* 初始化画图 */
void piclib_ai_draw_init(void);         /* 初始化智能画图 */
uint16_t piclib_alpha_blend(uint16_t src, uint16_t dst, uint8_t alpha);     /* alphablend处理 */
uint8_t piclib_is_element_ok(uint16_t x, uint16_t y, uint8_t chg); /* 判断像素是否有效 */
uint8_t piclib_ai_load_picfile(char *filename, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t fast); /* 智能画图 */

#endif






























