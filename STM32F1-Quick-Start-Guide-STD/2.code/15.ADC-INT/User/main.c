#include "stm32f10x.h"
#include "SysTick.h"
#include "led.h"
#include "usart.h"
#include "adc.h"
#include <stdio.h>

extern __IO uint16_t ADC_ConverValue;
float ADC_Result;

int main(void) 
{
    BSP_LED_Init();
    SysTick_Init();
    USARTx_Init();
    ADC_Mode_Init();
    while(1) {
        ADC_Result = (float)ADC_ConverValue/4096*3.3;
        printf("ADC_ConverValue = 0x%04x \r\n", ADC_ConverValue);
        printf("ADC_Result = %.2f \r\n", ADC_Result);
        Delay_ms(2000);
    }
}
