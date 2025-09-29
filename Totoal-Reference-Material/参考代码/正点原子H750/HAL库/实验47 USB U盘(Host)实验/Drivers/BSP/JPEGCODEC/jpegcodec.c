/**
 ****************************************************************************************************
 * @file        jpegcodec.c
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

#include "./MALLOC/malloc.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/JPEGCODEC/jpegcodec.h"


/* JPEG规范(ISO/IEC 10918-1标准)的样本量化表
 * 获取JPEG图片质量时需要用到
 */
const uint8_t JPEG_LUM_QuantTable[JPEG_QUANT_TABLE_SIZE] =
{
    16, 11, 10, 16, 24, 40, 51, 61, 12, 12, 14, 19, 26, 58, 60, 55,
    14, 13, 16, 24, 40, 57, 69, 56, 14, 17, 22, 29, 51, 87, 80, 62,
    18, 22, 37, 56, 68, 109, 103, 77, 24, 35, 55, 64, 81, 104, 113, 92,
    49, 64, 78, 87, 103, 121, 120, 101, 72, 92, 95, 98, 112, 100, 103, 99
};

const uint8_t JPEG_ZIGZAG_ORDER[JPEG_QUANT_TABLE_SIZE] =
{
    0, 1, 8, 16, 9, 2, 3, 10, 17, 24, 32, 25, 18, 11, 4, 5,
    12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13, 6, 7, 14, 21, 28,
    35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37, 44, 51,
    58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54, 47, 55, 62, 63
};

/**
 * @brief       JPEG硬件解码输入MDMA配置
 * @param       meminaddr   : JPEG输入MDMA存储器地址
 * @param       meminsize   : 输入MDMA数据长度,0~262143,以字节为单位
 * @retval      无
 */
void jpeg_in_dma_init(uint32_t meminaddr, uint32_t meminsize)
{
    uint32_t regval = 0;
    uint32_t addrmask = 0;
    RCC->AHB3ENR |= 1 << 0;             /* 使能MDMA时钟 */
    MDMA_Channel7->CCR = 0;             /* 输入MDMA清零 */

    while (MDMA_Channel7->CCR & 0X01);  /* 等待MDMA_Channel7关闭完成 */

    MDMA_Channel7->CIFCR = 0X1F;        /* 中断标志清零 */
    MDMA_Channel7->CCR |= 1 << 2;       /* CTCIE=1,使能通道传输完成中断 */
    MDMA_Channel7->CCR |= 2 << 6;       /* PL[1:0]=2,高优先级 */
    MDMA_Channel7->CBNDTR = meminsize;  /* 传输长度为meminsize */
    MDMA_Channel7->CDAR = (uint32_t)&JPEG->DIR; /* 目标地址为:JPEG->DIR */
    MDMA_Channel7->CSAR = meminaddr;    /* meminaddr作为源地址 */
    regval = 0 << 28;                   /* TRGM[1:0]=0,每个MDMA请求触发一次buffer传输 */
    regval |= 1 << 25;                  /* PKE=1,打包使能 */
    regval |= (32 - 1) << 18;           /* TLEN[6:0]=31,buffer传输长度为32字节 */
    regval |= 4 << 15;                  /* DBURST[2:0]=4,目标突发传输长度为16 */
    regval |= 4 << 12;                  /* SBURST[2:0]=4,源突发传输长度为16 */
    regval |= 0 << 8;                   /* SINCOS[1:0]=0,源地址变化单位为8位(字节) */
    regval |= 2 << 6;                   /* DSIZE[1:0]=2,目标位宽为32位 */
    regval |= 0 << 4;                   /* SSIZE[1:0]=0,源位宽为8位 */
    regval |= 0 << 2;                   /* DINC[1:0]=0,目标地址固定 */
    regval |= 2 << 0;                   /* SINC[1:0]=2,源地址自增 */
    MDMA_Channel7->CTCR = regval;       /* 设置CTCR寄存器 */
    MDMA_Channel7->CTBR = 17 << 0;      /* MDMA的硬件触发通道17触发inmdma,通道17=JPEG input FIFO threshold
                                         * 详见<STM32H7xx参考手册>577页,table 95
                                         */
    addrmask = meminaddr & 0XFF000000;  /* 获取掩码 */

    if (addrmask == 0X20000000 || addrmask == 0)MDMA_Channel7->CTBR |= 1 << 16; /* 使用AHBS总线访问DTCM/ITCM */

    HAL_NVIC_SetPriority(MDMA_IRQn,2,3);/* 设置中断优先级，抢占优先级2，子优先级3 */
    HAL_NVIC_EnableIRQ(MDMA_IRQn);      /* 开启MDMA中断 */
}

