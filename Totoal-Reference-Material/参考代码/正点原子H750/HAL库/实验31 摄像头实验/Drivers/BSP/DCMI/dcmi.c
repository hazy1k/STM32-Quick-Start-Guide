/**
 ****************************************************************************************************
 * @file        dcmi.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-03
 * @brief       DCMI 驱动代码
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
 * V1.0 20200403
 * 第一次发布
 *
 ****************************************************************************************************
 */

#include "./BSP/LED/led.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/DCMI/dcmi.h"
#include "./BSP/OV5640/ov5640.h"


DCMI_HandleTypeDef g_dcmi_handle;           /* DCMI句柄 */
DMA_HandleTypeDef g_dma_dcmi_handle;        /* DMA句柄 */

uint8_t g_ov_frame = 0;                     /* 帧率 */
extern void jpeg_data_process(void);        /* JPEG数据处理函数 */

/**
 * @brief       DCMI 初始化
 *   @note      IO对应关系如下:
 *              摄像头模块 ------------ STM32开发板
 *               OV_D0~D7  ------------  PC6/PC7/PC8/PC9/PC11/PD3/PB8/PB9
 *               OV_SCL    ------------  PB10
 *               OV_SDA    ------------  PB11
 *               OV_VSYNC  ------------  PB7
 *               OV_HREF   ------------  PA4
 *               OV_RESET  ------------  PA7
 *               OV_PCLK   ------------  PA6
 *               OV_PWDN   ------------  PC4
 *              本函数仅初始化OV_D0~D7/OV_VSYNC/OV_HREF/OV_PCLK等信号(11个).
 *
 * @param       无
 * @retval      无
 */
void dcmi_init(void)
{
    g_dcmi_handle.Instance = DCMI;
    g_dcmi_handle.Init.SynchroMode = DCMI_SYNCHRO_HARDWARE;    /* 硬件同步HSYNC,VSYNC */
    g_dcmi_handle.Init.PCKPolarity = DCMI_PCKPOLARITY_RISING;  /* PCLK 上升沿有效 */
    g_dcmi_handle.Init.VSPolarity = DCMI_VSPOLARITY_LOW;       /* VSYNC 低电平有效 */
    g_dcmi_handle.Init.HSPolarity = DCMI_HSPOLARITY_LOW;       /* HSYNC 低电平有效 */
    g_dcmi_handle.Init.CaptureRate = DCMI_CR_ALL_FRAME;        /* 全帧捕获 */
    g_dcmi_handle.Init.ExtendedDataMode = DCMI_EXTEND_DATA_8B; /* 8位数据格式 */
    HAL_DCMI_Init(&g_dcmi_handle);                             /* 初始化DCMI，此函数会开启帧中断 */

    /* 关闭行中断、VSYNC中断、同步错误中断和溢出中断 */
    //__HAL_DCMI_DISABLE_IT(&g_dcmi_handle,DCMI_IT_LINE|DCMI_IT_VSYNC|DCMI_IT_ERR|DCMI_IT_OVR);

    /* 关闭所有中断，函数HAL_DCMI_Init()会默认打开很多中断，开启这些中断
      以后我们就需要对这些中断做相应的处理，否则的话就会导致各种各样的问题，
      但是这些中断很多都不需要，所以这里将其全部关闭掉，也就是将IER寄存器清零。
      关闭完所有中断以后再根据自己的实际需求来使能相应的中断 */
    DCMI->IER=0x0;

    __HAL_DCMI_ENABLE_IT(&g_dcmi_handle,DCMI_IT_FRAME);      /* 使能帧中断 */
    __HAL_DCMI_ENABLE(&g_dcmi_handle);                       /* 使能DCMI */
}

/**
 * @brief       DCMI底层驱动，引脚配置，时钟使能，中断配置
 * @param       hdcmi:DCMI句柄
 * @note        此函数会被HAL_DCMI_Init()调用
 * @retval      无
 */
void HAL_DCMI_MspInit(DCMI_HandleTypeDef* hdcmi)
{
    GPIO_InitTypeDef gpio_init_struct;
    
    __HAL_RCC_DCMI_CLK_ENABLE();                /* 使能DCMI时钟 */
    __HAL_RCC_GPIOA_CLK_ENABLE();               /* 使能GPIOA时钟 */
    __HAL_RCC_GPIOB_CLK_ENABLE();               /* 使能GPIOB时钟 */
    __HAL_RCC_GPIOC_CLK_ENABLE();               /* 使能GPIOC时钟 */
    __HAL_RCC_GPIOD_CLK_ENABLE();               /* 使能GPIOD时钟 */
    
    gpio_init_struct.Pin = GPIO_PIN_4 | GPIO_PIN_6;
    gpio_init_struct.Mode = GPIO_MODE_AF_PP;            /* 推挽复用 */
    gpio_init_struct.Pull = GPIO_PULLUP;                /* 上拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH; /* 高速 */
    gpio_init_struct.Alternate = GPIO_AF13_DCMI;        /* 复用为DCMI */
    HAL_GPIO_Init(GPIOA, &gpio_init_struct);            /* 初始化PA4，6引脚 */

    gpio_init_struct.Pin = GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9;
    HAL_GPIO_Init(GPIOB, &gpio_init_struct);            /* 初始化PB7,8,9引脚 */

    gpio_init_struct.Pin = GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_11;
    HAL_GPIO_Init(GPIOC, &gpio_init_struct);            /* 初始化PC6,7,8,9,11引脚 */

    gpio_init_struct.Pin = GPIO_PIN_3; 
    HAL_GPIO_Init(GPIOD, &gpio_init_struct);            /* 初始化PD3引脚 */

    HAL_NVIC_SetPriority(DCMI_IRQn, 2, 2);              /* 抢占优先级2，子优先级2 */
    HAL_NVIC_EnableIRQ(DCMI_IRQn);                      /* 使能DCMI中断 */
}

