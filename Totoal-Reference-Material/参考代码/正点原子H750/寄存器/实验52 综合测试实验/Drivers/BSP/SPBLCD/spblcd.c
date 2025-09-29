/**
 ****************************************************************************************************
 * @file        spblcd.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-05-26
 * @brief       滑屏功能 驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.0 20220526
 * 第一次发布
 *
 ****************************************************************************************************
 */

#include "./MALLOC/malloc.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/SPBLCD/spblcd.h"
#include "./BSP/NORFLASH/norflash_ex.h"

#include "spb.h"


/**
 * @brief       从lcd拷贝到spi flash
 * @param       offset  : 存储地址偏移量
 * @param       width   : 宽度
 * @param       height  : 高度
 * @retval      0,成功; 其他,错误代码;
 */
uint8_t slcd_frame_lcd2spi(uint32_t offset, uint16_t width, uint16_t height)
{
    uint16_t *pbuf;
    uint32_t startx;        /* 起始地址 */
    uint16_t i, j, k;
    uint32_t woffset = 0;   /* 写地址偏移 */

    startx = SPILCD_BASE + offset * 2;  /* 因为offset是以像素为单位的,而写入是以字节为单位的，所以得*2 */
    pbuf = mymalloc(SRAMIN, 4096);      /* 申请内存 */

    if (!pbuf)return 1;

    woffset = 0;
    k = 0;

    for (i = 0; i < width; i++)
    {
        for (j = 0; j < height; j++)
        {
            pbuf[k++] = lcd_read_point(i, j);    /* 读取点 */

            if (k == 2048)
            {
                norflash_ex_write((uint8_t *)pbuf, startx + woffset, 4096); /* 写入一个扇区 */
                woffset += 4096;
                k = 0;
            }
        }
    }

    if (k)  /* 还有一点数据要写入 */
    {
        norflash_ex_write((uint8_t *)pbuf, startx + woffset, k * 2);    /* 写入最后一次数据 */
    }

    myfree(SRAMIN, pbuf);   /* 释放内存 */
    return 0;
}

/**
 * @brief       QSPI --> LCD_RAM dma配置
 *  @note       PAR和NDTR在后面使能的时候再设置
 * @param       无
 * @retval      无
 */
void slcd_dma_init(void)
{
    RCC->AHB1ENR |= 1 << 1;             /* DMA2时钟使能 */

    while (DMA2_Stream0->CR & 0X01);    /* 等待DMA2_Stream0可配置 */

    DMA2->LIFCR |= 0X3D << 6 * 0;       /* 清空通道0上所有中断标志 */
    DMA2_Stream0->FCR = 0X0000021;      /* 设置为默认值 */
 
    DMA2_Stream0->PAR = 0;              /* 暂不设置 */
    DMA2_Stream0->M0AR = (uint32_t)&LCD->LCD_RAM;   /* 目标地址为LCD_RAM */
    DMA2_Stream0->M1AR = 0;             /* 不用设置 */
    DMA2_Stream0->NDTR = 0;             /* 暂时设置长度为0 */
    DMA2_Stream0->CR = 0;               /* 先全部复位CR寄存器值 */
    DMA2_Stream0->CR |= 2 << 6;         /* 存储器到存储器模式 */
    DMA2_Stream0->CR |= 0 << 8;         /* 普通模式 */
    DMA2_Stream0->CR |= 1 << 9;         /* 外设增量模式 */
    DMA2_Stream0->CR |= 0 << 10;        /* 存储器非增量模式 */
    DMA2_Stream0->CR |= 1 << 11;        /* 外设数据长度:16位 */
    DMA2_Stream0->CR |= 1 << 13;        /* 存储器数据长度:16位 */
    DMA2_Stream0->CR |= 0 << 16;        /* 低优先级 */
    DMA2_Stream0->CR |= 0 << 18;        /* 单缓冲模式 */
    DMA2_Stream0->CR |= 0 << 21;        /* 外设突发单次传输 */
    DMA2_Stream0->CR |= 0 << 23;        /* 存储器突发单次传输 */

    DMA2_Stream0->FCR &= ~(1 << 2);     /* 不使用FIFO模式 */
    DMA2_Stream0->FCR &= ~(3 << 0);     /* 无FIFO 设置 */
}

/**
 * @brief       开启一次SPI到LCD的DMA的传输
 * @param       x       : 起始传输地址编号
 * @retval      无
 */
void slcd_dma_enable(uint32_t x)
{
    uint16_t *pflash = (uint16_t *)(0X90000000 + SPILCD_BASE);
    pflash += x * spbdev.spbheight;
    uint32_t lcdsize = spbdev.spbwidth * spbdev.spbheight;
    uint32_t dmatransfered = 0;

    while (lcdsize)
    {
        DMA2_Stream0->CR &= ~(1 << 0);      /* 关闭DMA传输 */

        while (DMA2_Stream0->CR & 0X01);    /* 等待DMA2_Stream0可配置 */
 
        DMA2->LIFCR |= 1 << 5;              /* 清除上次的传输完成标记 */

        if (lcdsize > SLCD_DMA_MAX_TRANS)
        {
            lcdsize -= SLCD_DMA_MAX_TRANS;
            DMA2_Stream0->NDTR = SLCD_DMA_MAX_TRANS;    /* 设置传输长度 */
        }
        else
        {
            DMA2_Stream0->NDTR = lcdsize;   /* 设置传输长度 */
            lcdsize = 0;
        }
 
        SCB_CleanInvalidateDCache();        /* 清除无效的D-Cache */
        DMA2_Stream0->PAR = (uint32_t)(pflash + dmatransfered);
        dmatransfered += SLCD_DMA_MAX_TRANS;
        DMA2_Stream0->CR |= 1 << 0;         /* 开启DMA传输 */

        while ((DMA2->LISR & (1 << 5)) == 0);   /* 等待传输完成 */
    }

    DMA2_Stream0->CR &= ~(1 << 0);          /* 关闭DMA传输 */
}

/**
 * @brief       显示一帧,即启动一次spi到lcd的显示.
 * @param       x       : 坐标偏移量
 * @retval      无
 */
void slcd_frame_show(uint16_t x)
{
    lcd_scan_dir(U2D_L2R);  /* 设置扫描方向 */

    if (lcddev.id == 0X9341 || lcddev.id == 0X7789 || lcddev.id == 0X5310 || lcddev.id == 0X7796 || lcddev.id == 0X5510 || lcddev.id == 0X9806)
    {
        lcd_set_window(spbdev.stabarheight, 0, spbdev.spbheight, spbdev.spbwidth);
        lcd_set_cursor(spbdev.stabarheight, 0); /* 设置光标位置 */
    }
    else
    {
        lcd_set_window(0, spbdev.stabarheight, spbdev.spbwidth, spbdev.spbheight);

        if (lcddev.id != 0X1963)lcd_set_cursor(0, spbdev.stabarheight); /* 设置光标位置 */
    }

    lcd_write_ram_prepare();    /* 开始写入GRAM */
    slcd_dma_enable(x);         /* 启动一次SPI到LCD的dma传输 */
    lcd_scan_dir(DFT_SCAN_DIR); /* 恢复默认方向 */
    lcd_set_window(0, 0, lcddev.width, lcddev.height);  /* 恢复默认窗口大小 */
} 

































