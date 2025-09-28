/*

libdemac - A Monkey's Audio decoder

$Id: decoder.c 28632 2010-11-21 17:58:42Z Buschel $

Copyright (C) Dave Chapman 2007

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110, USA

*/

#include <inttypes.h>
#include <string.h>

#include "apedecoder.h"
#include "predictor.h"
#include "entropy.h"
#include "filter.h"
#include "demac_config.h"

/* Statically allocate the filter buffers */

#ifdef FILTER256_IRAM
static filter_int filterbuf32[(32*3 + FILTER_HISTORY_SIZE) * 2]   
                  IBSS_ATTR_DEMAC MEM_ALIGN_ATTR; 
                  /* 2432 or 4864 bytes */
static filter_int filterbuf256[(256*3 + FILTER_HISTORY_SIZE) * 2]
                  IBSS_ATTR_DEMAC MEM_ALIGN_ATTR; 
                  /* 5120 or 10240 bytes */
#define FILTERBUF64 filterbuf256
#define FILTERBUF32 filterbuf32
#define FILTERBUF16 filterbuf32
#else
//static filter_int filterbuf64[(64*3 + FILTER_HISTORY_SIZE) * 2]   
//                  IBSS_ATTR_DEMAC MEM_ALIGN_ATTR; 
//                  /* 2816 or 5632 bytes */
//static filter_int filterbuf256[(256*3 + FILTER_HISTORY_SIZE) * 2]
//                  MEM_ALIGN_ATTR; /* 5120 or 10240 bytes */

filter_int *filterbuf64; 	//需要2816字节
filter_int *filterbuf256;	//需要5120字节

#define FILTERBUF64 filterbuf64
#define FILTERBUF32 filterbuf64
#define FILTERBUF16 filterbuf64
#endif



///* This is only needed for "insane" files, and no current Rockbox targets
//   can hope to decode them in realtime, except the Gigabeat S (at 528MHz). */
//static filter_int filterbuf1280[(1280*3 + FILTER_HISTORY_SIZE) * 2] 
//                  IBSS_ATTR_DEMAC_INSANEBUF MEM_ALIGN_ATTR;
//                  /* 17408 or 34816 bytes */

filter_int *filterbuf1280;	//需要17408字节