/**
 * @brief       DCMI DMA配置
 * @param       mem0addr: 存储器地址0     将要存储摄像头数据的内存地址(也可以是外设地址)
 * @param       mem1addr: 存储器地址1     当只使用mem0addr的时候,该值必须为0
 * @param       memsize : 存储器长度      0~65535
 * @param       memblen : 存储器位宽      DMA_MDATAALIGN_BYTE,8位;DMA_MDATAALIGN_HALFWORD,16位;DMA_MDATAALIGN_WORD,32位
 * @param       meminc  : 存储器增长方式  DMA_MINC_DISABLE,不增长; DMA_MINC_ENABLE,增长
 * @retval      无
 */
void dcmi_dma_init(uint32_t mem0addr,uint32_t mem1addr,uint16_t memsize,uint32_t memblen,uint32_t meminc)
{ 
    __HAL_RCC_DMA1_CLK_ENABLE();                                        /* 使能DMA1时钟 */
    __HAL_LINKDMA(&g_dcmi_handle, DMA_Handle, g_dma_dcmi_handle);       /* 将DMA与DCMI联系起来 */
    __HAL_DMA_DISABLE_IT(&g_dma_dcmi_handle, DMA_IT_TC);                /* 先关闭DMA传输完成中断(否则在使用MCU屏的时候会出现花屏的情况) */

    g_dma_dcmi_handle.Instance = DMA1_Stream1;                          /* DMA1数据流1 */
    g_dma_dcmi_handle.Init.Request = DMA_REQUEST_DCMI;                  /* DCMI的DMA请求 */
    g_dma_dcmi_handle.Init.Direction = DMA_PERIPH_TO_MEMORY;            /* 外设到存储器 */
    g_dma_dcmi_handle.Init.PeriphInc = DMA_PINC_DISABLE;                /* 外设非增量模式 */
    g_dma_dcmi_handle.Init.MemInc = meminc;                             /* 存储器增量模式 */
    g_dma_dcmi_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;   /* 外设数据长度:32位 */
    g_dma_dcmi_handle.Init.MemDataAlignment = memblen;                  /* 存储器数据长度:8/16/32位 */
    g_dma_dcmi_handle.Init.Mode = DMA_CIRCULAR;                         /* 使用循环模式 */
    g_dma_dcmi_handle.Init.Priority = DMA_PRIORITY_HIGH;                /* 高优先级 */
    g_dma_dcmi_handle.Init.FIFOMode = DMA_FIFOMODE_ENABLE;              /* 使能FIFO */
    g_dma_dcmi_handle.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL; /* 使用1/2的FIFO */
    g_dma_dcmi_handle.Init.MemBurst = DMA_MBURST_SINGLE;                /* 存储器突发传输 */
    g_dma_dcmi_handle.Init.PeriphBurst = DMA_PBURST_SINGLE;             /* 外设突发单次传输 */
    HAL_DMA_DeInit(&g_dma_dcmi_handle);                                 /* 先清除以前的设置 */
    HAL_DMA_Init(&g_dma_dcmi_handle);                                   /* 初始化DMA */
    
    /* 在开启DMA之前先使用__HAL_UNLOCK()解锁一次DMA,因为HAL_DMA_Statrt()HAL_DMAEx_MultiBufferStart()
      这两个函数一开始要先使用__HAL_LOCK()锁定DMA,而函数__HAL_LOCK()会判断当前的DMA状态是否为锁定状态，如果是
      锁定状态的话就直接返回HAL_BUSY，这样会导致函数HAL_DMA_Statrt()和HAL_DMAEx_MultiBufferStart()后续的DMA配置
      程序直接被跳过！DMA也就不能正常工作，为了避免这种现象，所以在启动DMA之前先调用__HAL_UNLOC()先解锁一次DMA。 */
    
    __HAL_UNLOCK(&g_dma_dcmi_handle);
    if (mem1addr == 0)  /* 开启DMA，不使用双缓冲 */
    {
        HAL_DMA_Start(&g_dma_dcmi_handle, (uint32_t)&DCMI->DR, mem0addr, memsize);
    }
    else                /* 使用双缓冲 */
    {
        HAL_DMAEx_MultiBufferStart(&g_dma_dcmi_handle, (uint32_t)&DCMI->DR, mem0addr, mem1addr, memsize); /* 开启双缓冲 */
        __HAL_DMA_ENABLE_IT(&g_dma_dcmi_handle, DMA_IT_TC);     /* 开启传输完成中断 */
        HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 2, 3);          /* DMA中断优先级 */
        HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);
    }
}

