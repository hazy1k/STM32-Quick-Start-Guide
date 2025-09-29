/**
 ****************************************************************************************************
 * @file        sms_main.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.1
 * @date        2022-05-31
 * @brief       SMS主函数 代码
 *              本程序移植自网友ye781205的NES模拟器工程, 特此感谢!
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
 * V1.1 20220531
 * 1, 修改注释方式
 * 2, 修改u8/u16/u32为uint8_t/uint16_t/uint32_t
 ****************************************************************************************************
 */

#ifndef __SMS_MAIN_H
#define __SMS_MAIN_H

#include "./SYSTEM/sys/sys.h"



extern uint8_t* SMS_romfile;    /* ROM指针 */


void sms_sram_free(void);
uint8_t sms_sram_malloc(uint32_t romsize);
void sms_set_window(void);

void sms_start(uint8_t bank_mun);
void sms_update_pad(void);
uint8_t sms_load(uint8_t* pname);

void sms_i2s_dma_tx_callback(void);
void sms_sound_open(int sample_rate);
void sms_sound_close(void);
void sms_apu_fill_buffer(int samples,uint16_t* wavebuf);
 
#endif










