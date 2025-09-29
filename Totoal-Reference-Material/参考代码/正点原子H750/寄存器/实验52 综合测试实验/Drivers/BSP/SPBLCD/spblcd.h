/**
 ****************************************************************************************************
 * @file        spblcd.h
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

#ifndef __SPBLCD_H__
#define __SPBLCD_H__

#include "./SYSTEM/sys/sys.h"



#define SLCD_DMA_MAX_TRANS  60*1024     /* DMA一次最多传输60K字节 */

#define SPILCD_BASE         200*4096    /**
                                         * SPB界面,从QSPI FLASH的第800KB字节开始存储,占用最大为2049600+1字节
                                         * SPILCD_BASE，存放液晶分辨率标志：
                                         * 0，表示240*320的屏；1，表示320*480的屏；2，表示480*800的屏；其他值，非法
                                         * 对于320*240的屏,最大占用   224*840*2≈368K字节
                                         * 对于480*320的屏,最大占用   364*1120*2≈797K字节
                                         * 对于800*480的屏,最大占用   610*1680*2≈2002K字节
                                         */

#define SPILCD_END_ADDR     700*4096    /* SPB界面数据结束位置 */

 
 
uint8_t slcd_frame_lcd2spi(uint32_t offset,uint16_t width,uint16_t height);

void slcd_spi2_mode(uint8_t mode);

void slcd_dma_init(void);

void slcd_dma_enable(uint32_t x);
void slcd_frame_show(uint16_t x);


#endif








