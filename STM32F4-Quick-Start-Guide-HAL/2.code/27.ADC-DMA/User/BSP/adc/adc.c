#include "adc.h"
#include "delay.h"

ADC_HandleTypeDef adc_handle;
DMA_HandleTypeDef dma_handle;
uint8_t adc_dma_sta = 0; // 0:未完成，1:完成

void adc_init(void)
{
    adc_handle.Instance = ADC1;
    adc_handle.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4; // 4分频
    adc_handle.Init.Resolution = ADC_RESOLUTION_12B; // 12位精度
    adc_handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    adc_handle.Init.ScanConvMode = DISABLE; // 单通道扫描模式禁用
    adc_handle.Init.ContinuousConvMode = DISABLE; // 连续转换模式禁用 (在DMA模式下会通过CR2_CONT使能)
    adc_handle.Init.NbrOfConversion = 1; // 单次转换
    adc_handle.Init.DiscontinuousConvMode = DISABLE;
    adc_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    adc_handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    adc_handle.Init.DMAContinuousRequests = ENABLE; // 使能DMA连续请求 (ADC生成DMA请求后，DMA会持续请求直到传输完成)
    if (HAL_ADC_Init(&adc_handle) != HAL_OK)
    {
        // 初始化失败处理，例如错误打印或死循环
        while(1);
    }
}

// ADC通道配置
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
    if(hadc->Instance == ADC1)
    {
        GPIO_InitTypeDef GPIO_Initure;
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_ADC1_CLK_ENABLE();
        /* PA5 作为ADC输入 */
        GPIO_Initure.Pin = GPIO_PIN_5;
        GPIO_Initure.Mode = GPIO_MODE_ANALOG; // 模拟输入模式
        GPIO_Initure.Pull = GPIO_NOPULL; // 无上拉/下拉
        HAL_GPIO_Init(GPIOA, &GPIO_Initure);
        // 关联ADC和DMA中断
        HAL_NVIC_SetPriority(DMA2_Stream4_IRQn, 2, 2); // DMA中断优先级
        HAL_NVIC_EnableIRQ(DMA2_Stream4_IRQn);
    }
}

/**
 * @brief       设置ADC通道采样时间
 * @param       adc_handle : adc句柄指针,ADC_HandleTypeDef
 * @param       ch   : 通道号, ADC_CHANNEL_0~ADC_CHANNEL_17
 * @param       rank: 多通道采集时需要设置的采集编号, 单通道DMA设置为 ADC_REGULAR_RANK_1
 * @param       stime: 采样时间
 * @retval      无
 */
void adc_channel_set(ADC_HandleTypeDef *adc_handle, uint32_t ch, uint32_t rank, uint32_t stime)
{
    ADC_ChannelConfTypeDef adc_channel;
    adc_channel.Channel = ch;         // 设置ADCX对通道ch
    adc_channel.Rank = rank;          // 设置采样序列
    adc_channel.SamplingTime = stime; // 设置采样时间
    if (HAL_ADC_ConfigChannel(adc_handle, &adc_channel) != HAL_OK)
    {
        // 通道配置失败处理
        while(1);
    }
}

// 获取ADC通道ch的转换值 (单次轮询模式，与DMA采集不冲突，可独立使用)
uint32_t adc_get_result(uint32_t ch)
{
    // 在这里重新配置通道，可能会影响DMA模式下的通道设置，通常不建议在DMA进行时调用
    // 如果只需要单次采集，可以创建一个独立的ADC句柄或者确保DMA已停止
    ADC_ChannelConfTypeDef s_config;
    s_config.Channel = ch;
    s_config.Rank = 1;
    s_config.SamplingTime = ADC_SAMPLETIME_480CYCLES;
    if (HAL_ADC_ConfigChannel(&adc_handle, &s_config) != HAL_OK)
    {
        while(1);
    }

    HAL_ADC_Start(&adc_handle); // 开启ADC
    HAL_ADC_PollForConversion(&adc_handle, 10); // 轮询转换，超时时间10ms
    return (uint16_t)HAL_ADC_GetValue(&adc_handle); // 返回最近一次ADC1规则组的转换结果
}

// 通道ch的times次转换结果平均值 (单次轮询模式，同上)
uint32_t adc_get_result_average(uint32_t ch, uint8_t times)
{
    uint32_t temp_val = 0;
    uint8_t t;
    for (t = 0; t < times; t++)
    {
        temp_val += adc_get_result(ch);
        delay_ms(5); // 稍微延迟一下，避免连续过快采样
    }
    return temp_val / times;
}

