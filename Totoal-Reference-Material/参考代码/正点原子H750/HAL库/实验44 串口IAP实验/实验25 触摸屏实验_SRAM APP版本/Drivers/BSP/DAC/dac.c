/**
 ****************************************************************************************************
 * @file        dac.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.2
 * @date        2020-03-28
 * @brief       DAC 驱动代码
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
 * V1.1 20200328
 * 新增dac_triangular_wave函数
 * V1.2 20200328
 * 新增dac_dma_wave_init函数
 *
 ****************************************************************************************************
 */

#include "./BSP/DAC/dac.h"
#include "./BSP/DMA/dma.h"
#include "./SYSTEM/delay/delay.h"


DAC_HandleTypeDef g_dac_handle;           /* DAC句柄 */

/**
 * @brief       DAC初始化函数
 *   @note      本函数支持DAC1_OUT1/2通道初始化
 *              DAC的输入时钟来自APB1, 时钟频率=120Mhz=8.3ns
 *              DAC在输出buffer关闭的时候, 输出建立时间: tSETTLING = 2us (H750数据手册有写)
 *              因此DAC输出的最高速度约为:500Khz, 以10个点为一个周期, 最大能输出50Khz左右的波形
 *
 * @param       outx: 要初始化的通道. 1,通道1; 2,通道2
 * @retval      无
 */
void dac_init(uint32_t outx)
{
    DAC_ChannelConfTypeDef dac_ch_conf;     /* DAC1通道配置结构体 */
    
    g_dac_handle.Instance = DAC1;
    HAL_DAC_Init(&g_dac_handle);                              /* 初始化DAC */
    
    dac_ch_conf.DAC_Trigger = DAC_TRIGGER_NONE;               /* 不使用触发功能 */
    dac_ch_conf.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;  /* DAC1输出缓冲关闭 */
    HAL_DAC_ConfigChannel(&g_dac_handle, &dac_ch_conf, outx); /* DAC通道配置 */
    
    HAL_DAC_Start(&g_dac_handle, outx);  /* 开启DAC通道 */
}

/**
 * @brief       DAC底层驱动，时钟配置，引脚 配置
 * @param       g_dac_dma_handle:DAC句柄
 * @note        此函数会被HAL_DAC_Init()调用
 * @retval      无
 */
void HAL_DAC_MspInit(DAC_HandleTypeDef* g_dac_dma_handle)
{      
    GPIO_InitTypeDef gpio_init_struct;

    __HAL_RCC_DAC12_CLK_ENABLE();             /* 使能DAC时钟 */
    __HAL_RCC_GPIOA_CLK_ENABLE();             /* 开启GPIOA时钟 */

    gpio_init_struct.Pin = GPIO_PIN_4;        /* PA4 */
    gpio_init_struct.Mode = GPIO_MODE_ANALOG; /* 模拟 */
    gpio_init_struct.Pull = GPIO_NOPULL;      /* 不带上下拉 */
    HAL_GPIO_Init(GPIOA, &gpio_init_struct);  /* 初始化DAC引脚 */
}

/**
 * @brief       设置通道1/2输出电压
 * @param       outx: 1,通道1; 2,通道2
 * @param       vol : 0~3300,代表0~3.3V
 * @retval      无
 */
void dac_set_voltage(uint16_t vol)
{
    double temp = vol;
    temp /= 1000;
    temp = temp * 4096 / 3.3;

    HAL_DAC_SetValue(&g_dac_handle, DAC_CHANNEL_1, DAC_ALIGN_12B_R, temp); /* 12位右对齐数据格式设置DAC值 */
}

/***************************************DAC输出三角波程序*****************************************/

/**
 * @brief       设置DAC_OUT1输出三角波
 *   @note      输出频率 ≈ 1000 / (dt * samples) Khz, 不过在dt较小的时候,比如小于5us时, 由于delay_us
 *              本身就不准了(调用函数,计算等都需要时间,延时很小的时候,这些时间会影响到延时), 频率会偏小.
 * 
 * @param       maxval : 最大值(0 < maxval < 4096), (maxval + 1)必须大于等于samples/2
 * @param       dt     : 每个采样点的延时时间(单位: us)
 * @param       samples: 采样点的个数, samples必须小于等于(maxval + 1) * 2 , 且maxval不能等于0
 * @param       n      : 输出波形个数,0~65535
 *
 * @retval      无
 */
void dac_triangular_wave(uint16_t maxval, uint16_t dt, uint16_t samples, uint16_t n)
{
    uint16_t i, j;
    float incval;           /* 递增量 */
    
    if((maxval + 1) <= samples)return ; /* 数据不合法 */
        
    incval = (maxval + 1) / (samples / 2);  /* 计算递增量 */
    
    for(j = 0; j < n; j++)
    { 
        DAC1->DHR12R1 = 0;  /* 先输出0 */
        for(i = 0; i < (samples / 2); i++)  /* 输出上升沿 */
        { 
             DAC1->DHR12R1 +=  incval;      /*由于用库函数太慢了，得不到连续的波形，所以都用寄存器操作 */
             delay_us(dt);
        } 
        for(i = 0; i < (samples / 2); i++)  /* 输出下降沿 */
        {
             DAC1->DHR12R1 -=  incval;      /*由于用库函数太慢了，得不到连续的波形，所以都用寄存器操作 */
             delay_us(dt);
        }
    }
}

/***************************************DAC输出正弦波程序*****************************************/

DAC_HandleTypeDef g_dac_dma_handle;
DMA_HandleTypeDef g_dma_dac_handle;

