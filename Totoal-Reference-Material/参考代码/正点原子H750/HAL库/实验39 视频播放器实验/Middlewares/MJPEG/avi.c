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

#include "./MJPEG/avi.h"
#include "./MJPEG/mjpeg.h"
#include "./SYSTEM/usart/usart.h"


AVI_INFO g_avix;                                        /* avi文件相关信息 */
char *const AVI_VIDS_FLAG_TBL[2] = {"00dc", "01dc"};    /* 视频编码标志字符串,00dc/01dc */
char *const AVI_AUDS_FLAG_TBL[2] = {"00wb", "01wb"};    /* 音频编码标志字符串,00wb/01wb */


/**
 * @brief       AVI解码初始化
 * @param       buf  : 输入缓冲区
 * @param       size : 缓冲区大小
 * @retval      执行结果
 *   @arg       AVI_OK, AVI文件解析成功
 *   @arg       其他  , 错误代码
 */
AVISTATUS avi_init(uint8_t *buf, uint32_t size)
{
    uint16_t offset;
    uint8_t *tbuf;
    AVISTATUS res = AVI_OK;
    AVI_HEADER *aviheader;
    LIST_HEADER *listheader;
    AVIH_HEADER *avihheader;
    STRH_HEADER *strhheader;

    STRF_BMPHEADER *bmpheader;
    STRF_WAVHEADER *wavheader;

    tbuf = buf;
    aviheader = (AVI_HEADER *)buf;

    if (aviheader->RiffID != AVI_RIFF_ID)return AVI_RIFF_ERR;   /* RIFF ID错误 */

    if (aviheader->AviID != AVI_AVI_ID)return AVI_AVI_ERR;      /* AVI ID错误 */

    buf += sizeof(AVI_HEADER);  /* 偏移 */
    listheader = (LIST_HEADER *)(buf);

    if (listheader->ListID != AVI_LIST_ID)return AVI_LIST_ERR;  /* LIST ID错误 */

    if (listheader->ListType != AVI_HDRL_ID)return AVI_HDRL_ERR;/* HDRL ID错误 */

    buf += sizeof(LIST_HEADER); /* 偏移 */
    avihheader = (AVIH_HEADER *)(buf);

    if (avihheader->BlockID != AVI_AVIH_ID)return AVI_AVIH_ERR; /* AVIH ID错误 */

    g_avix.SecPerFrame = avihheader->SecPerFrame;   /* 得到帧间隔时间 */
    g_avix.TotalFrame = avihheader->TotalFrame;     /* 得到总帧数 */
    buf += avihheader->BlockSize + 8;               /* 偏移 */
    listheader = (LIST_HEADER *)(buf);

    if (listheader->ListID != AVI_LIST_ID)return AVI_LIST_ERR;  /* LIST ID错误 */

    if (listheader->ListType != AVI_STRL_ID)return AVI_STRL_ERR;/* STRL ID错误 */

    strhheader = (STRH_HEADER *)(buf + 12);

    if (strhheader->BlockID != AVI_STRH_ID)return AVI_STRH_ERR; /* STRH ID错误 */

    if (strhheader->StreamType == AVI_VIDS_STREAM)              /* 视频帧在前 */
    {
        if (strhheader->Handler != AVI_FORMAT_MJPG)return AVI_FORMAT_ERR;   /* 非MJPG视频流,不支持 */

        g_avix.VideoFLAG = AVI_VIDS_FLAG_TBL[0];       /* 视频流标记  "00dc" */
        g_avix.AudioFLAG = AVI_AUDS_FLAG_TBL[1];       /* 音频流标记  "01wb" */
        bmpheader = (STRF_BMPHEADER *)(buf + 12 + strhheader->BlockSize + 8);   /* strf */

        if (bmpheader->BlockID != AVI_STRF_ID)return AVI_STRF_ERR;  /* STRF ID错误 */

        g_avix.Width = bmpheader->bmiHeader.Width;
        g_avix.Height = bmpheader->bmiHeader.Height;
        buf += listheader->BlockSize + 8;       /* 偏移 */
        listheader = (LIST_HEADER *)(buf);

        if (listheader->ListID != AVI_LIST_ID)  /* 是不含有音频帧的视频文件 */
        {
            g_avix.SampleRate = 0;    /* 音频采样率 */
            g_avix.Channels = 0;      /* 音频通道数 */
            g_avix.AudioType = 0;     /* 音频格式 */
        }
        else
        {
            if (listheader->ListType != AVI_STRL_ID)return AVI_STRL_ERR;    /* STRL ID错误 */

            strhheader = (STRH_HEADER *)(buf + 12);

            if (strhheader->BlockID != AVI_STRH_ID)return AVI_STRH_ERR;     /* STRH ID错误 */

            if (strhheader->StreamType != AVI_AUDS_STREAM)return AVI_FORMAT_ERR;    /* 格式错误 */

            wavheader = (STRF_WAVHEADER *)(buf + 12 + strhheader->BlockSize + 8);   /* strf */

            if (wavheader->BlockID != AVI_STRF_ID)return AVI_STRF_ERR;  /* STRF ID错误 */

            g_avix.SampleRate = wavheader->SampleRate;  /* 音频采样率 */
            g_avix.Channels = wavheader->Channels;      /* 音频通道数 */
            g_avix.AudioType = wavheader->FormatTag;    /* 音频格式 */
        }
    }
    else if (strhheader->StreamType == AVI_AUDS_STREAM) /* 音频帧在前 */
    {
        g_avix.VideoFLAG = AVI_VIDS_FLAG_TBL[1];        /* 视频流标记  "01dc" */
        g_avix.AudioFLAG = AVI_AUDS_FLAG_TBL[0];        /* 音频流标记  "00wb" */
        wavheader = (STRF_WAVHEADER *)(buf + 12 + strhheader->BlockSize + 8);   /* strf */

        if (wavheader->BlockID != AVI_STRF_ID)return AVI_STRF_ERR;  /* STRF ID错误 */

        g_avix.SampleRate = wavheader->SampleRate;      /* 音频采样率 */
        g_avix.Channels = wavheader->Channels;          /* 音频通道数 */
        g_avix.AudioType = wavheader->FormatTag;        /* 音频格式 */
        buf += listheader->BlockSize + 8;               /* 偏移 */
        listheader = (LIST_HEADER *)(buf);

        if (listheader->ListID != AVI_LIST_ID)return AVI_LIST_ERR;      /* LIST ID错误 */

        if (listheader->ListType != AVI_STRL_ID)return AVI_STRL_ERR;    /* STRL ID错误 */

        strhheader = (STRH_HEADER *)(buf + 12);

        if (strhheader->BlockID != AVI_STRH_ID)return AVI_STRH_ERR;     /* STRH ID错误 */

        if (strhheader->StreamType != AVI_VIDS_STREAM)return AVI_FORMAT_ERR;    /* 格式错误 */

        bmpheader = (STRF_BMPHEADER *)(buf + 12 + strhheader->BlockSize + 8);   /* strf */

        if (bmpheader->BlockID != AVI_STRF_ID)return AVI_STRF_ERR;      /* STRF ID错误 */

        if (bmpheader->bmiHeader.Compression != AVI_FORMAT_MJPG)return AVI_FORMAT_ERR;  /* 格式错误 */

        g_avix.Width = bmpheader->bmiHeader.Width;
        g_avix.Height = bmpheader->bmiHeader.Height;
    }

    offset = avi_srarch_id(tbuf, size, "movi");     /* 查找movi ID */

    if (offset == 0)return AVI_MOVI_ERR;            /* MOVI ID错误 */

    if (g_avix.SampleRate)    /* 有音频流,才查找 */
    {
        tbuf += offset;
        offset = avi_srarch_id(tbuf, size, g_avix.AudioFLAG); /* 查找音频流标记 */

        if (offset == 0)return AVI_STREAM_ERR;      /* 流错误 */

        tbuf += offset + 4;
        g_avix.AudioBufSize = *((uint16_t *)tbuf);    /* 得到音频流buf大小 */
    }

    printf("avi init ok\r\n");
    printf("g_avix.SecPerFrame:%d\r\n", g_avix.SecPerFrame);
    printf("g_avix.TotalFrame:%d\r\n", g_avix.TotalFrame);
    printf("g_avix.Width:%d\r\n", g_avix.Width);
    printf("g_avix.Height:%d\r\n", g_avix.Height);
    printf("g_avix.AudioType:%d\r\n", g_avix.AudioType);
    printf("g_avix.SampleRate:%d\r\n", g_avix.SampleRate);
    printf("g_avix.Channels:%d\r\n", g_avix.Channels);
    printf("g_avix.AudioBufSize:%d\r\n", g_avix.AudioBufSize);
    printf("g_avix.VideoFLAG:%s\r\n", g_avix.VideoFLAG);
    printf("g_avix.AudioFLAG:%s\r\n", g_avix.AudioFLAG);
    return res;
}