/**
 * @brief       JPEG硬件解码输出MDMA配置
 * @param       memoutaddr  : JPEG输出MDMA存储器地址
 * @param       memoutsize  : 输出MDMA数据长度,0~262143,以字节为单位
 * @retval      无
 */
void jpeg_out_dma_init(uint32_t memoutaddr, uint32_t memoutsize)
{
    uint32_t regval = 0;
    uint32_t addrmask = 0;
    RCC->AHB3ENR |= 1 << 0;             /* 使能MDMA时钟 */
    MDMA_Channel6->CCR = 0;             /* 输出MDMA清零 */

    while (MDMA_Channel6->CCR & 0X01);  /* 等待MDMA_Channel6关闭完成 */

    MDMA_Channel6->CIFCR = 0X1F;        /* 中断标志清零 */
    MDMA_Channel6->CCR |= 3 << 6;       /* PL[1:0]=2,最高优先级 */
    MDMA_Channel6->CCR |= 1 << 2;       /* CTCIE=1,使能通道传输完成中断 */
    MDMA_Channel6->CBNDTR = memoutsize; /* 传输长度为meminsize */
    MDMA_Channel6->CDAR = memoutaddr;   /* 目标地址为:memoutaddr */
    MDMA_Channel6->CSAR = (uint32_t)&JPEG->DOR; /* JPEG->DOR作为源地址 */
    regval = 0 << 28;                   /* TRGM[1:0]=0,每个MDMA请求触发一次buffer传输 */
    regval |= 1 << 25;                  /* PKE=1,打包使能 */
    regval |= (32 - 1) << 18;           /* TLEN[6:0]=31,buffer传输长度为32字节 */
    regval |= 4 << 15;                  /* DBURST[2:0]=4,目标突发传输长度为16 */
    regval |= 4 << 12;                  /* SBURST[2:0]=4,源突发传输长度为16 */
    regval |= 0 << 10;                  /* DINCOS[1:0]=0,目标地址变化单位为8位(字节) */
    regval |= 0 << 6;                   /* DSIZE[1:0]=0,目标位宽为8位 */
    regval |= 2 << 4;                   /* SSIZE[1:0]=2,源位宽为32位 */
    regval |= 2 << 2;                   /* DINC[1:0]=2,目标地址自增 */
    regval |= 0 << 0;                   /* SINC[1:0]=0,源地址固定 */
    MDMA_Channel6->CTCR = regval;       /* 设置CTCR寄存器 */
    MDMA_Channel6->CTBR = 19 << 0;      /* MDMA的硬件触发通道19触发outmdma,通道19=JPEG output FIFO threshold */
    /* 详见<STM32H7xx参考手册>577页,table 95 */
    addrmask = memoutaddr & 0XFF000000; /* 获取掩码 */

    if (addrmask == 0X20000000 || addrmask == 0)MDMA_Channel6->CTBR |= 1 << 17; /* 使用AHBS总线访问DTCM/ITCM */

    HAL_NVIC_SetPriority(MDMA_IRQn,2,3);/* 设置中断优先级，抢占优先级2，子优先级3 */
    HAL_NVIC_EnableIRQ(MDMA_IRQn);      /* 开启MDMA中断 */
}

void (*jpeg_in_callback)(void);         /* JPEG MDMA输入回调函数 */
void (*jpeg_out_callback)(void);        /* JPEG MDMA输出 回调函数 */
void (*jpeg_eoc_callback)(void);        /* JPEG 解码完成 回调函数 */
void (*jpeg_hdp_callback)(void);        /* JPEG Header解码完成 回调函数 */

