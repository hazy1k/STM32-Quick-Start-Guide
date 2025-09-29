/**
 ****************************************************************************************************
 * @file        sms_sn76496.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.1
 * @date        2022-05-31
 * @brief       SMS模拟器 代码
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

#ifndef __SMS_SN76496_H
#define __SMS_SN76496_H

#include "./SYSTEM/sys/sys.h"


#define MAX_76496       4
#define MASTER_CLOCK    3579545
#define Sound_Rate      44100            /* 采样率 */
#define SNBUF_size      Sound_Rate/60    /* 每帧数据量  1, 44100/60=735;  2, 22050/60=367 */
#define sms_Frame       2                /* 帧音频数据缓存数目 */


typedef struct
{
    int  Channel;
    int  SampleRate;
    int  VolTable[16];
    int  Register[8];
    int  Volume[4];
    int  LastRegister;
    int  NoiseFB;
    int  Period[4];
    int  Count[4];
    int  Output[4];
    unsigned int RNG_A;
    unsigned int UpdateStep;
} t_SN76496;
extern t_SN76496 *SN76496;
extern uint16_t *psg_buffer;    /* 音频数据缓存,大小为:SNBUF_size*2字节 */

void SN76496Write(int data);
void SN76496Update(short *buffer, int length, unsigned char mask);
void SN76496_set_clock(int clock);
void SN76496_set_gain(int gain);
int  SN76496_init(int clock, int volume, int sample_rate);

uint8_t sms_audio_init(void);
void sms_update_Sound(void);
#endif
