#include "stm32f10x.h"
#include "tim.h"
#include "systick.h"
#include "usart.h"
#include <stdio.h>

int main(void)
{   
    SysTick_Init();
    USARTx_Init(115200);
    TIM_Mode_Init();
    printf("TIM_Mode_Init...\r\n");
    while(1)
    {
        if(Is_NewCaptureAvailable())
        {
            uint32_t duration_us = Get_KeyPressDuration();
            float duration_ms = duration_us / 1000.0f;
            printf("duration_us: %d, duration_ms: %.2f\r\n", duration_us, duration_ms);
            Delay_ms(100);
        }
    }
}
