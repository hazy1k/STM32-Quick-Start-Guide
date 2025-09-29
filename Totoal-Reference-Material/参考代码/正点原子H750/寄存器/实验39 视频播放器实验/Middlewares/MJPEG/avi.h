/**
 ****************************************************************************************************
 * @file        avi.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-01-18
 * @brief       AVI视频格式解析 代码
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
 * V1.0 20220118
 * 第一次发布
 *
 ****************************************************************************************************
 */

#ifndef __AVI_H
#define __AVI_H

#include "./SYSTEM/sys/sys.h"


/* 帧大小, 根据自己的内存申请情况来设置.
 * 如果内存多,可以设置大一点. 如果内存少,设置小一点.
 * 一般设置这个值等于我们的帧缓存大小即可
 */
#define AVI_MAX_FRAME_SIZE  260*1024    /* 最大帧大小,不能超过260KB */

/* 错误类型 */
typedef enum
{
    AVI_OK = 0,             /* 0,成功 */
    AVI_RIFF_ERR,           /* 1,RIFF ID读取失败 */
    AVI_AVI_ERR,            /* 2,AVI ID读取失败 */
    AVI_LIST_ERR,           /* 3,LIST ID读取失败 */
    AVI_HDRL_ERR,           /* 4,HDRL ID读取失败 */
    AVI_AVIH_ERR,           /* 5,AVIH ID读取失败 */
    AVI_STRL_ERR,           /* 6,STRL ID读取失败 */
    AVI_STRH_ERR,           /* 7,STRH ID读取失败 */
    AVI_STRF_ERR,           /* 8,STRF ID读取失败 */
    AVI_MOVI_ERR,           /* 9,MOVI ID读取失败 */
    AVI_FORMAT_ERR,         /* 10,格式错误 */
    AVI_STREAM_ERR,         /* 11,流错误 */
} AVISTATUS;



#define AVI_RIFF_ID         0X46464952
#define AVI_AVI_ID          0X20495641
#define AVI_LIST_ID         0X5453494C
#define AVI_HDRL_ID         0X6C726468      /* 信息块标志 */
#define AVI_MOVI_ID         0X69766F6D      /* 数据块标志 */
#define AVI_STRL_ID         0X6C727473      /* strl标志 */

#define AVI_AVIH_ID         0X68697661      /* avih子块∈AVI_HDRL_ID */
#define AVI_STRH_ID         0X68727473      /* strh(流头)子块∈AVI_STRL_ID */
#define AVI_STRF_ID         0X66727473      /* strf(流格式)子块∈AVI_STRL_ID */
#define AVI_STRD_ID         0X64727473      /* strd子块∈AVI_STRL_ID (可选的) */

#define AVI_VIDS_STREAM     0X73646976      /* 视频流 */
#define AVI_AUDS_STREAM     0X73647561      /* 音频流 */


#define AVI_VIDS_FLAG       0X6463          /* 视频流标志 */
#define AVI_AUDS_FLAG       0X7762          /* 音频流标志 */



#define AVI_FORMAT_MJPG     0X47504A4D


/* AVI 信息结构体 */
/* 将一些重要的数据,存放在这里,方便解码 */
typedef __PACKED_STRUCT
{
    uint32_t SecPerFrame;       /* 视频帧间隔时间(单位为us) */
    uint32_t TotalFrame;        /* 文件总帧数 */
    uint32_t Width;             /* 图像宽 */
    uint32_t Height;            /* 图像高 */
    uint32_t SampleRate;        /* 音频采样率 */
    uint16_t Channels;          /* 声道数,一般为2,表示立体声 */
    uint16_t AudioBufSize;      /* 音频缓冲区大小 */
    uint16_t AudioType;         /* 音频类型:0X0001=PCM;0X0050=MP2;0X0055=MP3;0X2000=AC3; */
    uint16_t StreamID;          /* 流类型ID,StreamID=='dc'==0X6463 /StreamID=='wb'==0X7762 */
    uint32_t StreamSize;        /* 流大小,必须是偶数,如果读取到为奇数,则加1.补为偶数 */
    char *VideoFLAG;            /* 视频帧标记,VideoFLAG="00dc"/"01dc" */
    char *AudioFLAG;            /* 音频帧标记,AudioFLAG="00wb"/"01wb" */
} AVI_INFO;

