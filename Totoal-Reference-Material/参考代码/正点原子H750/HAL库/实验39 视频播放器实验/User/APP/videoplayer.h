/**
 ****************************************************************************************************
 * @file        videoplayer.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-05-24
 * @brief       视频播放器 应用代码
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
 * V1.0 20220524
 * 第一次发布
 *
 ****************************************************************************************************
 */

#ifndef __VIDEOPLAYER_H
#define __VIDEOPLAYER_H

#include "./MJPEG/avi.h"
#include "./SYSTEM/sys/sys.h"
#include "./FATFS/source/ff.h"


#define AVI_AUDIO_BUF_SIZE      1024*5          /* 定义avi解码时,音频buf大小 */
#define AVI_VIDEO_BUF_SIZE      1024*260        /* 定义avi解码时,视频buf大小.一般等于AVI_MAX_FRAME_SIZE的大小 */


void video_play(void);
uint8_t video_play_mjpeg(char *pname); 
void video_time_show(FIL *favi,AVI_INFO *aviinfo);
void video_info_show(AVI_INFO *aviinfo);
void video_bmsg_show(char* name,uint16_t index,uint16_t total);
uint16_t video_get_tnum(char *path);
uint8_t video_seek(FIL *favi,AVI_INFO *aviinfo, uint8_t *mbuf);

#endif

