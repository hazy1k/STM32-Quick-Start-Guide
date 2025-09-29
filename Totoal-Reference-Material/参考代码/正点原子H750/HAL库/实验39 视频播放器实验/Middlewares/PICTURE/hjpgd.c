/**
 ****************************************************************************************************
 * @file        hjpgd.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-05-23
 * @brief       驱动代码-jpeg硬件解码部分 代码
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
 * V1.0 20220523
 * 第一次发布
 *
 ****************************************************************************************************
 */
 
#include "./PICTURE/hjpgd.h"
#include "./PICTURE/piclib.h"
#include "./SYSTEM/usart/usart.h"


jpeg_codec_typedef hjpgd;       /* JPEG硬件解码结构体 */


/**
 * @brief       JPEG输入数据流回调函数
 *   @note      用于获取JPEG文件原始数据, 每当JPEG DMA IN BUF为空的时候,调用该函数
 * @param       无
 * @retval      无
 */
void jpeg_dma_in_callback(void)
{
    hjpgd.inbuf[hjpgd.inbuf_read_ptr].sta = 0;  /* 此buf已经处理完了 */
    hjpgd.inbuf[hjpgd.inbuf_read_ptr].size = 0; /* 此buf已经处理完了 */
    hjpgd.inbuf_read_ptr++;                     /* 指向下一个buf */

    if (hjpgd.inbuf_read_ptr >= JPEG_DMA_INBUF_NB)hjpgd.inbuf_read_ptr = 0; /* 归零 */

    if (hjpgd.inbuf[hjpgd.inbuf_read_ptr].sta == 0)   /* 无有效buf */
    {
        hjpgd.indma_pause = 1;  /* 标记暂停 */
    }
    else    /* 有效的buf */
    {
        /* 继续下一次DMA传输 */
        jpeg_in_dma_resume((uint32_t)hjpgd.inbuf[hjpgd.inbuf_read_ptr].buf, hjpgd.inbuf[hjpgd.inbuf_read_ptr].size);
    }
}

/**
 * @brief       JPEG输出数据流(YCBCR)回调函数
 *   @note      用于输出YCbCr数据流(YUV)
 * @param       无
 * @retval      无
 */
void jpeg_dma_out_callback(void)
{
    uint32_t *pdata = 0;
    hjpgd.outbuf[hjpgd.outbuf_write_ptr].sta = 1;   /* 此buf已满 */
    hjpgd.outbuf[hjpgd.outbuf_write_ptr].size = hjpgd.yuvblk_size - (MDMA_Channel6->CBNDTR & 0X1FFFF);  /* 此buf里面数据的长度 */

    if (hjpgd.state == JPEG_STATE_FINISHED) /* 如果文件已经解码完成,需要读取DOR最后的数据(<=32字节) */
    {
        pdata = (uint32_t *)(hjpgd.outbuf[hjpgd.outbuf_write_ptr].buf + hjpgd.outbuf[hjpgd.outbuf_write_ptr].size);

        while (JPEG->SR & (1 << 4))
        {
            *pdata = JPEG->DOR;
            pdata++;
            hjpgd.outbuf[hjpgd.outbuf_write_ptr].size += 4;
        }
    }

    hjpgd.outbuf_write_ptr++;   /* 指向下一个buf */

    if (hjpgd.outbuf_write_ptr >= JPEG_DMA_OUTBUF_NB)hjpgd.outbuf_write_ptr = 0;    /* 归零 */

    if (hjpgd.outbuf[hjpgd.outbuf_write_ptr].sta == 1)  /* 无有效buf */
    {
        hjpgd.outdma_pause = 1; /* 标记暂停 */
    }
    else    /* 有效的buf */
    {
        /* 继续下一次DMA传输 */
        jpeg_out_dma_resume((uint32_t)hjpgd.outbuf[hjpgd.outbuf_write_ptr].buf, hjpgd.yuvblk_size);

    }
}

/**
 * @brief       JPEG整个文件解码完成回调函数
 * @param       无
 * @retval      无
 */
void jpeg_endofcovert_callback(void)
{
    hjpgd.state = JPEG_STATE_FINISHED;  /* 标记JPEG解码完成 */
}

/**
 * @brief       JPEG header解析成功回调函数
 * @param       无
 * @retval      无
 */
