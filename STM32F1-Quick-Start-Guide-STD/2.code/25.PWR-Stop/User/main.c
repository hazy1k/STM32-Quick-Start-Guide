#include "stm32f10x.h"
#include "systick.h"
#include "led.h"
#include "key.h"
#include "usart.h"
#include <stdio.h>

// 停止模式唤醒后使用HSI时钟，我们需要切换至HSE
void STOP_SYSConfig(void)
{
    RCC_HSEConfig(RCC_HSE_ON);
    while(RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET);
    RCC_PLLCmd(ENABLE);
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    while(RCC_GetSYSCLKSource() != 0x08); // 系统时钟源切换到PLL
}

int main(void)
{
    RCC_ClocksTypeDef clock_status_wakeup;
    RCC_ClocksTypeDef clock_status_config;
    uint8_t clock_source_wakeup;
    uint8_t clock_source_config;
    
    BSP_LED_Init();
    KEY_EXTI_Init();
    SysTick_Init();
    USARTx_Init(115200);
    
    while(1)
    {
        printf("绿灯亮，系统正常工作中...\r\n");
        LED_ON(GREEN_LED_Pin);
        Delay_ms(500);
        LED_OFF(GREEN_LED_Pin);
        Delay_ms(500);
        printf("准备进入停止模式...\r\n");
        // 确保所有串口数据已发送
        while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);    
        PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
        
        // 1. 获取唤醒时的时钟状态 (使用正确的结构体)
        clock_source_wakeup = RCC_GetSYSCLKSource();
        RCC_GetClocksFreq(&clock_status_wakeup);
        
        // 2. 重新配置系统时钟
        STOP_SYSConfig();
        
        // 3. 重新初始化关键外设
        SysTick_Init();          // 系统时钟改变后需重配SysTick
        USARTx_Init(115200);     // 必须重新初始化USART
        
        // 4. 获取重新配置后的时钟状态
        clock_source_config = RCC_GetSYSCLKSource();
        RCC_GetClocksFreq(&clock_status_config);
        
        // 5. 打印时钟状态
        printf("\r\n--- 唤醒后时钟状态 ---\r\n");
        printf("时钟源: %s\r\n", (clock_source_wakeup == 0) ? "HSI" : "PLL");
        printf("SYSCLK: %d Hz\r\n", clock_status_wakeup.SYSCLK_Frequency);
        printf("HCLK  : %d Hz\r\n", clock_status_wakeup.HCLK_Frequency);
        printf("PCLK1 : %d Hz\r\n", clock_status_wakeup.PCLK1_Frequency);
        printf("PCLK2 : %d Hz\r\n", clock_status_wakeup.PCLK2_Frequency);
        
        printf("\r\n--- 重配后时钟状态 ---\r\n");
        printf("时钟源: %s\r\n", (clock_source_config == 8) ? "PLL" : "HSI");
        printf("SYSCLK: %d Hz\r\n", clock_status_config.SYSCLK_Frequency);
        printf("HCLK  : %d Hz\r\n", clock_status_config.HCLK_Frequency);
        printf("PCLK1 : %d Hz\r\n", clock_status_config.PCLK1_Frequency);
        printf("PCLK2 : %d Hz\r\n", clock_status_config.PCLK2_Frequency);
        
        printf("\r\n测试完成，等待下一次循环...\r\n\r\n");
        Delay_ms(2000);
    }
}
