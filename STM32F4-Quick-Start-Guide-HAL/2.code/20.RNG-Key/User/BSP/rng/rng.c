#include "rng.h"
#include "delay.h"

RNG_HandleTypeDef rng_hangle;

// 初始化RNG
uint8_t RNG_Init(void)
{
    uint16_t retry = 0;
    rng_hangle.Instance = RNG;
    HAL_RNG_DeInit(&rng_hangle);
    HAL_RNG_Init(&rng_hangle);
    while(__HAL_RNG_GET_FLAG(&rng_hangle, RNG_FLAG_DRDY) == RESET && retry < 10000)
    {
        retry++;
        delay_us(10);
    }
    if(retry >= 10000)
    {
        return 1;
    }
    return 0;
}

void HAL_RNG_MspInit(RNG_HandleTypeDef *hRNG)
{
    __HAL_RCC_RNG_CLK_ENABLE();
}

// 获取随机数
uint32_t rng_get_random(void)
{
    uint32_t random_num = 0;
    HAL_RNG_GenerateRandomNumber(&rng_hangle, &random_num);
    return random_num;
}

// 获取范围内的随机数
uint32_t rng_get_random_range(int min, int max)
{
    uint32_t random_num = 0;
    HAL_RNG_GenerateRandomNumber(&rng_hangle, &random_num);
    return (random_num % (max - min + 1)) + min;
}