/**
 * @brief       MDMA中断服务函数
 *   @note      处理硬件JPEG解码时输入/输出数据流
 * @param       无
 * @retval      无
 */
void MDMA_IRQHandler(void)
{
    if (MDMA_Channel7->CISR & (1 << 1))     /* CTCIF,通道7传输完成(输入) */
        if (MDMA_Channel7->CISR & (1 << 1)) /* CTCIF,通道7传输完成(输入) */
        {
            MDMA_Channel7->CIFCR |= 1 << 1; /* 清除通道传输完成中断 */
            JPEG->CR &= ~(0X7E);            /* 关闭JPEG中断,防止被打断 */
            jpeg_in_callback();             /* 执行输入回调函数,继续读取数据 */
            JPEG->CR |= 3 << 5;             /* 使能EOC和HPD中断 */
        }

    if (MDMA_Channel6->CISR & (1 << 1))     /* CTCIF,通道6传输完成(输出) */
    {
        MDMA_Channel6->CIFCR |= 1 << 1;     /* 清除通道传输完成中断 */
        JPEG->CR &= ~(0X7E);                /* 关闭JPEG中断,防止被打断 */
        jpeg_out_callback();                /* 执行输出回调函数,将数据转换成RGB */
        JPEG->CR |= 3 << 5;                 /* 使能EOC和HPD中断 */
    }
}

/**
 * @brief       JPEG解码中断服务函数
 * @param       无
 * @retval      无
 */
void JPEG_IRQHandler(void)
{
    if (JPEG->SR & (1 << 6))    /* JPEG Header解码完成 */
    {
        jpeg_hdp_callback();
        JPEG->CR &= ~(1 << 6);  /* 禁止Jpeg Header解码完成中断 */
        JPEG->CFR |= 1 << 6;    /* 清除HPDF位(header解码完成位) */
    }

    if (JPEG->SR & (1 << 5))    /* JPEG解码完成 */
    {
        jpeg_dma_stop();
        jpeg_eoc_callback();
        JPEG->CFR |= 1 << 5;    /* 清除EOC位(解码完成位) */
        MDMA_Channel6->CCR &= ~(1 << 0);    /* 关闭MDMA通道6 */
        MDMA_Channel7->CCR &= ~(1 << 0);    /* 关闭MDMA通道7 */
    }
}

/**
 * @brief       初始化硬件JPEG内核
 * @param       tjpeg       : JPEG编解码控制结构体
 * @retval      0, 成功; 1, 失败;
 */
uint8_t jpeg_core_init(jpeg_codec_typedef *tjpeg)
{
    uint8_t i;
    RCC->AHB3ENR |= 1 << 5;             /* 使能硬件jpeg时钟 */

    for (i = 0; i < JPEG_DMA_INBUF_NB; i++)
    {
        tjpeg->inbuf[i].buf = mymalloc(SRAMDTCM, JPEG_DMA_INBUF_LEN);

        if (tjpeg->inbuf[i].buf == NULL)
        {
            jpeg_core_destroy(tjpeg);
            return 1;
        }
    }

    JPEG->CR = 0;                       /* 先清零 */
    JPEG->CR |= 1 << 0;                 /* 使能硬件JPEG */
    JPEG->CONFR0 &= ~(1 << 0);          /* 停止JPEG编解码进程 */
    JPEG->CR |= 1 << 13;                /* 清空输入fifo */
    JPEG->CR |= 1 << 14;                /* 清空输出fifo */
    JPEG->CFR = 3 << 5;                 /* 清空标志 */
    HAL_NVIC_SetPriority(JPEG_IRQn,1,3);/* 设置中断优先级，抢占优先级1，子优先级3 */
    HAL_NVIC_EnableIRQ(JPEG_IRQn);      /* 开启JPEG中断 */
    JPEG->CONFR1 |= 1 << 8;             /* 使能header处理 */
    return 0;
}

/**
 * @brief       关闭硬件JPEG内核,并释放内存
 * @param       tjpeg       : JPEG编解码控制结构体
 * @retval      无
 */