/**
 * @brief       DAC DMA输出正弦波初始化函数
 *   @note      本函数支持DAC1_OUT1/2通道初始化
 *              DAC的输入时钟来自APB1, 时钟频率=120Mhz=8.3ns
 *              DAC在输出buffer关闭的时候, 输出建立时间: tSETTLING = 2us (H750数据手册有写)
 *              因此DAC输出的最高速度约为:500Khz, 以10个点为一个周期, 最大能输出50Khz左右的波形
 *
 * @param       outx: 要初始化的通道. 1,通道1; 2,通道2
 * @param       DMA_Streamx : DMA数据流,DMA1_Stream0~7/DMA2_Stream0~7
 * @param       ch          : DMA通道选择,范围:1~115(详见<<STM32H7xx参考手册>>16.3.2节,Table 116)
 * @param       arr         : TIM7的自动重装载值
 * @param       psc         : TIM7的分频系数
 * @retval      无
 */
void dac_dma_wave_init(void)
{
    DAC_ChannelConfTypeDef sConfig={0};
    TIM_MasterConfigTypeDef sMasterConfig={0};
    GPIO_InitTypeDef gpio_init_struct;
    TIM_HandleTypeDef tim7_handle;

    __HAL_RCC_GPIOA_CLK_ENABLE();             /* DAC通道引脚端口时钟使能 */
    __HAL_RCC_DAC12_CLK_ENABLE();             /* DAC外设时钟使能 */
    __HAL_RCC_DMA2_CLK_ENABLE();              /* DMA时钟使能 */
    __HAL_RCC_TIM7_CLK_ENABLE();              /* TIM7时钟使能 */

    gpio_init_struct.Pin = GPIO_PIN_4;        /* PA4 */
    gpio_init_struct.Mode = GPIO_MODE_ANALOG; /* 模拟 */
    gpio_init_struct.Pull = GPIO_NOPULL;      /* 不带上下拉 */
    HAL_GPIO_Init(GPIOA, &gpio_init_struct);  /* 初始化DAC引脚 */

    g_dac_dma_handle.Instance = DAC1;
    HAL_DAC_Init(&g_dac_dma_handle);                                     /* DAC初始化 */

    g_dma_dac_handle.Instance = DMA2_Stream6;                            /* 使用的DAM2 Stream6 */
    g_dma_dac_handle.Init.Request = DMA_REQUEST_DAC1_CH1;                /* DAC触发DMA传输 */
    g_dma_dac_handle.Init.Direction = DMA_MEMORY_TO_PERIPH;              /* 存储器到外设模式 */
    g_dma_dac_handle.Init.PeriphInc = DMA_PINC_DISABLE;                  /* 外设地址禁止自增 */
    g_dma_dac_handle.Init.MemInc = DMA_MINC_ENABLE;                      /* 存储器地址自增 */
    g_dma_dac_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD; /* 外设数据长度:16位 */
    g_dma_dac_handle.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;    /* 存储器数据长度:16位 */
    g_dma_dac_handle.Init.Mode = DMA_CIRCULAR;                           /* 循环模式 */
    g_dma_dac_handle.Init.Priority = DMA_PRIORITY_MEDIUM;                /* 中等优先级 */
    g_dma_dac_handle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;               /* 不使用FIFO */
    HAL_DMA_Init(&g_dma_dac_handle);                                     /* 初始化DMA */

    __HAL_LINKDMA(&g_dac_dma_handle, DMA_Handle1, g_dma_dac_handle);     /* DMA句柄与DAC句柄关联 */

    HAL_NVIC_SetPriority(DMA2_Stream6_IRQn, 1, 0);                       /* 抢占优先级1，子优先级0 */
    HAL_NVIC_EnableIRQ(DMA2_Stream6_IRQn);                               /* 使能DMA中断通道 */

    sConfig.DAC_SampleAndHold = DAC_SAMPLEANDHOLD_DISABLE;               /* 关闭采样保持模式，这个模式主要用于低功耗 */
    sConfig.DAC_Trigger = DAC_TRIGGER_T7_TRGO;                           /* 采用定时器7触发 */
    sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;                  /* 使能输出缓冲 */
    sConfig.DAC_ConnectOnChipPeripheral = DAC_CHIPCONNECT_DISABLE;       /* 不将DAC连接到片上外设 */
    sConfig.DAC_UserTrimming = DAC_TRIMMING_FACTORY;                     /* 使用出厂校准 */
    HAL_DAC_ConfigChannel(&g_dac_dma_handle, &sConfig, DAC_CHANNEL_1);   /* DAC通道输出配置 */

    tim7_handle.Instance = TIM7;                                         /* 选择定时器7 */
    tim7_handle.Init.Prescaler = 23;                                     /* 分频系数 */
    tim7_handle.Init.CounterMode = TIM_COUNTERMODE_UP;                   /* 向上计数 */
    tim7_handle.Init.Period = 19;                                        /* 重装载值 */
    tim7_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;  /* 自动重装 */
    HAL_TIM_Base_Init(&tim7_handle);                                     /* 初始化定时器7 */

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    HAL_TIMEx_MasterConfigSynchronization(&tim7_handle, &sMasterConfig); /* 配置TIM7 TRGO */
    HAL_TIM_Base_Start(&tim7_handle);                                    /* 使能定时器7 */
}

/**
 * @brief       DAC DMA中断服务函数
 * @param       无 
 * @retval      无
 */
void DMA2_Stream6_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&g_dma_dac_handle);
}







