/**
 ****************************************************************************************************
 * @file        rng.c
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
 
#include "./BSP/RNG/rng.h"
#include "./SYSTEM/delay/delay.h"


RNG_HandleTypeDef rng_handle;  //RNG句柄

/**
 * @brief       初始化RNG
 * @param       无
 * @retval      0,成功;1,失败
 */
uint8_t rng_init(void)
{
    uint16_t retry = 0;

    rng_handle.Instance = RNG;   
    HAL_RNG_DeInit(&rng_handle);
    HAL_RNG_Init(&rng_handle);    /* 初始化RNG */
    while (__HAL_RNG_GET_FLAG(&rng_handle, RNG_FLAG_DRDY) == RESET && retry < 10000) /* 等待RNG准备就绪 */
    {
        retry++;
        delay_us(10);
    }
    if (retry >= 10000)
    {
        return 1; /* 随机数产生器工作不正常 */
    }
    return 0;
}

/**
 * @brief       RNG底层驱动，时钟源设置和使能
 * @note        此函数会被HAL_RNG_Init()调用
 * @param       hrng:RNG句柄
 * @retval      无
 */
void HAL_RNG_MspInit(RNG_HandleTypeDef *hrng)
{
    RCC_PeriphCLKInitTypeDef RNGClkInitStruct;

    /* 设置RNG时钟源，选择PLL，时钟为480MHz */
    RNGClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RNG;    /* 设置RNG时钟源 */
    RNGClkInitStruct.RngClockSelection = RCC_RNGCLKSOURCE_PLL;    /* RNG时钟源选择PLL */
    HAL_RCCEx_PeriphCLKConfig(&RNGClkInitStruct);

    __HAL_RCC_RNG_CLK_ENABLE();/* 使能RNG时钟 */
}

/**
 * @brief       得到随机数
 * @param       无
 * @retval      获取到的随机数(32bit)
 */
uint32_t rng_get_random_num(void)
{
    uint32_t randomnum;
    HAL_RNG_GenerateRandomNumber(&rng_handle, &randomnum);
    return randomnum;
}

/**
 * @brief       得到某个范围内的随机数
 * @param       min,max: 最小,最大值.
 * @retval      得到的随机数(rval),满足:min<=rval<=max
 */
int rng_get_random_range(int min, int max)
{ 
    uint32_t randomnum;
    HAL_RNG_GenerateRandomNumber(&rng_handle, &randomnum);
    return randomnum%(max-min+1) + min;
}




