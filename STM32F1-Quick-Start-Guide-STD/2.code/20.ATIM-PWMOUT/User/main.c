#include "stm32f10x.h"
#include "tim.h"
#include "systick.h"
#include "usart.h"
#include <stdio.h>

int main(void)
{   
    SysTick_Init();
    USARTx_Init(115200);
    printf("\r\nSystem Booted\r\n");
    printf("TIM1 Advanced Timer Demo\r\n");
    printf("For break function test:\r\n");
    printf("  - Connect PB12 to 3.3V to trigger break\r\n");
    printf("  - Disconnect PB12 to clear break flag\r\n");
    ATIM_Mode_Init();
    TIM1_DebugInfo();
    while(1) {
        printf("\r\n===== Main Menu =====\r\n");
        printf("1. Read TIM1 Status\r\n");
        printf("2. Test Break Function (connect PB12 to 3.3V)\r\n");
        printf("3. Reset Break Flag (disconnect PB12 from 3.3V)\r\n");
        printf("4. Re-enable PWM Output\r\n");
        printf("Enter choice: ");
        char choice = USART_ReceiveChar();
        printf("\r\nSelected: %c\r\n", choice);

        switch(choice) {
            case '1':  // 读取状态
                TIM1_DebugInfo();
                break;

            case '2':  // 测试刹车功能
                printf("Testing Break Function...\r\n");
                printf("Current PB12 state: %s\r\n", 
                       GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) ? "HIGH" : "LOW");
                
                if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12)) {
                    printf("PB12 is already HIGH. Break should be active.\r\n");
                } else {
                    printf("Please connect PB12 to 3.3V to trigger break...\r\n");
                    // 等待用户操作
                    uint32_t start = SysTick_GetTick();
                    while(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) && 
                          (SysTick_GetTick() - start < 5000)) {
                        // 等待PB12变高或超时(5秒)
                    }
                    
                    if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12)) {
                        printf("PB12 is now HIGH. Break should be active.\r\n");
                    } else {
                        printf("Timeout. PB12 still LOW.\r\n");
                    }
                }
                
                // 检查刹车状态
                if(TIM_GetFlagStatus(TIM1, TIM_FLAG_Break)) {
                    printf("Break Event Detected!\r\n");
                } else {
                    printf("No Break Event Detected! Check PB12 connection.\r\n");
                }
                
                TIM1_DebugInfo();
                break;
                
            case '3':  // 清除刹车标志
                printf("Clearing Break Flag...\r\n");
                printf("Current PB12 state: %s\r\n", 
                       GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) ? "HIGH" : "LOW");
                
                if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12)) {
                    printf("PB12 is still HIGH. Please disconnect PB12 from 3.3V...\r\n");
                    // 等待用户操作
                    uint32_t start = SysTick_GetTick();
                    while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) && 
                          (SysTick_GetTick() - start < 5000)) {
                        // 等待PB12变低或超时(5秒)
                    }
                }
                
                if(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12)) {
                    TIM_ClearFlag(TIM1, TIM_FLAG_Break);
                    Delay_ms(10);  // 稍等片刻让标志位更新
                    
                    if(TIM_GetFlagStatus(TIM1, TIM_FLAG_Break) == RESET) {
                        printf("Break flag cleared.\r\n");
                    } else {
                        printf("Failed to clear break flag!\r\n");
                    }
                } else {
                    printf("PB12 still HIGH. Cannot clear break flag.\r\n");
                }
                
                TIM1_DebugInfo();
                break;
                
            case '4':  // 重新使能PWM输出
                printf("Re-enabling PWM Output...\r\n");
                
                // 确保刹车标志已清除
                if(TIM_GetFlagStatus(TIM1, TIM_FLAG_Break)) {
                    printf("Break flag still active! Clear it first.\r\n");
                    TIM1_DebugInfo();
                    break;
                }
                
                // 重新配置BDTR（确保所有配置正确）
                TIM_BDTRInitTypeDef TIM_BDTRInitStructure;
                TIM_BDTRStructInit(&TIM_BDTRInitStructure);
                TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
                TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
                TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_1;
                TIM_BDTRInitStructure.TIM_DeadTime = 11;
                TIM_BDTRInitStructure.TIM_Break = TIM_Break_Enable;
                TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;
                TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
                TIM_BDTRConfig(TIM1, &TIM_BDTRInitStructure);
                
                // 使能主输出
                TIM1->BDTR |= TIM_BDTR_MOE;
                TIM_CtrlPWMOutputs(TIM1, ENABLE);
                
                printf("PWM output re-enabled.\r\n");
                TIM1_DebugInfo();
                break;
                
            default:
                printf("Invalid choice!\r\n");
        }
        Delay_ms(1000);
    }
}