/**
 * @brief       DCMI,启动传输
 * @param       无
 * @retval      无
 */
void dcmi_start(void)
{  
    lcd_set_cursor(0,0);                  /* 设置坐标到原点 */
    lcd_write_ram_prepare();              /* 开始写入GRAM */
    __HAL_DMA_ENABLE(&g_dma_dcmi_handle); /* 使能DMA */
    DCMI->CR |= DCMI_CR_CAPTURE;          /* DCMI捕获使能 */
}

/**
 * @brief       DCMI,关闭传输
 * @param       无
 * @retval      无
 */
void dcmi_stop(void)
{ 
    DCMI->CR &= ~(DCMI_CR_CAPTURE);         /* DCMI捕获关闭 */

    while (DCMI->CR & 0X01);                /* 等待传输结束 */

    __HAL_DMA_DISABLE(&g_dma_dcmi_handle);  /* 关闭DMA */
} 

/**
 * @brief       DCMI中断服务函数
 * @param       无
 * @retval      无
 */
void DCMI_IRQHandler(void)
{
    HAL_DCMI_IRQHandler(&g_dcmi_handle);
}

/**
 * @brief       DCMI中断回调服务函数
 * @param       hdcmi:DCMI句柄
 * @note        捕获到一帧图像处理
 * @retval      无
 */
void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef *hdcmi)
{
    jpeg_data_process();    /* jpeg数据处理 */
    LED1_TOGGLE();          /* LED1闪烁 */
    g_ov_frame++;

    /* 重新使能帧中断,因为HAL_DCMI_IRQHandler()函数会关闭帧中断 */
    __HAL_DCMI_ENABLE_IT(&g_dcmi_handle, DCMI_IT_FRAME);
}

void (*dcmi_rx_callback)(void); /* DCMI DMA接收回调函数 */

/**
 * @brief       DMA1数据流1中断服务函数(仅双缓冲模式会用到)
 * @param       无
 * @retval      无
 */
void DMA1_Stream1_IRQHandler(void)
{
    if (__HAL_DMA_GET_FLAG(&g_dma_dcmi_handle, DMA_FLAG_TCIF1_5) != RESET)  /* DMA传输完成 */
    {
        __HAL_DMA_CLEAR_FLAG(&g_dma_dcmi_handle, DMA_FLAG_TCIF1_5); /* 清除DMA传输完成中断标志位 */
        dcmi_rx_callback();     /* 执行摄像头接收回调函数,读取数据等操作在这里面处理 */
    } 
}


/******************************************************************************************/
/* 以下两个函数,供usmart调用,用于调试代码 */

/**
 * @brief       DCMI设置显示窗口
 * @param       sx,sy       : LCD的起始坐标
 * @param       width,height: LCD显示范围.
 * @retval      无
 */
void dcmi_set_window(uint16_t sx, uint16_t sy, uint16_t width, uint16_t height)
{
    dcmi_stop();
    lcd_clear(WHITE);
    lcd_set_window(sx, sy, width, height);
    ov5640_outsize_set(0, 0, width, height);
    lcd_set_cursor(0, 0);
    lcd_write_ram_prepare();               /* 开始写入GRAM */
    __HAL_DMA_ENABLE(&g_dma_dcmi_handle);  /* 开启DMA1,Stream1 */
    DCMI->CR |= DCMI_CR_CAPTURE;           /* DCMI捕获使能 */
}

/**
 * @brief       通过usmart调试,辅助测试用.
 * @param       pclk/hsync/vsync : 三个信号的有效电平设置
 * @retval      无
 */
void dcmi_cr_set(uint8_t pclk, uint8_t hsync, uint8_t vsync)
{
    HAL_DCMI_DeInit(&g_dcmi_handle);                            /* 清除原来的设置 */
    g_dcmi_handle.Instance = DCMI;
    g_dcmi_handle.Init.SynchroMode = DCMI_SYNCHRO_HARDWARE;     /* 硬件同步HSYNC,VSYNC */
    g_dcmi_handle.Init.PCKPolarity = pclk<<5;                   /* PCLK 上升沿有效 */
    g_dcmi_handle.Init.VSPolarity = vsync<<7;                   /* VSYNC 低电平有效 */
    g_dcmi_handle.Init.HSPolarity = hsync<<6;                   /* HSYNC 低电平有效 */
    g_dcmi_handle.Init.CaptureRate = DCMI_CR_ALL_FRAME;         /* 全帧捕获 */
    g_dcmi_handle.Init.ExtendedDataMode = DCMI_EXTEND_DATA_8B;  /* 8位数据格式 */
    HAL_DCMI_Init(&g_dcmi_handle);                              /* 初始化DCMI */
    g_dcmi_handle.Instance->CR |= DCMI_MODE_CONTINUOUS;         /* 持续模式 */
}

/******************************************************************************************/







