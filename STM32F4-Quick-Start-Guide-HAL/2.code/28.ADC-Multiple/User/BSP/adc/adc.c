#include "delay.h"
#include "adc.h"
#include "led.h"

ADC_HandleTypeDef g_adc_handle;
DMA_HandleTypeDef g_dma_adc_handle;
uint8_t g_adc_dma_transfer_complete_flag = 0;

// 外部声明缓冲区
extern uint16_t g_adc_dma_buffer[];

// 多通道 GPIO 初始化
void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
    if (hadc->Instance == ADC_INSTANCE)
    {
        GPIO_InitTypeDef GPIO_InitStructure;
        ADC_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        GPIO_InitStructure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | 
                              GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5;
        GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStructure.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
    }
}

// 设置ADC通道采样时间
void adc_set_channel_config(ADC_HandleTypeDef *hadc, uint32_t channel, uint32_t rank, uint32_t sample_time)
{
    ADC_ChannelConfTypeDef adc_channel_config;
    adc_channel_config.Channel = channel; // 通道
    adc_channel_config.Rank = rank; // 通道序号
    adc_channel_config.SamplingTime = sample_time; // 采样时间
    HAL_ADC_ConfigChannel(hadc, &adc_channel_config);
}

// ADC多通道DMA初始化函数
void adc_multi_channel_dma_init(void)
{
    // 使能ADC时钟和DMA时钟
    __HAL_RCC_ADC1_CLK_ENABLE();
    __HAL_RCC_DMA2_CLK_ENABLE();

    // 初始化ADC
    g_adc_handle.Instance = ADC_INSTANCE; // ADC1
    g_adc_handle.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4; // 4分频
    g_adc_handle.Init.Resolution = ADC_RESOLUTION_12B; // 12位精度
    g_adc_handle.Init.DataAlign = ADC_DATAALIGN_RIGHT; // 数据右对齐
    g_adc_handle.Init.ScanConvMode = ENABLE; // 扫描模式
    g_adc_handle.Init.ContinuousConvMode = ENABLE; // 连续转换模式
    g_adc_handle.Init.DiscontinuousConvMode = DISABLE; // 不连续转换模式
    g_adc_handle.Init.NbrOfConversion = MULTI_ADC_CHANNEL_NUM; // 转换通道数-6
    g_adc_handle.Init.NbrOfDiscConversion = 0; // 不连续转换通道数
    g_adc_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START; // 软件触发
    g_adc_handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE; // 无外部触发信号
    g_adc_handle.Init.DMAContinuousRequests = ENABLE; // DMA连续请求模式
    HAL_ADC_Init(&g_adc_handle);

    // 初始化DMA
    g_dma_adc_handle.Instance = ADC_DMA_STREAM; // DMA2_Stream4
    g_dma_adc_handle.Init.Channel = ADC_DMA_CHANNEL; // DMA_CHANNEL_0
    g_dma_adc_handle.Init.Direction = DMA_PERIPH_TO_MEMORY; // 从外设到内存
    g_dma_adc_handle.Init.PeriphInc = DMA_PINC_DISABLE; // 外设地址不增
    g_dma_adc_handle.Init.MemInc = DMA_MINC_ENABLE; // 内存地址增
    g_dma_adc_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    g_dma_adc_handle.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    g_dma_adc_handle.Init.Mode = DMA_CIRCULAR; // 循环模式
    g_dma_adc_handle.Init.Priority = DMA_PRIORITY_HIGH; // 高优先级
    g_dma_adc_handle.Init.FIFOMode = DMA_FIFOMODE_DISABLE; // 禁用FIFO
    HAL_DMA_Init(&g_dma_adc_handle);
    
    // 链接DMA到ADC
    __HAL_LINKDMA(&g_adc_handle, DMA_Handle, g_dma_adc_handle);

    // 设置多通道采样规则序列
    adc_set_channel_config(&g_adc_handle, ADC_CHANNEL_0, 1, ADC_SAMPLETIME_480CYCLES);
    adc_set_channel_config(&g_adc_handle, ADC_CHANNEL_1, 2, ADC_SAMPLETIME_480CYCLES);
    adc_set_channel_config(&g_adc_handle, ADC_CHANNEL_2, 3, ADC_SAMPLETIME_480CYCLES);
    adc_set_channel_config(&g_adc_handle, ADC_CHANNEL_3, 4, ADC_SAMPLETIME_480CYCLES);
    adc_set_channel_config(&g_adc_handle, ADC_CHANNEL_4, 5, ADC_SAMPLETIME_480CYCLES);
    adc_set_channel_config(&g_adc_handle, ADC_CHANNEL_5, 6, ADC_SAMPLETIME_480CYCLES);

    // 配置DMA中断
    HAL_NVIC_SetPriority(ADC_DMA_IRQn, 0, 0);  // 最高优先级
    HAL_NVIC_EnableIRQ(ADC_DMA_IRQn);
}

// 使能ADC DMA传输
void adc_multi_channel_dma_enable(uint16_t num_transfers)
{
    // 启动ADC DMA传输
    HAL_ADC_Start_DMA(&g_adc_handle, (uint32_t*)g_adc_dma_buffer, num_transfers);
    // 软件触发启动转换
    SET_BIT(g_adc_handle.Instance->CR2, ADC_CR2_SWSTART);
}

// ADD DMA中断回调函数
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    if (hadc->Instance == ADC_INSTANCE) // 确保是ADC1
    {
        static uint32_t callback_count = 0;
        callback_count++;
        g_adc_dma_transfer_complete_flag = 1; // 标记DMA传输完成
        if(callback_count == 100)
        {
            LED_TOGGLE(LED1_GPIO_Pin); // 测试用
            callback_count = 0;
        }
    }
}

// DMA中断函数
void DMA2_Stream4_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&g_dma_adc_handle);
}
