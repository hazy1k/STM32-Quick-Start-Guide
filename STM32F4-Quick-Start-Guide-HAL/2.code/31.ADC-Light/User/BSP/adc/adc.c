#include "adc.h"
#include "delay.h"

ADC_HandleTypeDef adc_handle;

void adc_init(void)
{
    __HAL_RCC_ADC3_CLK_ENABLE();
    adc_handle.Instance = ADC3;
    adc_handle.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4; // 4分频
    adc_handle.Init.Resolution = ADC_RESOLUTION_12B; // 12位精度
    adc_handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    adc_handle.Init.ScanConvMode = DISABLE;
    adc_handle.Init.EOCSelection = DISABLE;
    adc_handle.Init.ContinuousConvMode = DISABLE;
    adc_handle.Init.NbrOfConversion = 1;
    adc_handle.Init.DiscontinuousConvMode = DISABLE;
    adc_handle.Init.NbrOfDiscConversion = 0;
    adc_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    adc_handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    HAL_ADC_Init(&adc_handle);
}

/**
 * @brief       设置ADC通道采样时间
 * @param       adcx : adc句柄指针,ADC_HandleTypeDef
 * @param       ch   : 通道号, ADC_CHANNEL_0~ADC_CHANNEL_17
 * @param       stime: 采样时间  0~7, 对应关系为:
 *   @arg       ADC_SAMPLETIME_3CYCLES,  3个ADC时钟周期        ADC_SAMPLETIME_15CYCLES, 15个ADC时钟周期
 *   @arg       ADC_SAMPLETIME_28CYCLES, 28个ADC时钟周期       ADC_SAMPLETIME_56CYCLES, 56个ADC时钟周期
 *   @arg       ADC_SAMPLETIME_84CYCLES, 84个ADC时钟周期       ADC_SAMPLETIME_112CYCLES,112个ADC时钟周期
 *   @arg       ADC_SAMPLETIME_144CYCLES,144个ADC时钟周期      ADC_SAMPLETIME_480CYCLES,480个ADC时钟周期
 * @param       rank: 多通道采集时需要设置的采集编号,
                假设你定义channel1的rank=1，channel2的rank=2，
                那么对应你在DMA缓存空间的变量数组AdcDMA[0] 就i是channel1的转换结果，AdcDMA[1]就是通道2的转换结果。 
                单通道DMA设置为 ADC_REGULAR_RANK_1
 *   @arg       编号1~16：ADC_REGULAR_RANK_1~ADC_REGULAR_RANK_16
 * @retval      无
 */
void adc_channel_set(ADC_HandleTypeDef *adc_handle, uint32_t ch, uint32_t rank, uint32_t stime)
{
    ADC_ChannelConfTypeDef adc_channel;
    adc_channel.Channel = ch;         // 设置ADCX对通道ch
    adc_channel.Rank = rank;          // 设置采样序列
    adc_channel.SamplingTime = stime; // 设置采样时间
    HAL_ADC_ConfigChannel( adc_handle, &adc_channel);   
}

// 获取ADC通道ch的转换值 
uint32_t adc_get_result(uint32_t ch)
{
    adc_channel_set(&adc_handle, ch, 1, ADC_SAMPLETIME_480CYCLES); // 设置通道，序列和采样时间
    HAL_ADC_Start(&adc_handle); // 开启ADC
    HAL_ADC_PollForConversion(&adc_handle, 10); // 轮询转换

    return (uint16_t)HAL_ADC_GetValue(&adc_handle); // 返回最近一次ADC1规则组的转换结果
}

// 通道ch的times次转换结果平均值
uint32_t adc_get_result_average(uint32_t ch, uint8_t times)
{
    uint32_t temp_val = 0;
    uint8_t t;

    for (t = 0; t < times; t++)  
    {
        temp_val += adc_get_result(ch);
        delay_ms(5);
    }
    return temp_val / times;     
}

// 初始化光敏传感器-PF7
void light_sensor_init(void)
{
    GPIO_InitTypeDef gpio_init_structure;
    __HAL_RCC_GPIOF_CLK_ENABLE();
    gpio_init_structure.Pin = GPIO_PIN_7;
    gpio_init_structure.Mode = GPIO_MODE_ANALOG;
    gpio_init_structure.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOF, &gpio_init_structure);
    adc_init();
}

// 读取光敏传感器值
uint8_t light_sensor_read(void)
{
    uint32_t temp_val = 0;
    temp_val = adc_get_result_average(ADC_CHANNEL_5, 10);
    temp_val /= 40;
    if(temp_val > 100)
    {
        temp_val = 100;
    }
    return (uint8_t)(100-temp_val);
}