void jpeg_core_destroy(jpeg_codec_typedef *tjpeg)
{
    uint8_t i;
    jpeg_dma_stop();    /* 停止MDMA传输 */

    for (i = 0; i < JPEG_DMA_INBUF_NB; i++)
    {
        myfree(SRAMDTCM, tjpeg->inbuf[i].buf);  /* 释放内存 */
    }
    for (i = 0; i < JPEG_DMA_OUTBUF_NB; i++)
    {
        myfree(SRAMIN, tjpeg->outbuf[i].buf);   /* 释放内存 */
    }
}

/**
 * @brief       初始化硬件JPEG解码器
 * @param       tjpeg       : JPEG编解码控制结构体
 * @retval      无
 */
void jpeg_decode_init(jpeg_codec_typedef *tjpeg)
{
    uint8_t i;
    tjpeg->inbuf_read_ptr = 0;
    tjpeg->inbuf_write_ptr = 0;
    tjpeg->indma_pause = 0;
    tjpeg->outbuf_read_ptr = 0;
    tjpeg->outbuf_write_ptr = 0;
    tjpeg->outdma_pause = 0;
    tjpeg->state = JPEG_STATE_NOHEADER; /* 图片解码结束标志 */

    for (i = 0; i < JPEG_DMA_INBUF_NB; i++)
    {
        tjpeg->inbuf[i].sta = 0;
        tjpeg->inbuf[i].size = 0;
    }

    for (i = 0; i < JPEG_DMA_OUTBUF_NB; i++)
    {
        tjpeg->outbuf[i].sta = 0;
        tjpeg->outbuf[i].size = 0;
    }

    MDMA_Channel6->CCR = 0;         /* MDMA通道6禁止 */
    MDMA_Channel7->CCR = 0;         /* MDMA通道7禁止 */
    MDMA_Channel6->CIFCR = 0X1F;    /* 中断标志清零 */
    MDMA_Channel7->CIFCR = 0X1F;    /* 中断标志清零 */

    JPEG->CONFR1 |= 1 << 3;         /* 硬件JPEG解码模式 */
    JPEG->CONFR0 &= ~(1 << 0);      /* 停止JPEG编解码进程 */
    JPEG->CR &= ~(0X3F << 1);       /* 关闭所有中断 */
    JPEG->CR |= 1 << 13;            /* 清空输入fifo */
    JPEG->CR |= 1 << 14;            /* 清空输出fifo */
    JPEG->CR |= 1 << 6;             /* 使能Jpeg Header解码完成中断 */
    JPEG->CR |= 1 << 5;             /* 使能解码完成中断 */
    JPEG->CFR = 3 << 5;             /* 清空标志 */
    JPEG->CONFR0 |= 1 << 0;         /* 使能JPEG编解码进程 */
}

/**
 * @brief       启动 jpeg in mdma, 开始解码JPEG
 * @param       无
 * @retval      无
 */
void jpeg_in_dma_start(void)
{
    MDMA_Channel7->CCR |= 1 << 0;   /* 使能MDMA通道7的传输 */
}

/**
 * @brief       启动 jpeg out mdma, 开始输出YUV数据
 * @param       无
 * @retval      无
 */
void jpeg_out_dma_start(void)
{
    MDMA_Channel6->CCR |= 1 << 0;   /* 使能MDMA通道6的传输 */
}

/**
 * @brief       停止JPEG MDMA解码过程
 * @param       无
 * @retval      无
 */
void jpeg_dma_stop(void)
{
    JPEG->CONFR0 &= ~(1 << 0);      /* 停止JPEG编解码进程 */
    JPEG->CR &= ~(0X3F << 1);       /* 关闭所有中断 */
    JPEG->CFR = 3 << 5;             /* 清空标志 */
}

/**
 * @brief       恢复MDMA IN过程
 * @param       memaddr     : 存储区首地址
 * @param       memlen      : 要传输数据长度(以字节为单位)
 * @retval      无
 */
void jpeg_in_dma_resume(uint32_t memaddr, uint32_t memlen)
{
    if (memlen % 4)memlen += 4 - memlen % 4;    /* 扩展到4的倍数 */

    MDMA_Channel7->CIFCR = 0X1F;    /* 中断标志清零 */
    MDMA_Channel7->CBNDTR = memlen; /* 传输长度为memlen */
    MDMA_Channel7->CSAR = memaddr;  /* memaddr作为源地址 */
    MDMA_Channel7->CCR |= 1 << 0;   /* 使能MDMA通道7的传输 */
}

