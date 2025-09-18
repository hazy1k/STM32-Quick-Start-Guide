#include "rng.h"
#include "delay.h"

RNG_HandleTypeDef RNG_Handler; // RNG句柄
// 初始化RNG
u8 RNG_Init(void)
{
    u16 retry = 0; // 重试次数
    RNG_Handler.Instance = RNG; // 选择RNG
    HAL_RNG_Init(&RNG_Handler); // 初始化RNG
    while(__HAL_RNG_GET_FLAG(&RNG_Handler, RNG_FLAG_DRDY) == RESET&&retry<10000) // 等待RNG准备就绪
    {
        retry++;
        delay_us(10);
    }
    if(retry>=10000) 
        return 1;//随机数产生器工作不正常
    return 0;
}
// RNG MSP初始化
void HAL_RNG_MspInit(RNG_HandleTypeDef *hrng)
{
     __HAL_RCC_RNG_CLK_ENABLE(); // 使能RNG时钟
}

// 得到随机数
// 返回值:获取到的随机数
u32 RNG_Get_RandomNum(void)
{
    return HAL_RNG_GetRandomNumber(&RNG_Handler);
}
// 生成[min,max]范围的随机数
int RNG_Get_RandomRange(int min,int max)
{ 
   return HAL_RNG_GetRandomNumber(&RNG_Handler)%(max-min+1) + min;
}