void jpeg_hdrover_callback(void)
{
    uint8_t i = 0;
    hjpgd.state = JPEG_STATE_HEADEROK;  /* HEADER获取成功 */
    jpeg_get_info(&hjpgd);              /* 获取JPEG相关信息,包括大小,色彩空间,抽样等 */
    picinfo.ImgWidth = hjpgd.Conf.ImageWidth;
    picinfo.ImgHeight = hjpgd.Conf.ImageHeight;

    /* 需要获取JPEG基本信息以后,才能根据jpeg输出大小和采样方式,来计算输出缓冲大小,并启动输出MDMA */
    switch (hjpgd.Conf.ChromaSubsampling)
    {
        case JPEG_420_SUBSAMPLING:
            hjpgd.yuvblk_size = 24 * hjpgd.Conf.ImageWidth; /* YUV420,每个YUV像素占1.5个字节.每次输出16行.16*1.5=24 */
            hjpgd.yuvblk_height = 16;   /* 每次输出16行 */
            break;

        case JPEG_422_SUBSAMPLING:
            hjpgd.yuvblk_size = 16 * hjpgd.Conf.ImageWidth; /* YUV422,每个YUV像素占2个字节.每次输出8行.8*2=16 */
            hjpgd.yuvblk_height = 8;    /* 每次输出8行 */
            break;

        case JPEG_444_SUBSAMPLING:
            hjpgd.yuvblk_size = 24 * hjpgd.Conf.ImageWidth; /* YUV444,每个YUV像素占3个字节.每次输出8行.8*3=24 */
            hjpgd.yuvblk_height = 8;    /* 每次输出8行 */
            break;
    }

    hjpgd.yuvblk_curheight = 0; /* 当前行计数器清零 */

    for (i = 0; i < JPEG_DMA_OUTBUF_NB; i++)
    {
        hjpgd.outbuf[i].buf = mymalloc(SRAMIN, hjpgd.yuvblk_size + 32); /* 有可能会多需要32字节内存 */

        if (hjpgd.outbuf[i].buf == NULL)
        {
            hjpgd.state = JPEG_STATE_ERROR; /* HEADER获取失败 */
        }
    }

    if (hjpgd.outbuf[JPEG_DMA_OUTBUF_NB - 1].buf != NULL)   /* 所有buf都申请OK */
    {
        jpeg_out_dma_init((uint32_t)hjpgd.outbuf[0].buf, hjpgd.yuvblk_size);/* 配置输出DMA */
        jpeg_out_dma_start();   /* 启动DMA OUT传输,开始接收JPEG解码数据流 */
    }

    piclib_ai_draw_init();
}

/**
 * @brief       JPEG硬件解码图片
 *   @note      注意, 请保证:
 *              1, 待解吗图片的分辨率,必须小于等于屏幕的分辨率!
 *              2, 请保证图片的宽度是16的倍数,否则解码出错!
 *
 * @param       filename : 包含路径的文件名(.jpeg/jpg)
 * @retval      操作结果
 *   @arg       0   , 成功
 *   @arg       其他, 错误码
 */
