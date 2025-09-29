/**
 ****************************************************************************************************
 * @file        settings.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.1
 * @date        2022-05-26
 * @brief       APP-设置 代码
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
 * V1.1 20220526
 * 1, 修改注释方式
 * 2, 修改u8/u16/u32为uint8_t/uint16_t/uint32_t
 ****************************************************************************************************
 */

#ifndef __SETTINGS_H
#define __SETTINGS_H

#include "./SYSTEM/sys/sys.h"
#include "os.h"
#include "common.h"
//#include "es8388.h"


/* list结构体.链表结构 */
typedef __PACKED_STRUCT
{
    uint8_t syslanguage;        /* 默认系统语言 */
    uint8_t lcdbklight;         /* LED背光亮度 10~110.10代表最暗;110代表最亮 */
    uint8_t picmode;            /* 图片浏览模式:0,顺序循环播放;1,随机播放 */
    //uint8_t audiomode;        /* 音频播放模式:0,顺序循环播放;1,随机播放;2,单曲循环播放 */
    uint8_t videomode;          /* 视频播放模式:0,顺序循环播放;1,随机播放;2,单曲循环播放 */
    uint8_t saveflag;           /* 保存标志,0X0A,保存过了;其他,还从未保存 */
} _system_setings;

extern _system_setings systemset;   /* 在settings.c里面设置 */

uint8_t sysset_time_set(uint16_t x, uint16_t y, uint8_t *hour, uint8_t *min, uint8_t *caption);
uint8_t sysset_date_set(uint16_t x, uint16_t y, uint16_t *year, uint8_t *month, uint8_t *date, uint8_t *caption);
uint8_t sysset_bklight_set(uint16_t x, uint16_t y, uint8_t *caption, uint16_t *bkval);
uint8_t sysset_system_update_cpymsg(uint8_t *pname, uint8_t pct, uint8_t mode);
void sysset_system_update(uint8_t *caption, uint16_t sx, uint16_t sy);
void sysset_system_info(uint16_t x, uint16_t y, uint8_t *caption);
void sysset_system_status(uint16_t x, uint16_t y, uint8_t *caption);
void sysset_system_about(uint16_t x, uint16_t y, uint8_t *caption);
uint8_t *set_search_caption(const uint8_t *mcaption);
void sysset_read_para(_system_setings *sysset);
void sysset_save_para(_system_setings *sysset);

uint8_t sysset_play(void);

#endif












