#include "stm32f10x.h"
#include "ADC.h"
#include "SysTick.h"
#include "usart.h"

// 双ADC模式结果变量 (ADC2结果在高16位，ADC1结果在低16位)
extern __IO uint32_t ADC_ConvertedValue;

int main(void)
{
    ADCx_Init();        
    USARTx_Init();   
    SysTick_Init();   
    // 电压转换结果
    float ADC1_Voltage = 0.0f;
    float ADC2_Voltage = 0.0f;  
    while(1)
    {
        // 从32位转换值中提取两个ADC的结果
        uint16_t adc2_value = (ADC_ConvertedValue >> 16) & 0xFFFF; // 高16位: ADC2
        uint16_t adc1_value = ADC_ConvertedValue & 0xFFFF;         // 低16位: ADC1
        // 转换为电压值 (3.3V参考电压, 12位ADC)
        ADC1_Voltage = (adc1_value * 3.3f) / 4096.0f;
        ADC2_Voltage = (adc2_value * 3.3f) / 4096.0f;
        printf("ADC1 (PC1) Voltage = %.3f V\r\n", ADC1_Voltage);
        printf("ADC2 (PC4) Voltage = %.3f V\r\n", ADC2_Voltage);
        Delay_ms(5000);
    }
}
