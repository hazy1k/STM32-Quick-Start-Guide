#ifndef __FLACPLAY_H__ 
#define __FLACPLAY_H__ 
#include <inttypes.h>
#include <string.h>
#include "flacdecoder.h"
#include "sys.h"  
#include "ff.h"  
//////////////////////////////////////////////////////////////////////////////////	 
//本程序移植自RockBox的flac解码库
//ALIENTEK STM32F407开发板
//FLAC 解码代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/6/29
//版本：V1.0
//********************************************************************************
//V1.0 说明
//1,支持16/24位单声道/立体声flac的解码
//2,最高支持192K/16bit或96K/24bit的flac解码  
////////////////////////////////////////////////////////////////////////////////// 	 

 
//flaC 标签 
typedef __packed struct 
{
    u8 id[3];		   	//ID,在文件起始位置,必须是flaC 4个字母 
}FLAC_Tag;

//metadata 数据块头信息结构体 
typedef __packed struct 
{
    u8 head;		   	//metadata block头
	u8 size[3];			//metadata block数据长度	
}MD_Block_Head;


//FLAC控制结构体
typedef __packed struct 
{ 
    u32 totsec ;				//整首歌时长,单位:秒
    u32 cursec ;				//当前播放时长
	
    u32 bitrate;	   			//比特率
	u32 samplerate;				//采样率
	u16 outsamples;				//PCM输出数据量大小
	u16 bps;					//位数,比如16bit,24bit,32bit
	
	u32 datastart;				//数据帧开始的位置(在文件里面的偏移)
}__flacctrl;

extern __flacctrl * flacctrl;


u8 flac_init(FIL* fx,__flacctrl* fctrl,FLACContext* fc);
void flac_i2s_dma_tx_callback(void);
void flac_get_curtime(FIL*fx,__flacctrl *flacx);
u8 flac_play_song(u8* fname);

#endif




























