/**
 ****************************************************************************************************
 * @file        adc.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.3
 * @date        2020-03-26
 * @brief       ADC 驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 STM32H750开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.0 20200326
 * 第一次发布
 * V1.1 20200326
 * 1,支持ADC单通道DMA采集 
 * 2,新增adc_dma_init和adc_dma_enable函数
 * V1.2 20200327
 * 1,支持ADC多通道DMA采集
 * 2,新增adc_nch_dma_init和adc_nch_dma_gpio_init函数
 * V1.3 20200327
 * 1,支持ADC单通道过采样
 * 2,新增adc_oversample_init函数
 *
 ****************************************************************************************************
 */

#include "./BSP/ADC/adc.h"
#include "./BSP/DMA/dma.h"
#include "./SYSTEM/delay/delay.h"


ADC_HandleTypeDef g_adc_handle;   /* ADC句柄 */

/**
 * @brief       ADC初始化函数
 *   @note      本函数支持ADC1/ADC2任意通道,但是不支持ADC3
 *              我们使用16位精度, ADC采样时钟=32M, 转换时间为:采样周期 + 8.5个ADC周期
 *              设置最大采样周期: 810.5, 则转换时间 = 819个ADC周期 = 25.6us
 * @param       无
 * @retval      无
 */
void adc_init(void)
{
    ADC_ADCX_CHY_CLK_ENABLE();                   /* 使能ADC1/2时钟 */
    __HAL_RCC_ADC_CONFIG(RCC_ADCCLKSOURCE_CLKP); /* ADC外设时钟选择 */

    adc_gpio_init();                             /* GPIO初始化 */

    g_adc_handle.Instance = ADC_ADCX;
    g_adc_handle.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV2;                /* 输入时钟2分频,即adc_ker_ck=per_ck/2=32Mhz */
    g_adc_handle.Init.Resolution = ADC_RESOLUTION_16B;                      /* 16位模式  */
    g_adc_handle.Init.ScanConvMode = ADC_SCAN_DISABLE;                      /* 非扫描模式 */
    g_adc_handle.Init.EOCSelection = ADC_EOC_SINGLE_CONV;                   /* 关闭EOC中断 */
    g_adc_handle.Init.LowPowerAutoWait = DISABLE;                           /* 自动低功耗关闭 */
    g_adc_handle.Init.ContinuousConvMode = DISABLE;                         /* 关闭连续转换 */
    g_adc_handle.Init.NbrOfConversion = 1;                                  /* 使用了1个转换通道 */
    g_adc_handle.Init.DiscontinuousConvMode = DISABLE;                      /* 禁止不连续采样模式 */
    g_adc_handle.Init.NbrOfDiscConversion = 0;                              /* 禁止不连续模式后，此参数忽略，不连续采样通道数为0 */
    g_adc_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;                /* 软件触发 */
    g_adc_handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE; /* 采用软件触发的话，此位忽略 */
    g_adc_handle.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;                   /* 有新的数据后直接覆盖掉旧数据 */
    g_adc_handle.Init.OversamplingMode = DISABLE;                           /* 过采样关闭 */
    g_adc_handle.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DR;     /* 常规通道的数据仅仅保存在DR寄存器里面 */
    HAL_ADC_Init(&g_adc_handle);                                            /* 初始化 */

    HAL_ADCEx_Calibration_Start(&g_adc_handle, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED); /* ADC校准 */
}

/**
 * @brief       ADC的gpio初始化函数
 * @param       无
 * @note        此函数会被adc_init()调用
 * @retval      无
 */
void adc_gpio_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;

    ADC_ADCX_CHY_GPIO_CLK_ENABLE();                  /* 开启ADC通道IO引脚时钟 */

    gpio_init_struct.Pin = ADC_ADCX_CHY_GPIO_PIN;    /* ADC通道IO引脚 */
    gpio_init_struct.Mode = GPIO_MODE_ANALOG;        /* 模拟 */
    gpio_init_struct.Pull = GPIO_NOPULL;             /* 不带上下拉 */
    HAL_GPIO_Init(ADC_ADCX_CHY_GPIO_PORT, &gpio_init_struct);
}

/**
 * @brief       获得ADC转换后的结果 
 * @param       ch: 通道值 0~19，取值范围为：ADC_CHANNEL_0~ADC_CHANNEL_19
 * @retval      返回值:转换结果
 */
