#include "dac.h"
#include "delay.h"

DAC_HandleTypeDef dac_handle;

void dac_init(uint8_t outx)
{
    __HAL_RCC_DAC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = (outx==1)? GPIO_PIN_4 : GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    dac_handle.Instance = DAC;
    HAL_DAC_Init(&dac_handle);
    DAC_ChannelConfTypeDef dac_ch_handle;
    dac_ch_handle.DAC_Trigger = DAC_TRIGGER_NONE;
    dac_ch_handle.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;

    switch (outx)
    {
    case 1:
        HAL_DAC_ConfigChannel(&dac_handle, &dac_ch_handle, DAC_CHANNEL_1);
        HAL_DAC_Start(&dac_handle, DAC_CHANNEL_1);
        break;
    
    case 2:
        HAL_DAC_ConfigChannel(&dac_handle, &dac_ch_handle, DAC_CHANNEL_2);
        HAL_DAC_Start(&dac_handle, DAC_CHANNEL_2);
        break;
    default:
        break;
    }
}

// 设置输出通道电压
void dac_set_voltage(uint8_t outx, uint16_t voltage)
{
    double temp = voltage;
    temp /= 1000;
    temp = temp*4095/3.3;
    if(temp > 4095)
        temp = 4095;
    if(outx)
    {
        HAL_DAC_SetValue(&dac_handle, DAC_CHANNEL_1, DAC_ALIGN_12B_R, temp); // 设置DAC输出值
    }   
    else
    {
        HAL_DAC_SetValue(&dac_handle, DAC_CHANNEL_2, DAC_ALIGN_12B_R, temp);
    }
}