/**
 * @brief       恢复MDMA OUT过程
 * @param       memaddr     : 存储区首地址
 * @param       memlen      : 要传输数据长度(以字节为单位)
 * @retval      无
 */
void jpeg_out_dma_resume(uint32_t memaddr, uint32_t memlen)
{
    if (memlen % 4)memlen += 4 - memlen % 4;    /* 扩展到4的倍数 */

    MDMA_Channel6->CIFCR = 0X1F;    /* 中断标志清零 */
    MDMA_Channel6->CBNDTR = memlen; /* 传输长度为memlen */
    MDMA_Channel6->CDAR = memaddr;  /* memaddr作为源地址 */
    MDMA_Channel6->CCR |= 1 << 0;   /* 使能MDMA通道6的传输 */
}

/**
 * @brief       获取图像信息
 * @param       tjpeg       : JPEG编解码控制结构体
 * @retval      无
 */
void jpeg_get_info(jpeg_codec_typedef *tjpeg)
{
    uint32_t yblockNb, cBblockNb, cRblockNb;

    switch (JPEG->CONFR1 & 0X03)
    {
        case 0:/* grayscale,1 color component */
            tjpeg->Conf.ColorSpace = JPEG_GRAYSCALE_COLORSPACE;
            break;

        case 2:/* YUV/RGB,3 color component */
            tjpeg->Conf.ColorSpace = JPEG_YCBCR_COLORSPACE;
            break;

        case 3:/* CMYK,4 color component */
            tjpeg->Conf.ColorSpace = JPEG_CMYK_COLORSPACE;
            break;
    }

    tjpeg->Conf.ImageHeight = (JPEG->CONFR1 & 0XFFFF0000) >> 16;    /* 获得图像高度 */
    tjpeg->Conf.ImageWidth = (JPEG->CONFR3 & 0XFFFF0000) >> 16;     /* 获得图像宽度 */

    if ((tjpeg->Conf.ColorSpace == JPEG_YCBCR_COLORSPACE) || (tjpeg->Conf.ColorSpace == JPEG_CMYK_COLORSPACE))
    {
        yblockNb  = (JPEG->CONFR4 & (0XF << 4)) >> 4;
        cBblockNb = (JPEG->CONFR5 & (0XF << 4)) >> 4;
        cRblockNb = (JPEG->CONFR6 & (0XF << 4)) >> 4;

        if ((yblockNb == 1) && (cBblockNb == 0) && (cRblockNb == 0))
        {
            tjpeg->Conf.ChromaSubsampling = JPEG_422_SUBSAMPLING;   /* 16x8 block */
        }
        else if ((yblockNb == 0) && (cBblockNb == 0) && (cRblockNb == 0))
        {
            tjpeg->Conf.ChromaSubsampling = JPEG_444_SUBSAMPLING;
        }
        else if ((yblockNb == 3) && (cBblockNb == 0) && (cRblockNb == 0))
        {
            tjpeg->Conf.ChromaSubsampling = JPEG_420_SUBSAMPLING;
        }
        else
        {
            tjpeg->Conf.ChromaSubsampling = JPEG_444_SUBSAMPLING;
        }
    }
    else
    {
        tjpeg->Conf.ChromaSubsampling = JPEG_444_SUBSAMPLING;   /* 默认用4:4:4 */
    }
    
    tjpeg->Conf.ImageQuality = 0;   /* 图像质量参数在整个图片的最末尾,刚开始的时候,是无法获取的,所以直接设置为0 */
}

/**
 * @brief       得到JPEG图像质量
 *   @note      在解码完成后,可以调用并获得正确的结果.
 * @param       无
 * @retval      图像质量, 0~100
 */
