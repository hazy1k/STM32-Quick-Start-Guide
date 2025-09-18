#include "clkconfig.h"
#include "stm32f10x_rcc.h"

// 前置知识
/*
    1.AHB总线时钟：HCLK
    2.APB1总线时钟：PCLK1
    3.APB2总线时钟：PCLK2
    PCLK2 = HCLK = SYSCLK
    PCLK1 = HCLK/2,最高只能是36M
*/

// 使用HSE作为系统时钟源
/*
    1.开启HSE，等待HSE稳定
    2.设置AHB、APB1、APB2的预分频系数
    3.设置PLL的时钟来源和PLL的倍频因子
    4.开启PLL，等待PLL稳定
    5.把PLLCK切换到系统时钟SYSCLK,设置SYSCLK的时钟源为PLLCK
*/

void HSE_SetSYSCLK(uint32_t plluml) // plluml: PLL的倍频因子
{
    __IO uint32_t StartUpCounter = 0;   // 等待HSE稳定计数器
    __IO uint32_t HSEStartUpStatus = 0; // HSE启动成功标志位
    RCC_DeInit(); // 复位RCC寄存器，复位HSE、PLL
    // 1.开启HSE，等待HSE稳定
    RCC_HSEConfig(RCC_HSE_ON);
    HSEStartUpStatus = RCC_WaitForHSEStartUp(); 
    if(HSEStartUpStatus == SUCCESS)
    {
        // 2.启动HSE成功后，设置AHB、APB1、APB2的预分频系数
        FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable); // 使能FLASH预读
        FLASH_SetLatency(FLASH_Latency_2); // SYSCLK周期与闪存访问时间的比例设置，设置为2：48< SYSCLK <= 72M
        // 下面正式开始设置分频系数
        RCC_HCLKConfig(RCC_SYSCLK_Div1);// AHB预分频因子设置为1，即HCLK = SYSCLK
        RCC_PCLK1Config(RCC_HCLK_Div2); // APB1预分频因子设置为2，即PCLK1 = HCLK/2
        RCC_PCLK2Config(RCC_HCLK_Div1); // APB2预分频因子设置为1，即PCLK2 = HCLK
        // 3.设置PLL的时钟来源和PLL的倍频因子
        // PLLCLK计算公式：PLLCLK = 8MHz * PLLMUL
        RCC_PLLConfig(RCC_PLLSource_HSE_Div1, plluml);
        // 4.开启PLL，等待PLL稳定
        RCC_PLLCmd(ENABLE);
        while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
        {

        }
        // 5.把PLLCK切换到系统时钟SYSCLK
        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
        // 6.读取时钟切换状态以确保PLLCLK被选中为系统时钟（SYSCLK）
        while(RCC_GetSYSCLKSource() != 0x08)
        {

        }
    }
    else
    {
        // 启动HSE失败，可能是HSE不足，或者外部晶振不稳定
        // 如果HSE启动时钟，单片机会自动把HSE切换到HSI
        // HSI是单片机内部的高速时钟源，HSI的频率为8MHz
        while(1)
        {

        }
    }
}

// 使用HSI作为系统时钟源
// 和HSE的步骤一样，我们就不多做说明了
void HSI_SetSYSCLK(uint32_t plluml)
{
    __IO uint32_t HSIStartStatus = 0; // HSI启动成功标志位
    RCC_DeInit(); 
    RCC_HSICmd(ENABLE);
    while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET)
    {

    }
    HSIStartStatus = RCC->CR & RCC_CR_HSIRDY; // 读取HSI的就绪状态
    if(HSIStartStatus == RCC_CR_HSIRDY)
    {
        FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
        FLASH_SetLatency(FLASH_Latency_2);
        RCC_HCLKConfig(RCC_SYSCLK_Div1);
        RCC_PCLK1Config(RCC_HCLK_Div2);
        RCC_PCLK2Config(RCC_HCLK_Div1);
        RCC_PLLConfig(RCC_PLLSource_HSI_Div2, plluml);
        RCC_PLLCmd(ENABLE);
        while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
        {

        }
        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
        while(RCC_GetSYSCLKSource() != 0x08)
        {

        }
    }
    else
    {
        while(1)
        {
            
        }
    }
}