void init_frame_decoder(struct ape_ctx_t* ape_ctx,
                        unsigned char* inbuffer, int* firstbyte,
                        int* bytesconsumed)
{
    init_entropy_decoder(ape_ctx, inbuffer, firstbyte, bytesconsumed);
    init_predictor_decoder(&ape_ctx->predictor);
    switch (ape_ctx->compressiontype)
    {
        case 2000:
            init_filter_16_11(FILTERBUF16);
            break;

        case 3000:
            init_filter_64_11(FILTERBUF64);
            break;
//高于3000的,无法在STM32F4上面流畅播放.
//        case 4000:
//            init_filter_256_13(filterbuf256);
//            init_filter_32_10(FILTERBUF32);
//            break;
//        case 5000:
//            init_filter_1280_15(filterbuf1280);
//            init_filter_256_13(filterbuf256);
//            init_filter_16_11(FILTERBUF32);
    }
}
//根据文件位置查找帧起始地址
//fpos:当前文件读取位置
//curframe:当前帧编号
//firstbyte:firstbyte参数
//apex:ape解码参数结构体
//返回值:0XFFFFFFFF,无法定位
//             其他,定位后的文件位置,也就是应该读取数据的地方
u32 ape_seek_frame(u32 fpos,u32*curframe,u32*firstbyte,struct ape_ctx_t *apex)
{  
	if((apex->seektablelength/sizeof(uint32_t))!=apex->totalframes)
	{ 
		return 0XFFFFFFFF;
	}
    while((*curframe<apex->totalframes)&&(*curframe<apex->numseekpoints)&&(fpos>apex->seektable[*curframe]))
    {
        ++*curframe;
        *curframe+=apex->blocksperframe;
    }
    if ((*curframe>0)&&(apex->seektable[*curframe]>fpos)) 
	{
        --*curframe;
    }
    fpos=apex->seektable[*curframe];//新的frame开始地址
    *firstbyte=3-(fpos&3); 
	fpos&=~3;
	return fpos;	
}
int  decode_chunk(struct ape_ctx_t* ape_ctx,
                                  unsigned char* inbuffer, int* firstbyte,
                                  int* bytesconsumed,
                                  int32_t* decoded0, int32_t* decoded1,
                                  int count)
{
    uint16_t left;
	uint16_t *abuf=(uint16_t*)decoded1;//利用decode1作音频输出缓冲
	
    if ((ape_ctx->channels==1) || ((ape_ctx->frameflags
        & (APE_FRAMECODE_PSEUDO_STEREO|APE_FRAMECODE_STEREO_SILENCE))
        == APE_FRAMECODE_PSEUDO_STEREO)) {

        entropy_decode(ape_ctx, inbuffer, firstbyte, bytesconsumed,
                       decoded0, NULL, count);

        if (ape_ctx->frameflags & APE_FRAMECODE_MONO_SILENCE) {
            /* We are pure silence, so we're done. */
            return 0;
        }

        switch (ape_ctx->compressiontype)
        {
            case 2000:
                apply_filter_16_11(ape_ctx->fileversion,0,decoded0,count);
                break;
    
            case 3000:
                apply_filter_64_11(ape_ctx->fileversion,0,decoded0,count);
                break;
    
//高于3000的,无法在STM32F4上面流畅播放.
//            case 4000:
//                apply_filter_32_10(ape_ctx->fileversion,0,decoded0,count);
//                apply_filter_256_13(ape_ctx->fileversion,0,decoded0,count);
//                break;
//    
//            case 5000:
//                apply_filter_16_11(ape_ctx->fileversion,0,decoded0,count);
//                apply_filter_256_13(ape_ctx->fileversion,0,decoded0,count);
//                apply_filter_1280_15(ape_ctx->fileversion,0,decoded0,count);
        }

        /* Now apply the predictor decoding */
        predictor_decode_mono(&ape_ctx->predictor,decoded0,count);
		//单声道也做立体声处理
        //if (ape_ctx->channels==2) 
		{
            /* Pseudo-stereo - copy left channel to right channel */
            while (count--)
            {
				*(abuf++)=*decoded0;
				*(abuf++)=*(decoded0++); 
            }
        } 
    } else { /* Stereo */
        entropy_decode(ape_ctx, inbuffer, firstbyte, bytesconsumed,
                       decoded0, decoded1, count);

        if ((ape_ctx->frameflags & APE_FRAMECODE_STEREO_SILENCE)
            == APE_FRAMECODE_STEREO_SILENCE) {
            /* We are pure silence, so we're done. */
            return 0;
        }

        /* Apply filters - compression type 1000 doesn't have any */
        switch (ape_ctx->compressiontype)
        {
            case 2000:
                apply_filter_16_11(ape_ctx->fileversion,0,decoded0,count);
                apply_filter_16_11(ape_ctx->fileversion,1,decoded1,count);
                break;
    
            case 3000:
                apply_filter_64_11(ape_ctx->fileversion,0,decoded0,count);
                apply_filter_64_11(ape_ctx->fileversion,1,decoded1,count);
                break;
    
//高于3000的,无法在STM32F4上面流畅播放.
//            case 4000:
//                apply_filter_32_10(ape_ctx->fileversion,0,decoded0,count);
//                apply_filter_32_10(ape_ctx->fileversion,1,decoded1,count);
//                apply_filter_256_13(ape_ctx->fileversion,0,decoded0,count);
//                apply_filter_256_13(ape_ctx->fileversion,1,decoded1,count);
//                break;
//    
//            case 5000:
//                apply_filter_16_11(ape_ctx->fileversion,0,decoded0,count);
//                apply_filter_16_11(ape_ctx->fileversion,1,decoded1,count);
//                apply_filter_256_13(ape_ctx->fileversion,0,decoded0,count);
//                apply_filter_256_13(ape_ctx->fileversion,1,decoded1,count);
//                apply_filter_1280_15(ape_ctx->fileversion,0,decoded0,count);
//                apply_filter_1280_15(ape_ctx->fileversion,1,decoded1,count);
        }

        /* Now apply the predictor decoding */
        predictor_decode_stereo(&ape_ctx->predictor,decoded0,decoded1,count);

        /* Decorrelate and scale to output depth */
        while (count--)
        {
            left = *(decoded1++) - (*decoded0 / 2);
			*(abuf++)=left;
            *(abuf++)=left+*(decoded0++); 
        }
    }
    return 0;
}

































