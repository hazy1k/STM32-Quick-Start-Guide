#ifndef __VIDEOPLAY_H
#define __VIDEOPLAY_H 
#include <stm32f4xx.h> 
#include "avi.h"
#include "ff.h"
#include "includes.h"	
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//APP-视频播放器 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/7/20
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//*******************************************************************************
//修改信息
//无
////////////////////////////////////////////////////////////////////////////////// 	   
 
#define AVI_AUDIO_BUF_SIZE    1024*5		//定义avi解码时,音频buf大小.
#define AVI_VIDEO_BUF_SIZE    1024*60		//定义avi解码时,视频buf大小.

//视频播放控制器
typedef __packed struct
{    
	u8 *path;			//当前文件夹路径 
	u8 *name;			//当前歌曲名字
	vu8 status;			//bit0:0,暂停播放;1,继续播放
						//bit1:0,快进/快退中;1,继续播放
						//其他,保留
	
	u16 curindex;		//当前播放的视频文件索引
	u16 mfilenum;		//视频文件数目	    
	u16 *mfindextbl;	//音频文件索引表
	
	FIL *file;			//视频文件指针 	
	vu8 i2splaybuf;		//即将播放的音频帧缓冲编号
	u8* i2sbuf[4]; 		//音频缓冲帧,共4帧,4*AVI_AUDIO_BUF_SIZE
}__videodev; 
extern __videodev videodev;//视频播放控制器


 
void video_time_show(u16 sx,u16 sy,u16 sec);
void video_load_ui(void);
void video_show_vol(u8 pctx);
void video_info_upd(__videodev *videodevx,_progressbar_obj* videoprgbx,_progressbar_obj* volprgbx,AVI_INFO *aviinfo,u8 flag);
u8 video_play(void);
u8 video_play_mjpeg(void); 

u8 video_seek(__videodev *videodevx,AVI_INFO *aviinfo,u8 *mbuf,u32 dstpos);

#endif











