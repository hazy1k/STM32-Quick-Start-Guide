/**
 ****************************************************************************************************
 * @file        jpegcodec.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-04
 * @brief       JPEG硬件编解码器 驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 STM32H750开发板
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

#ifndef __JPEGCODEC_H
#define __JPEGCODEC_H

#include "./SYSTEM/sys/sys.h"


#define JPEG_DMA_INBUF_LEN          4096                /* 单个MDMA IN  BUF的大小 */
#define JPEG_DMA_INBUF_NB           10                  /* MDMA IN  BUF的个数 */
#define JPEG_DMA_OUTBUF_NB          2                   /* MDMA OUT BUF的个数 */


#define JPEG_QUANT_TABLE_SIZE       ((uint32_t)64U)     /* JPEG Quantization Table Size */

/* JPEG数据缓冲结构体 */
typedef struct
{
    uint8_t sta;        /* 状态:0,无数据;1,有数据 */
    char *buf;          /* JPEG数据缓冲区 */
    uint16_t size;      /* JPEG数据长度 */
} jpeg_databuf_type;

#define JPEG_STATE_NOHEADER         0                   /* HEADER未读取,初始状态 */
#define JPEG_STATE_HEADEROK         1                   /* HEADER读取成功 */
#define JPEG_STATE_FINISHED         2                   /* 解码完成 */
#define JPEG_STATE_ERROR            3                   /* 解码错误 */

#define JPEG_GRAYSCALE_COLORSPACE   ((uint32_t)0x00000000U)
#define JPEG_YCBCR_COLORSPACE       JPEG_CONFR1_COLORSPACE_0
#define JPEG_CMYK_COLORSPACE        JPEG_CONFR1_COLORSPACE

#define JPEG_444_SUBSAMPLING        ((uint32_t)0x00000000U)     /* Chroma Subsampling 4:4:4 */
#define JPEG_420_SUBSAMPLING        ((uint32_t)0x00000001U)     /* Chroma Subsampling 4:2:0 */
#define JPEG_422_SUBSAMPLING        ((uint32_t)0x00000002U)     /* Chroma Subsampling 4:2:2 */

#define DMA2D_NO_CSS                ((uint32_t)0x00000000U)     /* No chroma sub-sampling 4:4:4 */
#define DMA2D_CSS_422               ((uint32_t)0x00000001U)     /* chroma sub-sampling 4:2:2 */
#define DMA2D_CSS_420               ((uint32_t)0x00000002U)     /* chroma sub-sampling 4:2:0 */


/* JPEG文件信息结构体 */
typedef struct
{
    uint8_t  ColorSpace;            /* 图像的颜色空间: gray-scale/YCBCR/RGB/CMYK */
    uint8_t  ChromaSubsampling;     /* YCBCR/CMYK颜色空间的色度抽样情况:0,4:4:4;1,4:2:2;2,4:1:1;3,4:2:0 */
    uint32_t ImageHeight;           /* 图像高度 */
    uint32_t ImageWidth;            /* 图像宽度 */
    uint8_t  ImageQuality;          /* 图像编码质量:1~100 */
} JPEG_ConfTypeDef;


/* jpeg编解码控制结构体 */
typedef struct
{
    JPEG_ConfTypeDef	Conf;                       /* 当前JPEG文件相关参数 */
    jpeg_databuf_type inbuf[JPEG_DMA_INBUF_NB];     /* MDMA IN buf */
    jpeg_databuf_type outbuf[JPEG_DMA_OUTBUF_NB];   /* MDMA OUT buf */
    volatile uint8_t inbuf_read_ptr;                /* MDMA IN buf当前读取位置 */
    volatile uint8_t inbuf_write_ptr;               /* MDMA IN buf当前写入位置 */
    volatile uint8_t indma_pause;                   /* 输入MDMA暂停状态标识 */
    volatile uint8_t outbuf_read_ptr;               /* MDMA OUT buf当前读取位置 */
    volatile uint8_t outbuf_write_ptr;              /* MDMA OUT buf当前写入位置 */
    volatile uint8_t outdma_pause;                  /* 输入MDMA暂停状态标识 */
    volatile uint8_t state;                         /* 解码状态;0,未识别到Header;1,识别到了Header;2,解码完成; */
    uint32_t yuvblk_size;                           /* YUV输出的字节数,使得完成一次DMA2D YUV2RGB转换,刚好是图片宽度的整数倍
                                                     * YUV420图片,每个像素占1.5个YUV字节,每次输出16行,yuvblk_size=图片宽度*16*1.5
                                                     * YUV422图片,每个像素占2个YUV字节和RGB565一样,每次输出8行,yuvblk_size=图片宽度*8*2
                                                     * YUV444图片,每个像素占3个YUV字节,每次输出8行,yuvblk_size=图片宽度*8*3
                                                     */
                                                    
    uint16_t yuvblk_height;                         /* 每个YUV块输出像素的高度,对于YUV420,为16,对于YUV422/YUV444为8 */
    uint16_t yuvblk_curheight;                      /* 当前输出高度,0~分辨率高度 */
} jpeg_codec_typedef;

/* MDMA回调函数(需要外部实现) */
extern void (*jpeg_in_callback)(void);              /* JPEG MDMA输入回调函数 */
extern void (*jpeg_out_callback)(void);             /* JPEG MDMA输出 回调函数 */
extern void (*jpeg_eoc_callback)(void);             /* JPEG 解码完成 回调函数 */
extern void (*jpeg_hdp_callback)(void);             /* JPEG Header解码完成 回调函数 */


/* 接口函数 */

void jpeg_dma_stop(void);
void jpeg_in_dma_start(void);
void jpeg_out_dma_start(void);
void jpeg_in_dma_resume(uint32_t memaddr, uint32_t memlen);
void jpeg_out_dma_resume(uint32_t memaddr, uint32_t memlen);
void jpeg_in_dma_init(uint32_t meminaddr, uint32_t meminsize);
void jpeg_out_dma_init(uint32_t memoutaddr, uint32_t memoutsize);

uint8_t jpeg_get_quality(void);
void jpeg_get_info(jpeg_codec_typedef *tjpeg);
void jpeg_decode_init(jpeg_codec_typedef *tjpeg);
void jpeg_core_destroy(jpeg_codec_typedef *tjpeg);
uint8_t jpeg_core_init(jpeg_codec_typedef *tjpeg);
uint8_t jpeg_dma2d_yuv2rgb_conversion(jpeg_codec_typedef *tjpeg, uint32_t *pdst);

#endif




