uint8_t hjpgd_decode(char *filename)
{
    FIL *ftemp;
    uint16_t *rgb565buf = 0;
    volatile  uint32_t timecnt = 0;
    uint32_t br = 0;
    uint8_t fileover = 0;
    uint8_t i = 0;
    uint8_t res;
    res = jpeg_core_init(&hjpgd);   /* 初始化JPEG内核 */

    if (res)return 1;

    ftemp = (FIL *)mymalloc(SRAMITCM, sizeof(FIL)); /* 申请内存 */

    if (f_open(ftemp, filename, FA_READ) != FR_OK)  /* 打开图片失败 */
    {
        jpeg_core_destroy(&hjpgd);
        myfree(SRAMITCM, ftemp);    /* 释放内存 */
        return 2;
    }

    jpeg_decode_init(&hjpgd);       /* 初始化硬件JPEG解码器 */

    for (i = 0; i < JPEG_DMA_INBUF_NB; i++)
    {
        res = f_read(ftemp, hjpgd.inbuf[i].buf, JPEG_DMA_INBUF_LEN, &br); /* 填满所有输入数据缓冲区 */

        if (res == FR_OK && br)
        {
            hjpgd.inbuf[i].size = br;   /* 读取 */
            hjpgd.inbuf[i].sta = 1;     /* 标记buf满 */
        }

        if (br == 0)break;
    }

    jpeg_in_dma_init((uint32_t)hjpgd.inbuf[0].buf, hjpgd.inbuf[0].size);/* 配置输入DMA */
    jpeg_in_callback = jpeg_dma_in_callback;        /* JPEG DMA读取数据回调函数 */
    jpeg_out_callback = jpeg_dma_out_callback;      /* JPEG DMA输出数据回调函数 */
    jpeg_eoc_callback = jpeg_endofcovert_callback;  /* JPEG 解码结束回调函数 */
    jpeg_hdp_callback = jpeg_hdrover_callback;      /* JPEG Header解码完成回调函数 */
    jpeg_in_dma_start();    /* 启动DMA IN传输,开始解码JPEG图片 */

    while (1)
    {
        if (rgb565buf == 0 && hjpgd.state == JPEG_STATE_HEADEROK)   /* rgb565buf空,且JPEG HEAD解码完成 */
        {
            rgb565buf = mymalloc(SRAMIN, hjpgd.Conf.ImageWidth * hjpgd.yuvblk_height * 2 + 32); /* 申请单次输出缓冲内存 */
        }

        if (hjpgd.inbuf[hjpgd.inbuf_write_ptr].sta == 0 && fileover == 0)   /* 有buf为空 */
        {
            res = f_read(ftemp, hjpgd.inbuf[hjpgd.inbuf_write_ptr].buf, JPEG_DMA_INBUF_LEN, &br);   /* 填满一个缓冲区 */

            if (res == FR_OK && br)
            {
                hjpgd.inbuf[hjpgd.inbuf_write_ptr].size = br;   /* 读取 */
                hjpgd.inbuf[hjpgd.inbuf_write_ptr].sta = 1;     /* buf满 */
            }
            else if (br == 0)
            {
                timecnt = 0;    /* 清零计时器 */
                fileover = 1;   /* 文件结束了 */
            }

            if (hjpgd.indma_pause == 1 && hjpgd.inbuf[hjpgd.inbuf_read_ptr].sta == 1)   /* 之前是暂停的了,继续传输 */
            {
                jpeg_in_dma_resume((uint32_t)hjpgd.inbuf[hjpgd.inbuf_read_ptr].buf, hjpgd.inbuf[hjpgd.inbuf_read_ptr].size);    /* 继续下一次DMA传输 */
                hjpgd.indma_pause = 0;
            }

            hjpgd.inbuf_write_ptr++;

            if (hjpgd.inbuf_write_ptr >= JPEG_DMA_INBUF_NB)hjpgd.inbuf_write_ptr = 0;
        }

        if (hjpgd.outbuf[hjpgd.outbuf_read_ptr].sta == 1)   /* buf里面有数据要处理 */
        {
            SCB_CleanInvalidateDCache();    /* 清空D catch */
            jpeg_dma2d_yuv2rgb_conversion(&hjpgd, (uint32_t *)rgb565buf);   /* 利用DMA2D,将YUV图像转成RGB565图像 */
            
            /* 输出到LCD屏幕上面 */
            pic_phy.fillcolor(picinfo.S_XOFF, picinfo.S_YOFF + hjpgd.yuvblk_curheight, hjpgd.Conf.ImageWidth, hjpgd.yuvblk_height, rgb565buf);
            hjpgd.yuvblk_curheight += hjpgd.yuvblk_height; /* 列偏移 */

            //SCB_CleanInvalidateDCache();                  /* 清空D catch */
            hjpgd.outbuf[hjpgd.outbuf_read_ptr].sta = 0;    /* 标记buf为空 */
            hjpgd.outbuf[hjpgd.outbuf_read_ptr].size = 0;   /* 数据量清空 */
            hjpgd.outbuf_read_ptr++;

            if (hjpgd.outbuf_read_ptr >= JPEG_DMA_OUTBUF_NB)hjpgd.outbuf_read_ptr = 0;  /* 限制范围 */

            if (hjpgd.yuvblk_curheight >= hjpgd.Conf.ImageHeight)break; /* 当前高度等于或者超过图片分辨率的高度,则说明解码完成了,直接退出 */
        }
        else if (hjpgd.outdma_pause == 1 && hjpgd.outbuf[hjpgd.outbuf_write_ptr].sta == 0)  /* out暂停,且当前writebuf已经为空了,则恢复out输出 */
        {
            jpeg_out_dma_resume((uint32_t)hjpgd.outbuf[hjpgd.outbuf_write_ptr].buf, hjpgd.yuvblk_size); /* 继续下一次DMA传输 */
            hjpgd.outdma_pause = 0;
        }

        timecnt++;

        if (hjpgd.state == JPEG_STATE_ERROR)    /* 解码出错,直接退出 */
        {
            res = 2;
            break;
        }

        if (fileover)   /* 文件结束后,及时退出,防止死循环 */
        {
            if (hjpgd.state == JPEG_STATE_NOHEADER && hjpgd.indma_pause == 1)   /* 当前处于暂停状态，且没有解析到JPEG头 */
            {
                break;  /* 解码JPEG头失败了 */
            }

            if (timecnt > 0X3FFFF)break;    /* 超时退出 */
        }
    }

    f_close(ftemp);             /* 关闭文件 */
    myfree(SRAMITCM, ftemp);    /* 释放申请的内存 */
    myfree(SRAMIN, rgb565buf);  /* 释放内存 */
    jpeg_core_destroy(&hjpgd);  /* 结束JPEG解码,释放内存 */
    return res;
}











