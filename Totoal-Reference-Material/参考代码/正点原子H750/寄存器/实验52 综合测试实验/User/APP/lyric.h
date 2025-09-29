/**
 ****************************************************************************************************
 * @file        lyric.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.1
 * @date        2022-05-26
 * @brief       APP-歌词显示实现 代码
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

#ifndef __LYRIC_H
#define __LYRIC_H

#include "os.h"
#include "common.h"


#define LYRIC_BUFFER_SIZE       512     /* 歌词缓存,512字节 */
#define LYRIC_MAX_SIZE          200     /* 最大歌词条数 */

/* LYRIC结构 */
typedef __PACKED_STRUCT
{
    uint32_t time_tbl[LYRIC_MAX_SIZE];  /* LRC时间表 */
    uint16_t addr_tbl[LYRIC_MAX_SIZE];  /* LRC地址表 */
    uint16_t indexsize;                 /* LRC大小,歌词总条数(包括重复的). */
    uint16_t curindex;                  /* 当前位置 */
    uint32_t curtime;                   /* 当前时间,单位10ms */
    
    /* oldostime,给定时器用 */
    uint32_t oldostime;                 /* 上一次ucos的时间 */
    
    /* 实现滚动效果的4个必要参数 */
    uint8_t  detatime;                  /* 上一次滚动后,经过的10ms次数 */
    uint8_t  updatetime;                /* 更新时间:即当前根据前后两句歌词的时间差,以及本句歌词的长度确定的滚动时间间隔 */
    uint16_t namelen;
    uint16_t curnamepos;

    //uint16_t bkcolor;                 /* 歌词背景色 */
    uint16_t color;                     /* 歌词颜色 */
    uint8_t font;                       /* 歌词字体 */

    FIL *flrc;                          /* LRC文件 */
    uint8_t buf[LYRIC_BUFFER_SIZE];     /* LRC歌词文件缓存区 */
    uint16_t *lrcbkcolor[7];            /* 7条歌词的背景色表 */
} _lyric_obj;


void lrc_chg_suffix(uint8_t *name, uint8_t *sfx);
_lyric_obj *lrc_creat(void);
void lrc_delete(_lyric_obj *lcrdelete);
uint8_t lrc_str2num(uint8_t *str);
void lrc_analyze(_lyric_obj *lrcx, uint16_t lrcpos, uint8_t *str);
void lrc_sequence(_lyric_obj *lrcx);
uint8_t lrc_read(_lyric_obj *lrcx, uint8_t *path, uint8_t *name);
void lrc_show_linelrc(_lyric_obj *lrcx, uint16_t x, uint16_t y, uint16_t width, uint16_t height);


uint8_t lrc_test(uint8_t *path, uint8_t *name);
#endif