uint32_t adc_get_result(uint32_t ch)   
{
    ADC_ChannelConfTypeDef ADC1_ChanConf;

    ADC1_ChanConf.Channel = ch;                              /* 通道 */
    ADC1_ChanConf.Rank = ADC_REGULAR_RANK_1;                 /* 1个序列 */
    ADC1_ChanConf.SamplingTime = ADC_SAMPLETIME_810CYCLES_5; /* 采样时间，设置最大采样周期: 810.5个ADC周期 */
    ADC1_ChanConf.SingleDiff = ADC_SINGLE_ENDED;             /* 单边采集 */
    ADC1_ChanConf.OffsetNumber = ADC_OFFSET_NONE;            /* 不使用偏移量的通道 */
    ADC1_ChanConf.Offset = 0;                                /* 偏移量为0 */
    HAL_ADC_ConfigChannel(&g_adc_handle ,&ADC1_ChanConf);    /* 通道配置 */

    HAL_ADC_Start(&g_adc_handle);                            /* 开启ADC */
    HAL_ADC_PollForConversion(&g_adc_handle, 10);            /* 轮询转换 */
    return HAL_ADC_GetValue(&g_adc_handle);                  /* 返回最近一次ADC1规则组的转换结果 */
}

/**
 * @brief       获取通道ch的转换值，取times次,然后平均
 * @param       ch      : 通道号, 0~19
 * @param       times   : 获取次数
 * @retval      通道ch的times次转换结果平均值
 */
uint32_t adc_get_result_average(uint32_t ch, uint8_t times)
{
    uint32_t temp_val = 0;
    uint8_t t;

    for (t = 0; t < times; t++) /* 获取times次数据 */
    {
        temp_val += adc_get_result(ch);
        delay_ms(5);
    }

    return temp_val / times;    /* 返回平均值 */
}

/***************************************单通道ADC采集(DMA读取)程序*****************************************/

ADC_HandleTypeDef g_adc_dma_handle;     /* 与DMA关联的ADC句柄 */
DMA_HandleTypeDef g_dma_adc_handle;     /* 与ADC关联的DMA句柄 */

uint8_t g_adc_dma_sta = 0;              /* DMA传输状态标志, 0,未完成; 1, 已完成 */

/**
 * @brief       ADC DMA读取 初始化函数
 * @param       par         : 外设地址
 * @param       mar         : 存储器地址 
 * @retval      无
 */
