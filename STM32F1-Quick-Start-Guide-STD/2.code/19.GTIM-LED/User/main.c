#include "stm32f10x.h"
#include "tim.h"
#include "usart.h"
#include "led.h"

extern volatile uint32_t time;

int main(void)
{    
    USARTx_Init();   
    BSP_LED_Init();
    TIM_Mode_Init();
    while(1)
    {
        if(time  == 1000) // 1000*1=1ms
        {
            time = 0;
            LED_TOGGLE(RED_LED_Pin);
            printf("LED TOGGLE\r\n");
        }
    }
}