// DMA配置
void adc_dma_init(uint32_t address)
{
    // 确保ADC先初始化，因为DMA需要关联到ADC句柄
    adc_init();

    // 确定DMA时钟使能
    __HAL_RCC_DMA2_CLK_ENABLE(); // 对于DMA2_Stream4，直接使能DMA2时钟即可

    dma_handle.Instance = DMA2_Stream4;
    dma_handle.Init.Channel = DMA_CHANNEL_0; // 对应ADC1的DMA请求，通常是通道0
    dma_handle.Init.Direction = DMA_PERIPH_TO_MEMORY; // 外设到存储器
    dma_handle.Init.PeriphInc = DMA_PINC_DISABLE; // 外设地址不递增
    dma_handle.Init.MemInc = DMA_MINC_ENABLE; // 存储器地址递增
    dma_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD; // 外设数据宽度半字 (16位)
    dma_handle.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD; // 存储器数据宽度半字 (16位)
    dma_handle.Init.Mode = DMA_CIRCULAR; 
    dma_handle.Init.Priority = DMA_PRIORITY_MEDIUM; // DMA优先级中等
    dma_handle.Init.FIFOMode = DMA_FIFOMODE_DISABLE; // 禁用FIFO
    dma_handle.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL; // FIFO阈值
    dma_handle.Init.MemBurst = DMA_MBURST_SINGLE; // 存储器突发模式单次传输
    dma_handle.Init.PeriphBurst = DMA_PBURST_SINGLE; // 外设突发模式单次传输

    if (HAL_DMA_Init(&dma_handle) != HAL_OK)
    {
        // DMA初始化失败处理
        while(1);
    }

    // 关联DMA句柄到ADC句柄
    __HAL_LINKDMA(&adc_handle, DMA_Handle, dma_handle);

    // 设置ADC通道用于DMA传输 (这里我们只用一个通道，PA5/ADC_CHANNEL_5)
    adc_channel_set(&adc_handle, ADC_CHANNEL_5, 1, ADC_SAMPLETIME_56CYCLES); // 设置通道，序列和采样时间

    // 开启ADC连续转换模式（对于DMA通常需要，以便ADC自动开始下一次转换）
    SET_BIT(adc_handle.Instance->CR2, ADC_CR2_CONT); // 使能连续转换

    // 启动DMA ADC转换
    // 第三个参数是数据传输的次数。在DMA_CIRCULAR模式下，DMA会自动循环。
    if (HAL_ADC_Start_DMA(&adc_handle, (uint32_t*)address, ADC_DMA_BUFFSIZE) != HAL_OK)
    {
        // 启动DMA转换失败处理
        while(1);
    }

    // 开启DMA传输完成中断 (在DMA_CIRCULAR模式下，每次缓冲区填充满时会触发)
    __HAL_DMA_ENABLE_IT(&dma_handle, DMA_IT_TC);
}

// 在DMA_CIRCULAR模式下，此函数通常只用于首次启动DMA，或从停止状态重新启动。
// 一旦DMA以CIRCULAR模式启动，它会自行循环，无需重复调用。
void adc_dma_enable(uint16_t times)
{
    // 检查DMA是否已经处于活跃状态，如果已经活跃，则不执行任何操作。
    // 如果DMA处于HAL_DMA_STATE_READY或HAL_DMA_STATE_BUSY_TC，则不应再次启动。
    // 这里简单地禁用和重新使能，以确保状态被重置。
    // 在DMA_CIRCULAR模式下，HAL_ADC_Start_DMA 已经启动了，所以这个函数可能变得多余。
    // 如果您想使用它来停止和重新启动，可以保留逻辑。
    __HAL_ADC_DISABLE(&adc_handle); // 禁用ADC
    __HAL_DMA_DISABLE(&dma_handle); // 禁用DMA

    // 在DMA_CIRCULAR模式下，NDTR会被HAL_ADC_Start_DMA设置，并自动重置。
    // 重新设置NDTR在这里可能没有必要，甚至可能导致问题，因为HAL_ADC_Start_DMA会处理它。
    // dma_handle.Instance->NDTR = times; // 考虑注释掉或删除此行

    __HAL_DMA_ENABLE(&dma_handle); // 重新使能DMA
    __HAL_ADC_ENABLE(&adc_handle); // 重新使能ADC

    // 启动ADC规则组转换（软件触发），这将启动DMA传输
    SET_BIT(adc_handle.Instance->CR2, ADC_CR2_SWSTART); // 启动规则转换通道
}


// DMA中断函数
void DMA2_Stream4_IRQHandler(void)
{
    // HAL库中断处理函数会自动处理DMA中断标志和调用回调函数
    HAL_DMA_IRQHandler(&dma_handle);
}

// DMA传输完成回调函数
// 在DMA_CIRCULAR模式下，每次整个缓冲区被填充完毕后，此回调函数都会被调用。
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    if (hadc->Instance == ADC1)
    {
        adc_dma_sta = 1; // 完成标志位置1
        // 在DMA_CIRCULAR模式下，DMA会自动重新开始传输，无需在此处手动重新启动。
    }
}
