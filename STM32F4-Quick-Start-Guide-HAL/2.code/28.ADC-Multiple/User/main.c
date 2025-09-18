#include "bsp_init.h"
#include "adc.h"
#include <stdio.h>

// 定义ADC DMA缓冲区
#define ADC_DMA_SAMPLES_PER_CHANNEL 50 // 单通道时DMA缓冲区大小
#define ADC_DMA_BUFFER_SIZE (ADC_DMA_SAMPLES_PER_CHANNEL * MULTI_ADC_CHANNEL_NUM) // 多通道时DMA缓冲区大小：单通道大小 * 通道数
uint16_t g_adc_dma_buffer[ADC_DMA_BUFFER_SIZE]; // DMA缓冲区

// LCD显示文本
const char *lcd_title[] = {
    "STM32F407",
    "ADC 6CH DMA TEST",
    "There are 6 channels"
};

const char *lcd_channel_info_val[] = {
    "ADC1_CH0_VAL:",
    "ADC1_CH1_VAL:",
    "ADC1_CH2_VAL:",
    "ADC1_CH3_VAL:",
    "ADC1_CH4_VAL:",
    "ADC1_CH5_VAL:",
};

const char *lcd_channel_info_vol[] = {
    "ADC1_CH0_VOL:0.000V",
    "ADC1_CH1_VOL:0.000V",
    "ADC1_CH2_VOL:0.000V",
    "ADC1_CH3_VOL:0.000V",
    "ADC1_CH4_VOL:0.000V",
    "ADC1_CH5_VOL:0.000V",
};

int main(void)
{
    uint32_t channel_sum;  // 通道数
    uint16_t adc_raw_value; // ADC原始值
    float voltage_value; // 计算得来电压值
    
    // 启动ADC DMA采集
    bsp_init();
    adc_multi_channel_dma_init();
    adc_multi_channel_dma_enable(ADC_DMA_BUFFER_SIZE);

    // 初始化LCD显示
    LCD_ShowString(30, 50, 200, 16, 16, (char*)lcd_title[0]);
    LCD_ShowString(30, 70, 200, 16, 16, (char*)lcd_title[1]);
    LCD_ShowString(30, 90, 200, 16, 16, (char*)lcd_title[2]);

    for (uint8_t i = 0; i < MULTI_ADC_CHANNEL_NUM; i++)
    {
        LCD_ShowString(30, 110 + (i * 30), 200, 12, 12, (char*)lcd_channel_info_val[i]);
        LCD_ShowString(30, 122 + (i * 30), 200, 12, 12, (char*)lcd_channel_info_vol[i]);
    }

    while (1)
    {
        if(g_adc_dma_transfer_complete_flag) // DMA采集完成
        {
            // 处理并显示每个通道的数据
            for (uint8_t channel_index = 0; channel_index < MULTI_ADC_CHANNEL_NUM; channel_index++)
            {
                channel_sum = 0; 
                // 计算每个通道的平均值
                for (uint16_t sample_index = 0; sample_index < ADC_DMA_SAMPLES_PER_CHANNEL; sample_index++)
                {
                    channel_sum += g_adc_dma_buffer[sample_index * MULTI_ADC_CHANNEL_NUM + channel_index];
                }
                adc_raw_value = channel_sum / ADC_DMA_SAMPLES_PER_CHANNEL;
                // 显示ADC原始值
                LCD_ShowxNum(108, 110 + (channel_index * 30), adc_raw_value, 4, 12, 0);
                // 计算并显示电压值
                voltage_value = (float)adc_raw_value * (3.3f / 4096.0f);
                // 显示电压值
                uint16_t integer_part = (uint16_t)voltage_value;
                uint16_t decimal_part = (uint16_t)((voltage_value - integer_part) * 1000);
                LCD_ShowxNum(108, 122 + (channel_index * 30), integer_part, 1, 12, 0);
                LCD_ShowxNum(120, 122 + (channel_index * 30), decimal_part, 3, 12, 0X80);
            }
            g_adc_dma_transfer_complete_flag = 0; // 开始下一轮
        }
        LED_TOGGLE(LED0_GPIO_Pin);
        HAL_Delay(100);
    }
}