uint8_t jpeg_get_quality(void)
{
    uint32_t quality = 0;
    uint32_t quantRow, quantVal, scale, i, j;
    uint32_t *tableAddress = (uint32_t *)JPEG->QMEM0;
    i = 0;

    while (i < JPEG_QUANT_TABLE_SIZE)
    {
        quantRow = *tableAddress;

        for (j = 0; j < 4; j++)
        {
            quantVal = (quantRow >> (8 * j)) & 0xFF;

            if (quantVal == 1)quality += 100;   /* 100% */
            else
            {
                scale = (quantVal * 100) / ((uint32_t)JPEG_LUM_QuantTable[JPEG_ZIGZAG_ORDER[i + j]]);

                if (scale <= 100)
                {
                    quality += (200 - scale) / 2;
                }
                else
                {
                    quality += 5000 / scale;
                }
            }
        }

        i += 4;
        tableAddress++;
    }

    return (quality / ((uint32_t)64));
}

/**
 * @brief       将YUV数据转换成RGB数据
 *   @note      利用DMA2D, 将JPEG解码的YUV数据转换成RGB数据, 全硬件完成, 速度非常快
 * @param       tjpeg       : JPEG编解码控制结构体
 * @param       pdst        : 输出数组首地址
 * @retval      0, 成功; 1, 超时,失败;
 */
uint8_t jpeg_dma2d_yuv2rgb_conversion(jpeg_codec_typedef *tjpeg, uint32_t *pdst)
{
    uint32_t regval = 0;
    uint32_t cm = 0;            /* 采样方式n */
    uint32_t timeout = 0;

    if (tjpeg->Conf.ChromaSubsampling == JPEG_420_SUBSAMPLING)
    {
        cm = DMA2D_CSS_420;     /* YUV420转RGB */
    }
    else if (tjpeg->Conf.ChromaSubsampling == JPEG_422_SUBSAMPLING)
    {
        cm = DMA2D_CSS_422;     /* YUV422转RGB */
    }
    else if (tjpeg->Conf.ChromaSubsampling == JPEG_444_SUBSAMPLING)
    {
        cm = DMA2D_NO_CSS;      /* YUV444转RGB */
    }
    
    RCC->AHB3ENR |= 1 << 4;     /* 使能DMA2D时钟 */
    RCC->AHB3RSTR |= 1 << 4;    /* 复位DMA2D */
    RCC->AHB3RSTR &= ~(1 << 4); /* 结束复位 */
    DMA2D->CR &= ~(1 << 0);     /* 先停止DMA2D */
    DMA2D->CR = 1 << 16;        /* MODE[2:0]=001,存储器到存储器,带PFC模式 */
    DMA2D->OPFCCR = 2 << 0;     /* CM[2:0]=010,输出为RGB565格式 */
    DMA2D->OOR = 0;             /* 设置行偏移为0 */
    DMA2D->IFCR |= 1 << 1;      /* 清除传输完成标志 */
    regval = 11 << 0;           /* CM[3:0]=1011,输入数据为YCbCr格式 */
    regval |= cm << 18;         /* CSS[1:0]=cm,Chroma Sub-Sampling:0,4:4:4;1,4:2:2;2,4:2:0 */
    DMA2D->FGPFCCR = regval;    /* 设置FGPCCR寄存器 */
    DMA2D->FGOR = 0;            /* 前景层行偏移为0 */
    DMA2D->NLR = tjpeg->yuvblk_height | (tjpeg->Conf.ImageWidth << 16); /* 设定行数寄存器 */
    DMA2D->OMAR = (uint32_t)pdst;   /* 输出存储器地址 */
    DMA2D->FGMAR = (uint32_t)tjpeg->outbuf[tjpeg->outbuf_read_ptr].buf; /* 源地址 */
    DMA2D->CR |= 1 << 0;        /* 启动DMA2D */

    while ((DMA2D->ISR & (1 << 1)) == 0)    /* 等待传输完成 */
    {
        timeout++;

        if (timeout > 0X1FFFFFF)break;      /* 超时退出 */
    }

    /* YUV2RGB转码结束后,再复位一次DMA2D */
    RCC->AHB3RSTR |= 1 << 4;    /* 复位DMA2D */
    RCC->AHB3RSTR &= ~(1 << 4); /* 结束复位 */

    if (timeout > 0X1FFFFFF)return 1;

    return 0;
}





























