/**
 ****************************************************************************************************
 * @file        dma.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-03-26
 * @brief       DMA 驱动代码
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
 * V1.0 20200326
 * 第一次发布
 *
 ****************************************************************************************************
 */

#include "./BSP/DMA/dma.h"
#include "./SYSTEM/delay/delay.h"


/**
 * @brief       串口TX DMA初始化函数
 *   @note      这里的传输形式是固定的, 这点要根据不同的情况来修改
 *              从存储器 -> 外设模式/8位数据宽度/存储器增量模式
 *
 * @param       dma_streamx : DMA数据流,DMA1_Stream0~7/DMA2_Stream0~7
 * @param       ch         :  DMA通道选择,范围:1~115(详见<<STM32H7xx参考手册>>16.3.2节,Table 116)
 * @param       par         : 外设地址
 * @param       mar         : 存储器地址
 * @retval      无
 */
void dma_usart_tx_config(DMA_Stream_TypeDef *dma_streamx, uint8_t ch, uint32_t par, uint32_t mar)
{
    dma_mux_init(dma_streamx, ch);  /* 初始化DMA 请求复用器 */
    
    dma_streamx->PAR = par;     /* DMA外设地址 */
    dma_streamx->M0AR = mar;    /* DMA 存储器0地址 */
    dma_streamx->NDTR = 0;      /* DMA 传输长度清零, 后续在dma_enable函数设置 */
    dma_streamx->CR = 0;        /* 先全部复位CR寄存器值 */

    dma_streamx->CR |= 1 << 6;  /* 存储器到外设模式 */
    dma_streamx->CR |= 0 << 8;  /* 非循环模式(即使用普通模式) */
    dma_streamx->CR |= 0 << 9;  /* 外设非增量模式 */
    dma_streamx->CR |= 1 << 10; /* 存储器增量模式 */
    dma_streamx->CR |= 0 << 11; /* 外设数据长度:8位 */
    dma_streamx->CR |= 0 << 13; /* 存储器数据长度:8位 */
    dma_streamx->CR |= 1 << 16; /* 中等优先级 */
    dma_streamx->CR |= 0 << 21; /* 外设突发单次传输 */
    dma_streamx->CR |= 0 << 23; /* 存储器突发单次传输 */

    //dma_streamx->FCR=0X21;      /* FIFO控制寄存器 */
}

/**
 * @brief       DMA请求复用器初始化
 *   @note      函数功能包括:
 *              1, 使能DMA时钟
 *              2, 清空对应Stream上面的所有中断多标志
 *              3, 选择DMA请求通道
 *
 * @param       dma_streamx : DMA数据流,DMA1_Stream0~7/DMA2_Stream0~7
 * @param       ch          : DMA通道选择,范围:1~115(详见<<STM32H7xx参考手册>>16.3.2节,Table 116)
 * @retval      无
 */
void dma_mux_init(DMA_Stream_TypeDef *dma_streamx, uint8_t ch)
{
    DMA_TypeDef *DMAx;
    DMAMUX_Channel_TypeDef *DMAMUXx;
    uint8_t streamx;

    if ((uint32_t)dma_streamx > (uint32_t)DMA2)   /* 得到当前stream是属于DMA2还是DMA1 */
    {
        DMAx = DMA2;
        RCC->AHB1ENR |= 1 << 1;     /* DMA2时钟使能 */
    }
    else
    {
        DMAx = DMA1;
        RCC->AHB1ENR |= 1 << 0;     /* DMA1时钟使能 */
    }

    while (dma_streamx->CR & 0X01); /* 等待DMA可配置 */

    streamx = (((uint32_t)dma_streamx - (uint32_t)DMAx) - 0X10) / 0X18; /* 得到stream通道号 */

    if (streamx >= 6)
    {
        DMAx->HIFCR |= 0X3D << (6 * (streamx - 6) + 16);   /* 清空之前该stream上的所有中断标志 */
    }
    else if (streamx >= 4)
    {
        DMAx->HIFCR |= 0X3D << 6 * (streamx - 4);           /* 清空之前该stream上的所有中断标志 */
    }
    else if (streamx >= 2)
    {
        DMAx->LIFCR |= 0X3D << (6 * (streamx - 2) + 16);    /* 清空之前该stream上的所有中断标志 */
    }
    else
    {
        DMAx->LIFCR |= 0X3D << 6 * streamx; /* 清空之前该stream上的所有中断标志 */
    }
    
    if ((uint32_t)dma_streamx > (uint32_t)DMA2)
    {
        streamx += 8;  /* 如果是DMA2,通道编号+8 */
    }

    DMAMUXx = (DMAMUX_Channel_TypeDef *)(DMAMUX1_BASE + streamx * 4);   /* 得到对应的DMAMUX通道控制地址 */
    DMAMUXx->CCR = ch & 0XFF;   /* 选择DMA请求通道 */

}


/**
 * @brief       开启一次DMA传输
 * @param       dma_streamx : DMA数据流,DMA1_Stream0~7/DMA2_Stream0~7
 * @param       ndtr        : 数据传输量
 * @retval      无
 */
void dma_enable(DMA_Stream_TypeDef *dma_streamx, uint16_t ndtr)
{
    dma_streamx->CR &= ~(1 << 0);   /* 关闭DMA传输 */

    while (dma_streamx->CR & 0X1);  /* 确保DMA可以被设置 */

    dma_streamx->NDTR = ndtr;       /* DMA 存储器0地址 */
    dma_streamx->CR |= 1 << 0;      /* 开启DMA传输 */
}



























