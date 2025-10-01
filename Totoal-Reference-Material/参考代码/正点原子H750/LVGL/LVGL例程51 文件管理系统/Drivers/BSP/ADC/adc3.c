/**
 ****************************************************************************************************
 * @file        adc3.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-03-28
 * @brief       ADC3(开启内部温度传感器) 驱动代码
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
 * V1.0 20200328
 * 第一次发布
 ****************************************************************************************************
 */

#include "./BSP/ADC/adc3.h"
#include "./SYSTEM/delay/delay.h"


ADC_HandleTypeDef adc3_handle;  /* ADC句柄 */

/**
 * @brief       ADC3初始化函数
 *   @note      本函数专用于支持ADC3, 和ADC1/2区分开来, 方便使用
 *              我们使用16位精度, ADC采样时钟=32M, 转换时间为:采样周期 + 8.5个ADC周期
 *              设置最大采样周期: 810.5, 则转换时间 = 819个ADC周期 = 25.6us
 * @param       无
 * @retval      无
 */
void adc3_init(void)
{
    adc3_handle.Instance = ADC3;
    adc3_handle.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;             /* 2分频，ADCCLK=PER_CK/2=64/2 = 32MHZ */
    adc3_handle.Init.Resolution = ADC_RESOLUTION_16B;                       /* 16位模式 */
    adc3_handle.Init.ScanConvMode = DISABLE;                                /* 非扫描模式 */
    adc3_handle.Init.EOCSelection = ADC_EOC_SINGLE_CONV;                    /* 关闭EOC中断 */
    adc3_handle.Init.LowPowerAutoWait = DISABLE;                            /* 自动低功耗关闭 */
    adc3_handle.Init.ContinuousConvMode = DISABLE;                          /* 关闭连续转换 */
    adc3_handle.Init.NbrOfConversion = 1;                                   /* 1个转换在规则序列中 也就是只转换规则序列1 */
    adc3_handle.Init.DiscontinuousConvMode = DISABLE;                       /* 禁止不连续采样模式 */
    adc3_handle.Init.NbrOfDiscConversion = 0;                               /* 不连续采样通道数为0 */
    adc3_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;                 /* 软件触发 */
    adc3_handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;  /* 使用软件触发 */
    adc3_handle.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;                    /* 有新的数据的死后直接覆盖掉旧数据 */
    adc3_handle.Init.OversamplingMode = DISABLE;                            /* 过采样关闭 */
    adc3_handle.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DR;      /* 规则通道的数据仅仅保存在DR寄存器里面 */
    HAL_ADC_Init(&adc3_handle);                                             /* 初始化 */

    HAL_ADCEx_Calibration_Start(&adc3_handle, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED); /* ADC校准 */
}

/**
 * @brief       ADC底层驱动,时钟使能
 * @param       hadc:ADC句柄
 * @note        此函数会被HAL_ADC_Init()调用
 * @retval      无
 */
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
    RCC_PeriphCLKInitTypeDef rcc_periph_clk_handle;

    __HAL_RCC_ADC3_CLK_ENABLE();            /* 使能ADC3时钟 */

    rcc_periph_clk_handle.PeriphClockSelection = RCC_PERIPHCLK_ADC;
    rcc_periph_clk_handle.AdcClockSelection = RCC_ADCCLKSOURCE_CLKP;
    HAL_RCCEx_PeriphCLKConfig(&rcc_periph_clk_handle);
}

/**
 * @brief       获得ADC转换后的结果
 * @param       ch: 通道号, ADC_CHANNEL_0~ADC_CHANNEL_19
 * @retval      无
 */
uint32_t adc3_get_result(ADC_HandleTypeDef adc_handle, uint32_t ch)
{
    ADC_ChannelConfTypeDef adc_ch_conf;

    adc_ch_conf.Channel = ch;                              /* 通道 */
    adc_ch_conf.Rank = ADC_REGULAR_RANK_1;                 /* 1个序列 */
    adc_ch_conf.SamplingTime = ADC_SAMPLETIME_810CYCLES_5; /* 采样时间 */
    adc_ch_conf.SingleDiff = ADC_SINGLE_ENDED;             /* 单边采集 */
    adc_ch_conf.OffsetNumber = ADC_OFFSET_NONE;
    adc_ch_conf.Offset=0;   
    HAL_ADC_ConfigChannel(&adc_handle, &adc_ch_conf);      /* 通道配置 */

    HAL_ADC_Start(&adc_handle);                            /* 开启ADC */

    HAL_ADC_PollForConversion(&adc_handle, 10);            /* 轮询转换 */
    return HAL_ADC_GetValue(&adc_handle);  /* 返回最近一次ADC规则组的转换结果 */
}

/**
 * @brief       获取通道ch的转换值，取times次,然后平均
 * @param       ch      : 通道号, 0~19
 * @param       times   : 获取次数
 * @retval      通道ch的times次转换结果平均值
 */
uint32_t adc3_get_result_average(ADC_HandleTypeDef adc_handle, uint32_t ch, uint8_t times)
{
    uint32_t temp_val = 0;
    uint8_t t;

    for (t = 0; t < times; t++)   /* 获取times次数据 */
    {
        temp_val += adc3_get_result(adc_handle, ch);
        delay_ms(5);
    }

    return temp_val / times;      /* 返回平均值 */
}

/**
 * @brief       获取内部温度传感器温度值
 * @param       无
 * @retval      温度值(扩大了100倍,单位:℃.)
 */
short adc3_get_temperature(void)
{
    uint32_t adcx;
    short result;
    double temperature;
    float temp = 0;
    uint16_t ts_cal1, ts_cal2;
    
    ts_cal1 = *(volatile uint16_t *)(0X1FF1E820);          /* 获取TS_CAL1 */
    ts_cal2 = *(volatile uint16_t *)(0X1FF1E840);          /* 获取TS_CAL2 */
    temp = (float)((110.0 - 30.0) / (ts_cal2 - ts_cal1));  /* 获取比例因子 */

    adcx = adc3_get_result_average(adc3_handle, ADC3_TEMPSENSOR_CHX, 10); /* 读取内部温度传感器通道,10次取平均 */
    temperature = (float)(temp * (adcx - ts_cal1) + 30);   /* 计算温度 */

    result = temperature *= 100;                           /* 扩大100倍. */
    return result;
}
















