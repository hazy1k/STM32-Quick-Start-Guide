/**
 ****************************************************************************************************
 * @file        sms_render.h
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

#ifndef __SMS_RENDER_H
#define __SMS_RENDER_H

#include "sms_vdp.h"


/* formato de cor usado pelo gens (16 bits 555 ou 565) */
#define MAKE_PIXEL(r, g, b) ((r << 8) & 0xF800) | ((g << 3) & 0x07E0) | ((b >> 3) & 0x001F) 

#define BACKDROP_COLOR      (0x10 | (SMS_VDP->reg[7] & 0x0F))

void  render_init(void);
void  render_reset(void);
//void  render_bg_gg(int line);
void  render_bg_sms(int line);
void  render_obj(int line);
void  update_cache(void);
void  palette_sync(int index);
void render_line(int line);

#endif