extern AVI_INFO g_avix;         /* avi文件相关信息 */


/* AVI 块信息 */
typedef struct
{
    uint32_t RiffID;            /* RiffID=='RIFF'==0X61766968 */
    uint32_t FileSize;          /* AVI文件大小(不包含最初的8字节,也RIFFID和FileSize不计算在内) */
    uint32_t AviID;             /* AviID=='AVI '==0X41564920 */
} AVI_HEADER;

/* AVI 块信息 */
typedef struct
{
    uint32_t FrameID;           /* 帧ID,FrameID=='RIFF'==0X61766968 */
    uint32_t FrameSize;         /* 帧大小 */
} FRAME_HEADER;


/* LIST 块信息 */
typedef struct
{
    uint32_t ListID;            /* ListID=='LIST'==0X4c495354 */
    uint32_t BlockSize;         /* 块大小(不包含最初的8字节,也ListID和BlockSize不计算在内) */
    uint32_t ListType;          /* LIST子块类型:hdrl(信息块)/movi(数据块)/idxl(索引块,非必须,是可选的) */
} LIST_HEADER;

/* avih 子块信息 */
typedef struct
{
    uint32_t BlockID;           /* 块标志:avih==0X61766968 */
    uint32_t BlockSize;         /* 块大小(不包含最初的8字节,也就是BlockID和BlockSize不计算在内) */
    uint32_t SecPerFrame;       /* 视频帧间隔时间(单位为us) */
    uint32_t MaxByteSec;        /* 最大数据传输率,字节/秒 */
    uint32_t PaddingGranularity;/* 数据填充的粒度 */
    uint32_t Flags;             /* AVI文件的全局标记，比如是否含有索引块等 */
    uint32_t TotalFrame;        /* 文件总帧数 */
    uint32_t InitFrames;        /* 为交互格式指定初始帧数（非交互格式应该指定为0） */
    uint32_t Streams;           /* 包含的数据流种类个数,通常为2 */
    uint32_t RefBufSize;        /* 建议读取本文件的缓存大小（应能容纳最大的块）默认可能是1M字节!!! */
    uint32_t Width;             /* 图像宽 */
    uint32_t Height;            /* 图像高 */
    uint32_t Reserved[4];       /* 保留 */
} AVIH_HEADER;

/* strh 流头子块信息(strh∈strl) */
typedef struct
{
    uint32_t BlockID;           /* 块标志:strh==0X73747268 */
    uint32_t BlockSize;         /* 块大小(不包含最初的8字节,也就是BlockID和BlockSize不计算在内) */
    uint32_t StreamType;        /* 数据流种类，vids(0X73646976):视频;auds(0X73647561):音频 */
    uint32_t Handler;           /* 指定流的处理者，对于音视频来说就是解码器,比如MJPG/H264之类的 */
    uint32_t Flags;             /* 标记：是否允许这个流输出？调色板是否变化？ */
    uint16_t Priority;          /* 流的优先级（当有多个相同类型的流时优先级最高的为默认流） */
    uint16_t Language;          /* 音频的语言代号 */
    uint32_t InitFrames;        /* 为交互格式指定初始帧数 */
    uint32_t Scale;             /* 数据量, 视频每桢的大小或者音频的采样大小 */
    uint32_t Rate;              /* Scale/Rate=每秒采样数 */
    uint32_t Start;             /* 数据流开始播放的位置，单位为Scale */
    uint32_t Length;            /* 数据流的数据量，单位为Scale */
    uint32_t RefBufSize;        /* 建议使用的缓冲区大小 */
    uint32_t Quality;           /* 解压缩质量参数，值越大，质量越好 */
    uint32_t SampleSize;        /* 音频的样本大小 */
    struct                      /* 视频帧所占的矩形 */
    {
        short Left;
        short Top;
        short Right;
        short Bottom;
    } Frame;
} STRH_HEADER;