void adc_dma_init(uint32_t par, uint32_t mar)
{
    ADC_ChannelConfTypeDef adc_ch_conf;
    
    ADC_ADCX_CHY_CLK_ENABLE();                         /* 使能ADC1/2时钟 */
    
    if ((uint32_t)ADC_ADCX_DMASx > (uint32_t)DMA2)     /* 得到当前stream是属于DMA2还是DMA1 */
    {
        __HAL_RCC_DMA2_CLK_ENABLE();                   /* DMA2时钟使能 */
    }
    else 
    {
        __HAL_RCC_DMA1_CLK_ENABLE();                   /* DMA1时钟使能 */
    }
    
    __HAL_RCC_ADC_CONFIG(RCC_ADCCLKSOURCE_CLKP);       /* ADC外设时钟选择 */
    
    adc_gpio_init();                                   /* GPIO初始化 */
    
    g_adc_dma_handle.Instance = ADC_ADCX;
    g_adc_dma_handle.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;                 /* 输入时钟2分频,即adc_ker_ck=per_ck/2=32Mhz */
    g_adc_dma_handle.Init.Resolution = ADC_RESOLUTION_16B;                           /* 16位模式  */
    g_adc_dma_handle.Init.ScanConvMode = ADC_SCAN_DISABLE;                           /* 非扫描模式 */
    g_adc_dma_handle.Init.EOCSelection = ADC_EOC_SINGLE_CONV;                        /* 关闭EOC中断 */
    g_adc_dma_handle.Init.LowPowerAutoWait = DISABLE;                                /* 自动低功耗关闭 */
    g_adc_dma_handle.Init.ContinuousConvMode = ENABLE;                               /* 使能连续转换模式 */
    g_adc_dma_handle.Init.NbrOfConversion = 1;                                       /* 使用了1个转换通道 */
    g_adc_dma_handle.Init.DiscontinuousConvMode = DISABLE;                           /* 禁止不连续采样模式 */
    g_adc_dma_handle.Init.NbrOfDiscConversion = 0;                                   /* 禁止不连续模式后，此参数忽略，不连续采样通道数为0 */
    g_adc_dma_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;                     /* 采用软件触发 */
    g_adc_dma_handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;      /* 采用软件触发的话，此位忽略 */
    g_adc_dma_handle.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;                        /* 有新的数据后直接覆盖掉旧数据 */
    g_adc_dma_handle.Init.OversamplingMode = DISABLE;                                /* 过采样关闭 */
    g_adc_dma_handle.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_ONESHOT; /* DMA单次传输ADC数据 */
    HAL_ADC_Init(&g_adc_dma_handle);                                                 /* 初始化 */

    HAL_ADCEx_Calibration_Start(&g_adc_dma_handle, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED); /* ADC校准 */

    g_dma_adc_handle.Instance = ADC_ADCX_DMASx;                           /* 使用DMA1 Stream7 */
    g_dma_adc_handle.Init.Request = ADC_ADCX_DMASx_REQ;                   /* 请求类型采用DMA_REQUEST_ADC1 */
    g_dma_adc_handle.Init.Direction = DMA_PERIPH_TO_MEMORY;               /* 传外设到存储器模式 */
    g_dma_adc_handle.Init.PeriphInc = DMA_PINC_DISABLE;                   /* 外设非增量模式 */
    g_dma_adc_handle.Init.MemInc = DMA_MINC_ENABLE;                       /* 存储器增量模式 */
    g_dma_adc_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;  /* 外设数据长度:16位 */
    g_dma_adc_handle.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;     /* 存储器数据长度:16位 */
    g_dma_adc_handle.Init.Mode = DMA_CIRCULAR;                            /* 循环模式 */
    g_dma_adc_handle.Init.Priority = DMA_PRIORITY_MEDIUM;                 /* 中等优先级 */
    g_dma_adc_handle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;                /* 禁止FIFO*/
    HAL_DMA_Init(&g_dma_adc_handle);                                      /* 初始化DMA */

    __HAL_LINKDMA(&g_adc_dma_handle, DMA_Handle, g_dma_adc_handle);       /* 将DMA与adc联系起来 */

    HAL_NVIC_SetPriority(ADC_ADCX_DMASx_IRQn, 3, 3);
    HAL_NVIC_EnableIRQ(ADC_ADCX_DMASx_IRQn);

    adc_ch_conf.Channel = ADC_ADCX_CHY;                    /* 配置使用的ADC通道 */
    adc_ch_conf.Rank = ADC_REGULAR_RANK_1;                 /* 采样序列里的第1个 */
    adc_ch_conf.SamplingTime = ADC_SAMPLETIME_810CYCLES_5; /* 采样周期为810.5个时钟周期 */
    adc_ch_conf.SingleDiff = ADC_SINGLE_ENDED ;            /* 单端输入 */
    adc_ch_conf.OffsetNumber = ADC_OFFSET_NONE;            /* 无偏移 */
    adc_ch_conf.Offset = 0;                                /* 无偏移的情况下，此参数忽略 */
    adc_ch_conf.OffsetRightShift       = DISABLE;          /* 禁止右移 */
    adc_ch_conf.OffsetSignedSaturation = DISABLE;          /* 禁止有符号饱和 */
    HAL_ADC_ConfigChannel(&g_adc_dma_handle, &adc_ch_conf);/* 配置ADC通道 */

    ADC_Enable(&g_adc_dma_handle);                         /* 使能ADC */
    HAL_DMA_Start_IT(&g_dma_adc_handle, par, mar, 0);      /* 配置DMA传输参数 */
}

/***************************************多通道ADC采集(DMA读取)程序*****************************************/

ADC_HandleTypeDef g_adc_nch_dma_handle;     /* 与DMA关联的ADC句柄 */
DMA_HandleTypeDef g_dma_nch_adc_handle;     /* 与ADC关联的DMA句柄 */

/**
 * @brief       ADC N通道(6通道) DMA读取 初始化函数
 *   @note      由于本函数用到了6个通道, 宏定义会比较多内容, 因此,本函数就不采用宏定义的方式来修改通道了,
 *              直接在本函数里面修改, 这里我们默认使用PA0~PA5这6个通道.
 *
 *              注意: 本函数还是使用 ADC_ADCX(默认=ADC1) 和 ADC_ADCX_DMASx(默认=DMA1_Stream7) 及其相关定义
 *              不要乱修改adc.h里面的这两部分内容, 必须在理解原理的基础上进行修改, 否则可能导致无法正常使用.
 *
 * @param       par         : 外设地址
 * @param       mar         : 存储器地址 
 * @retval      无
 */