/**
 * @brief       查找 ID
 * @param       buf  : 输入缓冲区
 * @param       size : 缓冲区大小
 * @param       id   : 要查找的id, 必须是4字节长度
 * @retval      执行结果
 *   @arg       0     , 没找到
 *   @arg       其他  , movi ID偏移量
 */
uint32_t avi_srarch_id(uint8_t *buf, uint32_t size, char *id)
{
    uint32_t i;
    uint32_t idsize = 0;
    size -= 4;
    for (i = 0; i < size; i++)
    {
        if ((buf[i] == id[0]) &&
            (buf[i + 1] == id[1]) &&
            (buf[i + 2] == id[2]) &&
            (buf[i + 3] == id[3]))
        {
            idsize = MAKEDWORD(buf + i + 4);    /* 得到帧大小,必须大于16字节,才返回,否则不是有效数据 */

            if (idsize > 0X10)return i;         /* 找到"id"所在的位置 */
        }
    }

    return 0;
}

/**
 * @brief       得到stream流信息
 * @param       buf  : 流开始地址(必须是01wb/00wb/01dc/00dc开头)
 * @retval      执行结果
 *   @arg       AVI_OK, AVI文件解析成功
 *   @arg       其他  , 错误代码
 */
AVISTATUS avi_get_streaminfo(uint8_t *buf)
{
    g_avix.StreamID = MAKEWORD(buf + 2);          /* 得到流类型 */
    g_avix.StreamSize = MAKEDWORD(buf + 4);       /* 得到流大小 */

    if (g_avix.StreamSize > AVI_MAX_FRAME_SIZE)   /* 帧大小太大了,直接返回错误 */
    {
        printf("FRAME SIZE OVER:%d\r\n", g_avix.StreamSize);
        g_avix.StreamSize = 0;
        return AVI_STREAM_ERR;
    }

    if (g_avix.StreamSize % 2)g_avix.StreamSize++;  /* 奇数加1(g_avix.StreamSize,必须是偶数) */

    if (g_avix.StreamID == AVI_VIDS_FLAG || g_avix.StreamID == AVI_AUDS_FLAG)return AVI_OK;

    return AVI_STREAM_ERR;
}




















