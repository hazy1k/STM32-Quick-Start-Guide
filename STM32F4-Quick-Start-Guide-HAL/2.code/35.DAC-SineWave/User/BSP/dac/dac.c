#include "dac.h"
#include "delay.h"

DAC_HandleTypeDef dac_handle;
DMA_HandleTypeDef dac_dma_handle;
extern uint16_t dac_sin_buf[4096];

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
    float incval; // 递增量 
    float Curval; // 当前值
    if((maxval + 1) <= samples) return ;   // 数据不合法
    incval = (maxval + 1) / (samples / 2); // 计算递增量
    for(j = 0; j < n; j++)
    { 
        Curval = 0;
        HAL_DAC_SetValue(&dac_handle, DAC_CHANNEL_1, DAC_ALIGN_12B_R, Curval); // 先输出0
        for(i = 0; i < (samples / 2); i++)  // 输出上升沿
        {
            Curval  +=  incval;             // 新的输出值
            HAL_DAC_SetValue(&dac_handle, DAC_CHANNEL_1, DAC_ALIGN_12B_R, Curval);
            delay_us(dt);
        } 
        for(i = 0; i < (samples / 2); i++)  // 输出下降沿
        {
            Curval  -=  incval;             // 新的输出值
            HAL_DAC_SetValue(&dac_handle, DAC_CHANNEL_1, DAC_ALIGN_12B_R, Curval);
            delay_us(dt);
        }
    }
}

void dac_dma_init(uint8_t outx)
{
    DAC_ChannelConfTypeDef dac_ch_handle;
    GPIO_InitTypeDef GPIO_InitStruct;
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_DAC_CLK_ENABLE();
    __HAL_RCC_DMA1_CLK_ENABLE();

    GPIO_InitStruct.Pin = (outx==1)? GPIO_PIN_4 : GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    dac_handle.Instance = DAC;
    HAL_DAC_Init(&dac_handle);
    dac_dma_handle.Instance = (outx==1)?DMA1_Stream5:DMA1_Stream6; // DMA1_Stream5/6
    dac_dma_handle.Init.Channel = DMA_CHANNEL_7;
    dac_dma_handle.Init.Direction = DMA_MEMORY_TO_PERIPH; // 存储器到外设
    dac_dma_handle.Init.PeriphInc = DMA_PINC_DISABLE; // 外设地址不增长
    dac_dma_handle.Init.MemInc = DMA_MINC_ENABLE; // 存储器地址增长
    dac_dma_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    dac_dma_handle.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    dac_dma_handle.Init.Mode = DMA_CIRCULAR; // 循环模式
    dac_dma_handle.Init.Priority = DMA_PRIORITY_MEDIUM;
    dac_dma_handle.Init.FIFOMode = DMA_FIFOMODE_DISABLE; // 禁用FIFO
    HAL_DMA_Init(&dac_dma_handle);
    __HAL_LINKDMA(&dac_handle, DMA_Handle1, dac_dma_handle); // 链接DMA
    dac_ch_handle.DAC_Trigger = DAC_TRIGGER_T7_TRGO; // 定时器7触发
    dac_ch_handle.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE; // 使能输出缓冲
    HAL_DAC_ConfigChannel(&dac_handle, &dac_ch_handle, DAC_CHANNEL_1); // 配置通道1
}

/**
 * @brief       DAC DMA使能波形输出
 *   @note      TIM7的输入时钟频率(f)来自APB1, f = 42 * 2 = 84Mhz.
 *              DAC触发频率 ftrgo = f / ((psc + 1) * (arr + 1))
 *              波形频率 = ftrgo / ndtr;
 * @param       outx        : DAC通道1/2
 * @param       ndtr        : DMA通道单次传输数据量
 * @param       arr         : TIM7的自动重装载值
 * @param       psc         : TIM7的分频系数
 * @retval      无
 */
void dac_dma_wave_enable(uint8_t outx, uint16_t ndtr, uint16_t arr, uint16_t psc)
{
    TIM_HandleTypeDef tim7_handle = {0};
    TIM_MasterConfigTypeDef master_config = {0};

    __HAL_RCC_TIM7_CLK_ENABLE();                                       

    tim7_handle.Instance = TIM7;                                        
    tim7_handle.Init.Prescaler = psc;                                  
    tim7_handle.Init.CounterMode = TIM_COUNTERMODE_UP;                 
    tim7_handle.Init.Period = arr;                                     
    tim7_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    HAL_TIM_Base_Init(&tim7_handle); // TIM7初始化                                    

    master_config.MasterOutputTrigger = TIM_TRGO_UPDATE; // 定时器7更新触发
    master_config.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE; // 禁止同步模式
    HAL_TIMEx_MasterConfigSynchronization(&tim7_handle, &master_config); // 配置TIM7 TRGO
    HAL_TIM_Base_Start(&tim7_handle); 
    HAL_DAC_Stop_DMA(&dac_handle, (outx == 1)  ? DAC_CHANNEL_1 : DAC_CHANNEL_2); // 先停止之前的传输
    HAL_DAC_Start_DMA(&dac_handle, (outx == 1) ? DAC_CHANNEL_1 : DAC_CHANNEL_2, (uint32_t*)dac_sin_buf, ndtr, DAC_ALIGN_12B_R); // 启动DMA传输
}