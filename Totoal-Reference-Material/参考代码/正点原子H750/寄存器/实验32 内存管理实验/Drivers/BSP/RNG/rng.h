/**
 ****************************************************************************************************
 * @file        rng.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-03-25
 * @brief       RNG(随机数发生器) 驱动代码
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
 * V1.0 20200325
 * 第一次发布
 *
 ****************************************************************************************************
 */

#ifndef __RNG_H
#define __RNG_H 

#include "./SYSTEM/sys/sys.h"

uint8_t rng_init(void);             /* RNG初始化 */
uint32_t rng_get_random_num(void);  /* 得到随机数 */
int rng_get_random_range(int min,int max);  /* 得到属于某个范围内的随机数 */

#endif

















