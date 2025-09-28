#ifndef __APEPLAY_H__ 
#define __APEPLAY_H__ 
#include "apedecoder.h"
#include "parser.h"
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序移植自RockBox的ape解码库
//ALIENTEK STM32F407开发板
//APE 解码代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/6/29
//版本：V1.0
//********************************************************************************
//V1.0 说明
//1,支持16位单声道/立体声ape的解码
//2,最高支持96K的APE格式(LV1流畅,LV2小卡)
//3,LV1~LV3,在48K及以下流畅播放,LV4,LV5大卡.
//4,对某些ape文件,可能不支持,请用Monkey's Audio软件进行转换一下,即可正常播放
////////////////////////////////////////////////////////////////////////////////// 	 
 
#define APE_FILE_BUF_SZ    			20*1024		//APE解码时,文件buf大小
#define APE_BLOCKS_PER_LOOP			2*1024		//APE解码时,每个循环解码block的个数

//APE控制结构体
typedef __packed struct 
{ 
    u32 totsec ;				//整首歌时长,单位:秒
    u32 cursec ;				//当前播放时长
	
    u32 bitrate;	   			//比特率
	u32 samplerate;				//采样率
	u16 outsamples;				//PCM输出数据量大小
	u16 bps;					//位数,比如16bit,24bit,32bit
	
	u32 datastart;				//数据帧开始的位置(在文件里面的偏移)
}__apectrl;

extern __apectrl * apectrl; 


void ape_fill_buffer(u16* buf,u16 size);
void ape_i2s_dma_tx_callback(void);
void ape_get_curtime(FIL*fx,__apectrl *apectrl);
u8 ape_play_song(u8* fname);
#endif