/* BMP结构体 */
typedef struct
{
    uint32_t BmpSize;           /* bmp结构体大小,包含(BmpSize在内) */
    long Width;                 /* 图像宽 */
    long Height;                /* 图像高 */
    uint16_t  Planes;           /* 平面数，必须为1 */
    uint16_t  BitCount;         /* 像素位数,0X0018表示24位 */
    uint32_t  Compression;      /* 压缩类型，比如:MJPG/H264等 */
    uint32_t  SizeImage;        /* 图像大小 */
    long XpixPerMeter;          /* 水平分辨率 */
    long YpixPerMeter;          /* 垂直分辨率 */
    uint32_t  ClrUsed;          /* 实际使用了调色板中的颜色数,压缩格式中不使用 */
    uint32_t  ClrImportant;     /* 重要的颜色 */
} BMP_HEADER;

/* 颜色表 */
typedef struct
{
    uint8_t  rgbBlue;           /* 蓝色的亮度(值范围为0-255) */
    uint8_t  rgbGreen;          /* 绿色的亮度(值范围为0-255) */
    uint8_t  rgbRed;            /* 红色的亮度(值范围为0-255) */
    uint8_t  rgbReserved;       /* 保留，必须为0 */
} AVIRGBQUAD;

/* 对于strh,如果是视频流,strf(流格式)使STRH_BMPHEADER块 */
typedef struct
{
    uint32_t BlockID;           /* 块标志,strf==0X73747266 */
    uint32_t BlockSize;         /* 块大小(不包含最初的8字节,也就是BlockID和本BlockSize不计算在内) */
    BMP_HEADER bmiHeader;       /* 位图信息头 */
    AVIRGBQUAD bmColors[1];     /* 颜色表 */
} STRF_BMPHEADER;

/* 对于strh,如果是音频流,strf(流格式)使STRH_WAVHEADER块 */
typedef struct
{
    uint32_t BlockID;           /* 块标志,strf==0X73747266 */
    uint32_t BlockSize;         /* 块大小(不包含最初的8字节,也就是BlockID和本BlockSize不计算在内) */
    uint16_t FormatTag;         /* 格式标志:0X0001=PCM,0X0055=MP3 */
    uint16_t Channels;          /* 声道数,一般为2,表示立体声 */
    uint32_t SampleRate;        /* 音频采样率 */
    uint32_t BaudRate;          /* 波特率 */
    uint16_t BlockAlign;        /* 数据块对齐标志 */
    uint16_t Size;              /* 该结构大小 */
} STRF_WAVHEADER;

#define	 MAKEWORD(ptr)	(uint16_t)(((uint16_t)*((uint8_t*)(ptr))<<8)|(uint16_t)*(uint8_t*)((ptr)+1))
#define  MAKEDWORD(ptr)	(uint32_t)(((uint16_t)*(uint8_t*)(ptr)|(((uint16_t)*(uint8_t*)(ptr+1))<<8)|\
                               (((uint16_t)*(uint8_t*)(ptr+2))<<16)|(((uint16_t)*(uint8_t*)(ptr+3))<<24)))


AVISTATUS avi_init(uint8_t *buf, uint32_t size);                    /* 初始化avi解码器 */
uint32_t avi_srarch_id(uint8_t *buf, uint32_t size, char *id);      /* 查找ID,ID必须是4个字节长度 */
AVISTATUS avi_get_streaminfo(uint8_t *buf);                         /* 获取流信息 */


#endif






