void adc_nch_dma_init(uint32_t par, uint32_t mar)
{
    ADC_ChannelConfTypeDef adc_ch_conf;
    
    ADC_ADCX_CHY_CLK_ENABLE();                         /* 使能ADC1/2时钟 */
    
    if ((uint32_t)ADC_ADCX_DMASx > (uint32_t)DMA2)     /* 得到当前stream是属于DMA2还是DMA1 */
    {
        __HAL_RCC_DMA2_CLK_ENABLE();                   /* DMA2时钟使能 */
    }
    else 
    {
        __HAL_RCC_DMA1_CLK_ENABLE();                   /* DMA1时钟使能 */
    }
    
    __HAL_RCC_ADC_CONFIG(RCC_ADCCLKSOURCE_CLKP);       /* ADC外设时钟选择 */
    
    adc_nch_dma_gpio_init();                           /* GPIO初始化 */
    
    g_adc_nch_dma_handle.Instance = ADC_ADCX;
    g_adc_nch_dma_handle.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;                /* 输入时钟2分频,即adc_ker_ck=per_ck/2=32Mhz */
    g_adc_nch_dma_handle.Init.Resolution = ADC_RESOLUTION_16B;                          /* 16位模式  */
    g_adc_nch_dma_handle.Init.ScanConvMode = ADC_SCAN_ENABLE;                           /* 扫描模式 */
    g_adc_nch_dma_handle.Init.EOCSelection = ADC_EOC_SINGLE_CONV;                       /* 关闭EOC中断 */
    g_adc_nch_dma_handle.Init.LowPowerAutoWait = DISABLE;                               /* 自动低功耗关闭 */
    g_adc_nch_dma_handle.Init.ContinuousConvMode = ENABLE;                              /* 使能连续转换 */
    g_adc_nch_dma_handle.Init.NbrOfConversion = 6;                                      /* 使用了6个转换通道 */
    g_adc_nch_dma_handle.Init.DiscontinuousConvMode = DISABLE;                          /* 禁止不连续采样模式 */
    g_adc_nch_dma_handle.Init.NbrOfDiscConversion = 0;                                  /* 禁止不连续模式后，此参数忽略，不连续采样通道数为0 */
    g_adc_nch_dma_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;                    /* 采用软件触发 */
    g_adc_nch_dma_handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;     /* 采用软件触发的话，此位忽略 */
    g_adc_nch_dma_handle.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;                       /* 有新的数据后直接覆盖掉旧数据 */
    g_adc_nch_dma_handle.Init.OversamplingMode = DISABLE;                               /* 过采样关闭 */
    g_adc_nch_dma_handle.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_ONESHOT;/* DMA单次传输ADC数据 */
    HAL_ADC_Init(&g_adc_nch_dma_handle);                                                /* 初始化 */

    HAL_ADCEx_Calibration_Start(&g_adc_nch_dma_handle, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED); /* ADC校准 */

    g_dma_nch_adc_handle.Instance = ADC_ADCX_DMASx;                           /* 使用DMA1 Stream7 */
    g_dma_nch_adc_handle.Init.Request = ADC_ADCX_DMASx_REQ;                   /* 请求类型采用DMA_REQUEST_ADC1 */
    g_dma_nch_adc_handle.Init.Direction = DMA_PERIPH_TO_MEMORY;               /* 传外设到存储器模式 */
    g_dma_nch_adc_handle.Init.PeriphInc = DMA_PINC_DISABLE;                   /* 外设非增量模式 */
    g_dma_nch_adc_handle.Init.MemInc = DMA_MINC_ENABLE;                       /* 存储器增量模式 */
    g_dma_nch_adc_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;  /* 外设数据长度:16位 */
    g_dma_nch_adc_handle.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;     /* 存储器数据长度:16位 */
    g_dma_nch_adc_handle.Init.Mode = DMA_CIRCULAR;                            /* 循环模式 */
    g_dma_nch_adc_handle.Init.Priority = DMA_PRIORITY_MEDIUM;                 /* 中等优先级 */
    g_dma_nch_adc_handle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;                /* 禁止FIFO*/
    HAL_DMA_Init(&g_dma_nch_adc_handle);                                      /* 初始化DMA */

    __HAL_LINKDMA(&g_adc_nch_dma_handle, DMA_Handle, g_dma_nch_adc_handle);   /* 将DMA与adc联系起来 */
    
    HAL_NVIC_SetPriority(ADC_ADCX_DMASx_IRQn, 3, 3);
    HAL_NVIC_EnableIRQ(ADC_ADCX_DMASx_IRQn);
    
    adc_ch_conf.Channel = ADC_CHANNEL_14;                       /* 配置使用的ADC通道 */
    adc_ch_conf.Rank = ADC_REGULAR_RANK_1;                      /* 采样序列里的第1个 */
    adc_ch_conf.SamplingTime = ADC_SAMPLETIME_810CYCLES_5;      /* 采样周期为810.5个时钟周期 */
    adc_ch_conf.SingleDiff = ADC_SINGLE_ENDED ;                 /* 单端输入 */
    adc_ch_conf.OffsetNumber = ADC_OFFSET_NONE;                 /* 无偏移 */
    adc_ch_conf.Offset = 0;                                     /* 无偏移的情况下，此参数忽略 */
    adc_ch_conf.OffsetRightShift       = DISABLE;               /* 禁止右移 */
    adc_ch_conf.OffsetSignedSaturation = DISABLE;               /* 禁止有符号饱和 */
    HAL_ADC_ConfigChannel(&g_adc_nch_dma_handle, &adc_ch_conf); /* 配置ADC通道 */

    adc_ch_conf.Channel = ADC_CHANNEL_15;                       /* 配置使用的ADC通道 */
    adc_ch_conf.Rank = ADC_REGULAR_RANK_2;                      /* 采样序列里的第2个 */
    HAL_ADC_ConfigChannel(&g_adc_nch_dma_handle, &adc_ch_conf); /* 配置ADC通道 */

    adc_ch_conf.Channel = ADC_CHANNEL_16;                       /* 配置使用的ADC通道 */
    adc_ch_conf.Rank = ADC_REGULAR_RANK_3;                      /* 采样序列里的第3个 */
    HAL_ADC_ConfigChannel(&g_adc_nch_dma_handle, &adc_ch_conf); /* 配置ADC通道 */

    adc_ch_conf.Channel = ADC_CHANNEL_17;                       /* 配置使用的ADC通道 */
    adc_ch_conf.Rank = ADC_REGULAR_RANK_4;                      /* 采样序列里的第4个 */
    HAL_ADC_ConfigChannel(&g_adc_nch_dma_handle, &adc_ch_conf); /* 配置ADC通道 */

    adc_ch_conf.Channel = ADC_CHANNEL_18;                       /* 配置使用的ADC通道 */
    adc_ch_conf.Rank = ADC_REGULAR_RANK_5;                      /* 采样序列里的第5个 */
    HAL_ADC_ConfigChannel(&g_adc_nch_dma_handle, &adc_ch_conf); /* 配置ADC通道 */

    adc_ch_conf.Channel = ADC_CHANNEL_19;                       /* 配置使用的ADC通道 */
    adc_ch_conf.Rank = ADC_REGULAR_RANK_6;                      /* 采样序列里的第6个 */
    HAL_ADC_ConfigChannel(&g_adc_nch_dma_handle, &adc_ch_conf); /* 配置ADC通道 */

    ADC_Enable(&g_adc_nch_dma_handle);                          /* 使能ADC */
    HAL_DMA_Start_IT(&g_dma_nch_adc_handle, par, mar, 0);       /* 配置DMA传输参数 */
}

