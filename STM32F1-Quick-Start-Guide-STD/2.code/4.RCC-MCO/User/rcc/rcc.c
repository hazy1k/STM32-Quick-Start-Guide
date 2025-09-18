#include "rcc.h"

/* 设置系统时钟步骤
    1.开启HSE/HSI，等待稳定
    2.设置AHB、APB2、APB1时钟分频
    3.设置PLL时钟来源和PLL时钟分配
    4.开启PLL，等待稳定
    5.把PLLCLK输出作为系统时钟 
    6.读取时钟切换状态位，确保时钟切换成功
*/

void HSE_Clock_Init(uint32_t pllmul)
{
    __IO uint32_t StartUpCounter = 0, HSEStartUpStatus = 0;

    RCC_DeInit(); // 复位RCC
    RCC_HSEConfig(RCC_HSE_ON); // 开启HSE
    HSEStartUpStatus = RCC_WaitForHSEStartUp(); // 等待HSE稳定
    if(HSEStartUpStatus == SUCCESS)
    {
        FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
        FLASH_SetLatency(FLASH_Latency_2);
        RCC_HCLKConfig(RCC_SYSCLK_Div1); // AHB分频为1分频，HCLK = SYSCLK 
        RCC_PCLK2Config(RCC_HCLK_Div1); // APB2分频为1分频，PCLK2 = HCLK 
        RCC_PCLK1Config(RCC_HCLK_Div2); // APB1分频为2分频，PCLK1 = HCLK/2
        RCC_PLLConfig(RCC_PLLSource_HSE_Div1, pllmul); // 设置PLL时钟来源为HSE，设置PLL倍频
        RCC_PLLCmd(ENABLE); // 启动PLL
        while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET); // 等待PLL启动完成
        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK); // 选择PLL作为系统时钟源
        while(RCC_GetSYSCLKSource() != 0x08); // 读取时钟切换状态位，确保系统时钟切换完成
    }
    else // 如果HSE开启失败
    {
        while(1);
    }
}

void HSI_Clock_Init(uint32_t pllmul)
{
    __IO uint32_t HSIStartUpStatus = 0;

    RCC_DeInit();
    RCC_HSICmd(ENABLE); // 启动HSI
    HSIStartUpStatus = RCC->CR & RCC_CR_HSIRDY; // 等待HSI就绪
    if(HSIStartUpStatus == RCC_CR_HSIRDY)
    {
        FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
        FLASH_SetLatency(FLASH_Latency_2);
        RCC_HCLKConfig(RCC_SYSCLK_Div1);
        RCC_PCLK2Config(RCC_HCLK_Div1);
        RCC_PCLK1Config(RCC_HCLK_Div2);
        RCC_PLLConfig(RCC_PLLSource_HSI_Div2, pllmul);
        RCC_PLLCmd(ENABLE);
        while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
        while(RCC_GetSYSCLKSource() != 0x08);
    }
    else
    {
        while(1);
    }
}
