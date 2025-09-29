/**
 ****************************************************************************************************
 * @file        videoplay.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.1
 * @date        2022-05-26
 * @brief       APP-视频播放器 代码
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

#ifndef __VIDEOPLAY_H
#define __VIDEOPLAY_H

#include "./SYSTEM/sys/sys.h"
#include "./MJPEG/avi.h"
#include "./MJPEG/mjpeg.h"
#include "os.h"


#define AVI_AUDIO_BUF_SIZE    1024*5        /* 定义avi解码时,音频buf大小 */
#define AVI_VIDEO_BUF_SIZE    1024*260      /* 定义avi解码时,视频buf大小 */

/* 视频播放控制器 */
typedef __PACKED_STRUCT
{
    uint8_t *path;          /* 当前文件夹路径 */
    uint8_t *name;          /* 当前歌曲名字 */
    volatile uint8_t status;/**
                             * bit0:0,暂停播放;1,继续播放
                             * bit1:0,快进/快退中;1,继续播放
                             * 其他,保留
                             */

    uint16_t curindex;      /* 当前播放的视频文件索引 */
    uint16_t mfilenum;      /* 视频文件数目 */
    uint32_t *mfindextbl;   /* 音频文件索引表 */

    FIL *file;                      /* 视频文件指针 */
    volatile uint8_t saiplaybuf;    /* 即将播放的音频帧缓冲编号 */
    uint8_t *saibuf[4];             /* 音频缓冲帧,共4帧,4*AVI_AUDIO_BUF_SIZE */
} __videodev;

extern __videodev videodev;         /* 视频播放控制器 */


/* 视频播放界面,UI位置结构体 */
typedef __PACKED_STRUCT
{
    /* 顶部标题相关参数 */
    uint8_t tpbar_height;   /* 顶部标题栏高度 */
    uint8_t capfsize;       /* 标题字体大小 */
    uint8_t msgfsize;       /* 提示信息字体大小(视频名/音量/视频数/音频采样率/帧数/分辨率/播放时长等) */

    /* 中间信息栏相关参数 */
    uint8_t msgbar_height;  /* 信息栏高度 */
    uint8_t nfsize;         /* 视频名字体大小 */
    uint8_t xygap;          /* x,y方向的偏移量,歌曲名/音量图标/帧率间隔 1 gap,其他信息,1/2 gap */
    uint16_t vbarx;         /* 音量条x坐标 */
    uint16_t vbary;         /* 音量条y坐标 */
    uint16_t vbarwidth;     /* 音量条长度 */
    uint16_t vbarheight;    /* 音量条宽度 */
    uint8_t msgdis;         /* 横向间隔(分三个) dis+音量图标+vbar+dis+采样率+帧率+dis */

    /* 播放进度相关参数 */
    uint8_t prgbar_height;  /* 进度栏高度 */
    uint16_t pbarwidth;     /* 进度条长度 */
    uint16_t pbarheight;    /* 进度条宽度 */
    uint16_t pbarx;         /* 进度条x坐标 */
    uint16_t pbary;         /* 进度条y坐标 */

    /* 按钮栏相关参数 */
    uint8_t btnbar_height;  /* 按钮栏高度 */
} __videoui;

extern __videoui *vui;      /* 视频播放界面控制器 */


void video_time_show(uint16_t sx, uint16_t sy, uint16_t sec);
void video_load_ui(void);
void video_show_vol(uint8_t pctx);
void video_info_upd(__videodev *videodevx, _progressbar_obj *videoprgbx, _progressbar_obj *volprgbx, AVI_INFO *aviinfo, uint8_t flag);
uint8_t video_play(void);
uint8_t video_play_mjpeg(void);
uint8_t video_seek_vids(__videodev *videodevx, AVI_INFO *aviinfo, uint8_t *mbuf);
uint8_t video_seek(__videodev *videodevx, AVI_INFO *aviinfo, uint8_t *mbuf, uint32_t dstpos);
uint8_t video_seek_key(FIL *favi, AVI_INFO *aviinfo, uint8_t *mbuf);    /* 按键方式快进快退 */

#endif