/**
 * @brief       多通道ADC的gpio初始化函数
 * @param       无
 * @note        此函数会被adc_nch_dma_init()调用
 * @note        PA0-ADC_CHANNEL_16、PA1-ADC_CHANNEL_17、PA2-ADC_CHANNEL_14
                PA3-ADC_CHANNEL_15、PA4-ADC_CHANNEL_18、PA5-ADC_CHANNEL_19
* @retval       无
 */
void adc_nch_dma_gpio_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;

    __HAL_RCC_GPIOA_CLK_ENABLE();                    /* 开启GPIOA引脚时钟 */

    gpio_init_struct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5; /* GPIOA0~5 */
    gpio_init_struct.Mode = GPIO_MODE_ANALOG;        /* 模拟 */
    gpio_init_struct.Pull = GPIO_NOPULL;             /* 不带上下拉 */
    HAL_GPIO_Init(GPIOA, &gpio_init_struct);
}

/***************************************以上ADC实验公共部分程序*****************************************/

/**
 * @brief       使能一次ADC DMA传输
 *   @note      该函数用寄存器来操作，防止用HAL库操作对其他参数有修改
 * @param       ndtr: DMA传输的次数
 * @retval      无
 */
void adc_dma_enable(uint16_t ndtr)
{
    ADC_ADCX->CR &= ~(1 << 0);         /* 先关闭ADC */

    ADC_ADCX_DMASx->CR &= ~(1 << 0);   /* 关闭DMA传输 */
    while (ADC_ADCX_DMASx->CR & 0X1);  /* 确保DMA可以被设置 */
    ADC_ADCX_DMASx->NDTR = ndtr;       /* 要传输的数据项数目 */
    ADC_ADCX_DMASx->CR |= 1 << 0;      /* 开启DMA传输 */
    
    ADC_ADCX->CR |= 1 << 0;            /* 重新启动ADC */
    ADC_ADCX->CR |= 1 << 2;            /* 启动规则转换通道 */
}

