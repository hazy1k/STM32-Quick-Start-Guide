/**
 ****************************************************************************************************
 * @file        wdg.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-03-13
 * @brief       看门狗 驱动代码
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
 * V1.0 20200312
 * 第一次发布
 *
 ****************************************************************************************************
 */

#include "./BSP/WDG/wdg.h"

/**
 * @brief       初始化独立看门狗 
 * @param       prer: 分频数:0~7(只有低3位有效!)
 *   @arg       分频因子 = 4 * 2^prer. 但最大值只能是256!
 * @param       rlr: 自动重装载值,0~0XFFF. 
 * @note        时间计算(大概):Tout=((4 * 2^prer) * rlr) / 32 (ms). 
 * @retval      无
 */
void iwdg_init(uint8_t prer, uint16_t rlr)
{
    IWDG1->KR = 0X5555; /* 使能对IWDG->PR和IWDG->RLR的写 */
    IWDG1->PR = prer;   /* 设置分频系数 */
    IWDG1->RLR = rlr;   /* 从加载寄存器 IWDG->RLR */
    IWDG1->KR = 0XAAAA; /* reload */
    IWDG1->KR = 0XCCCC; /* 使能看门狗 */
}

/**
 * @brief       喂独立看门狗
 * @param       无
 * @retval      无
 */
void iwdg_feed(void)
{
    IWDG1->KR = 0XAAAA; /* reload */
}








