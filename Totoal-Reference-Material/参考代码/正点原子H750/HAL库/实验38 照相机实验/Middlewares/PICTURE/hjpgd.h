/**
 ****************************************************************************************************
 * @file        hjpgd.h
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

#ifndef __HJPGD_H
#define __HJPGD_H

#include "./BSP/JPEGCODEC/jpegcodec.h"


extern jpeg_codec_typedef hjpgd;  

 
/* 接口函数 */

void jpeg_dma_in_callback(void);
void jpeg_dma_out_callback(void);
void jpeg_endofcovert_callback(void);
void jpeg_hdrover_callback(void);
uint8_t hjpgd_decode(char* pname);

#endif




