/**
 * @brief       ADC DMA采集中断服务函数
 * @param       无 
 * @retval      无
 */
void ADC_ADCX_DMASx_IRQHandler(void)
{
    if (ADC_ADCX_DMASx_IS_TC())
    {
        g_adc_dma_sta = 1;          /* 标记DMA传输完成 */
        ADC_ADCX_DMASx_CLR_TC();    /* 清除DMA1 数据流7 传输完成中断 */
    }
}

/***************************************单通道ADC过采样(26位分辨率)程序*****************************************/

/**
 * @brief       ADC 过采样 初始化函数
 *   @note      本函数可以控制ADC过采样范围从1x ~ 1024x, 得到最高26位分辨率的AD转换结果
 * @param       osr : 过采样倍率, 0 ~ 1023, 表示:1x ~ 1024x过采样倍率
 * @param       ovss: 过采样右移位数, 0~11, 表示右移0位~11位.
 *   @note      过采样后, ADC的转换时间相应的会慢 osr倍.
 * @retval      无
 */
void adc_oversample_init(uint32_t osr, uint32_t ovss)
{
    ADC_ADCX_CHY_CLK_ENABLE();                   /* 使能ADC1/2时钟 */
    __HAL_RCC_ADC_CONFIG(RCC_ADCCLKSOURCE_CLKP); /* ADC外设时钟选择 */

    adc_gpio_init();                             /* GPIO初始化 */

    g_adc_handle.Instance = ADC_ADCX;
    g_adc_handle.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;            /* 输入时钟2分频,即adc_ker_ck=per_ck/2=32Mhz */
    g_adc_handle.Init.Resolution = ADC_RESOLUTION_16B;                      /* 16位模式  */
    g_adc_handle.Init.ScanConvMode = ADC_SCAN_DISABLE;                      /* 非扫描模式 */
    g_adc_handle.Init.EOCSelection = ADC_EOC_SINGLE_CONV;                   /* 关闭EOC中断 */
    g_adc_handle.Init.LowPowerAutoWait = DISABLE;                           /* 自动低功耗关闭 */
    g_adc_handle.Init.ContinuousConvMode = DISABLE;                         /* 关闭连续转换 */
    g_adc_handle.Init.NbrOfConversion = 1;                                  /* 使用了1个转换通道 */
    g_adc_handle.Init.DiscontinuousConvMode = DISABLE;                      /* 禁止不连续采样模式 */
    g_adc_handle.Init.NbrOfDiscConversion = 0;                              /* 禁止不连续模式后，此参数忽略，不连续采样通道数为0 */
    g_adc_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;                /* 软件触发 */
    g_adc_handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE; /* 采用软件触发的话，此位忽略 */
    g_adc_handle.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;                   /* 有新的数据后直接覆盖掉旧数据 */
    g_adc_handle.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DR;     /* 常规通道的数据仅仅保存在DR寄存器里面 */
    g_adc_handle.Init.OversamplingMode = ENABLE;                            /* 开启过采样 */
    g_adc_handle.Init.Oversampling.Ratio = osr+1;                           /* osr+1倍过采样 */
    g_adc_handle.Init.Oversampling.RightBitShift = ovss;                    /* 数据右移ovss  bit */
    g_adc_handle.Init.Oversampling.TriggeredMode = ADC_TRIGGEREDMODE_SINGLE_TRIGGER;           /* 触发后连续完成每个通道的所有过采样转换 */
    g_adc_handle.Init.Oversampling.OversamplingStopReset = ADC_REGOVERSAMPLING_CONTINUED_MODE; /* 如果触发注入转换，过采样会暂时停止，并会在注
                                                                                                  入序列完成后继续，注入序列过程中会保留过采样缓冲区*/
    HAL_ADC_Init(&g_adc_handle);                                            /* 初始化 */

    HAL_ADCEx_Calibration_Start(&g_adc_handle, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED); /* ADC校准 */
}



















