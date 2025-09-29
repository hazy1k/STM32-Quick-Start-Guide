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


uint8_t g_ov_frame = 0;                 /* 帧率 */
extern void jpeg_data_process(void);    /* JPEG数据处理函数 */

/**
 * @brief       DCMI中断服务函数
 * @param       无
 * @retval      无
 */
void DCMI_IRQHandler(void)
{
    if (DCMI->MISR & 0X01)      /* 捕获到一帧图像 */
    {
        jpeg_data_process();    /* jpeg数据处理 */
        DCMI->ICR |= 1 << 0;    /* 清除帧中断 */
        LED1_TOGGLE();          /* LED1闪烁 */
        g_ov_frame++;
    }
}

/**
 * @brief       DCMI DMA配置
 * @param       mem0addr: 存储器地址0     将要存储摄像头数据的内存地址(也可以是外设地址)
 * @param       mem1addr: 存储器地址1     当只使用mem0addr的时候,该值必须为0
 * @param       memsize : 存储器长度      0~65535
 * @param       memblen : 存储器位宽      0,8位,1,16位,2,32位
 * @param       meminc  : 存储器增长方式  0,不增长; 1,增长
 * @retval      无
 */
void dcmi_dma_init(uint32_t mem0addr, uint32_t mem1addr, uint16_t memsize, uint8_t memblen, uint8_t meminc)
{
    uint32_t tempreg = 0;
    RCC->AHB1ENR |= 1 << 0;             /* DMA1时钟使能 */
    RCC->D3AMR |= 1 << 0;               /* DMAMUX时钟使能 */

    while (DMA1_Stream1->CR & 0X01);    /* 等待DMA1可配置 */

    /* 通道选择 详见<<STM32H7xx参考手册>>16.3.2节,Table 116 
     * DMAMUX1_Channel1~7,  对应 DMA1_Stream1~7
     * DMAMUX1_Channel8~15, 对应 DMA2_Stream1~7
     * 详见DMA例程的说明
     */
    DMAMUX1_Channel1->CCR = 75;         /* DMA1_stream1的通道选择: 75,即DCMI对应的通道 */

    DMA1->LIFCR |= 0X3D << 6 * 1;       /* 清空通道1上所有中断标志 */
    DMA1_Stream1->FCR = 0X0000021;      /* 设置为默认值 */

    DMA1_Stream1->PAR = (uint32_t)&DCMI->DR; /* 外设地址为:DCMI->DR */
    DMA1_Stream1->M0AR = mem0addr;      /* mem0addr作为目标地址0 */
    DMA1_Stream1->M1AR = mem1addr;      /* mem1addr作为目标地址1 */
    DMA1_Stream1->NDTR = memsize;       /* 传输长度为memsize */
    tempreg |= 0 << 6;                  /* 外设到存储器模式 */
    tempreg |= 1 << 8;                  /* 循环模式 */
    tempreg |= 0 << 9;                  /* 外设非增量模式 */
    tempreg |= meminc << 10;            /* 存储器增量模式 */
    tempreg |= 2 << 11;                 /* 外设数据长度:32位 */
    tempreg |= memblen << 13;           /* 存储器位宽,8/16/32bit */
    tempreg |= 2 << 16;                 /* 高优先级 */
    tempreg |= 0 << 21;                 /* 外设突发单次传输 */
    tempreg |= 0 << 23;                 /* 存储器突发单次传输 */

    if (mem1addr)   /* 双缓冲的时候,才需要开启 */
    {
        tempreg |= 1 << 18;             /* 双缓冲模式 */
        tempreg |= 1 << 4;              /* 开启传输完成中断 */
        sys_nvic_init(2, 3, DMA1_Stream1_IRQn, 2);   /* 抢占1，子优先级3，组2 */
    }

    DMA1_Stream1->CR = tempreg;         /* 设置CR寄存器 */
}

/* DCMI DMA接收回调函数, 仅双缓冲模式用到, 配合中断服务函数使用 */
void (*dcmi_rx_callback)(void);

/**
 * @brief       DMA1_Stream1中断服务函数(仅双缓冲模式会用到)
 * @param       无
 * @retval      无
 */
void DMA1_Stream1_IRQHandler(void)
{
    if (DMA1->LISR & (1 << 11)) /* DMA1_Steam1,传输完成标志 */
    {
        DMA1->LIFCR |= 1 << 11; /* 清除传输完成中断 */
        dcmi_rx_callback();     /* 执行摄像头接收回调函数,读取数据等操作在这里面处理 */
        SCB_CleanInvalidateDCache();    /* 清除无效的D-Cache */
    }
}

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
    uint32_t tempreg = 0;
    /* 设置IO */
    RCC->AHB4ENR |= 1 << 0;     /* 使能外设PORTA时钟 */
    RCC->AHB4ENR |= 1 << 1;     /* 使能外设PORTB时钟 */
    RCC->AHB4ENR |= 1 << 2;     /* 使能外设PORTC时钟 */
    RCC->AHB4ENR |= 1 << 3;     /* 使能外设PORTD时钟 */
    RCC->AHB2ENR |= 1 << 0;     /* 能DCMI时钟 */

    /* PA4/6复用功能输出 */
    sys_gpio_set(GPIOA, SYS_GPIO_PIN4 | SYS_GPIO_PIN6,
                SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);   
    
    /* PB7/8/9复用功能输出 */
    sys_gpio_set(GPIOB, SYS_GPIO_PIN7 | SYS_GPIO_PIN8 | SYS_GPIO_PIN9,
                SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);    

    /* PC6/7/8/9/11复用功能输出 */
    sys_gpio_set(GPIOC, SYS_GPIO_PIN6 | SYS_GPIO_PIN7 | SYS_GPIO_PIN8 | SYS_GPIO_PIN9 | SYS_GPIO_PIN11,
                SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);    

    /* PD3复用功能输出 */
    sys_gpio_set(GPIOD, SYS_GPIO_PIN3,
                SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);    

    sys_gpio_af_set(GPIOA, SYS_GPIO_PIN4, 13);  /* PA4,AF13  DCMI_HSYNC */
    sys_gpio_af_set(GPIOA, SYS_GPIO_PIN6, 13);  /* PA6,AF13  DCMI_PCLK */
    sys_gpio_af_set(GPIOB, SYS_GPIO_PIN7, 13);  /* PB7,AF13  DCMI_VSYNC */
    sys_gpio_af_set(GPIOC, SYS_GPIO_PIN6, 13);  /* PC6,AF13  DCMI_D0 */
    sys_gpio_af_set(GPIOC, SYS_GPIO_PIN7, 13);  /* PC7,AF13  DCMI_D1 */
    sys_gpio_af_set(GPIOC, SYS_GPIO_PIN8, 13);  /* PC8,AF13  DCMI_D2 */
    sys_gpio_af_set(GPIOC, SYS_GPIO_PIN9, 13);  /* PC9,AF13  DCMI_D3 */
    sys_gpio_af_set(GPIOC, SYS_GPIO_PIN11, 13); /* PC11,AF13 DCMI_D4 */
    sys_gpio_af_set(GPIOD, SYS_GPIO_PIN3, 13);  /* PD3,AF13  DCMI_D5 */
    sys_gpio_af_set(GPIOB, SYS_GPIO_PIN8, 13);  /* PB8,AF13  DCMI_D6 */
    sys_gpio_af_set(GPIOB, SYS_GPIO_PIN9, 13);  /* PB9,AF13  DCMI_D7 */
    
    /* 清除原来的设置 */
    DCMI->IER = 0x0;
    DCMI->ICR = 0x1F;
    DCMI->ESCR = 0x0;
    DCMI->ESUR = 0x0;
    DCMI->CWSTRTR = 0x0;
    DCMI->CWSIZER = 0x0;
    tempreg |= 0 << 1;      /* 连续模式 */
    tempreg |= 0 << 2;      /* 全帧捕获 */
    tempreg |= 0 << 4;      /* 硬件同步HSYNC,VSYNC */
    tempreg |= 1 << 5;      /* PCLK 上升沿有效 */
    tempreg |= 0 << 6;      /* HSYNC 低电平有效 */
    tempreg |= 0 << 7;      /* VSYNC 低电平有效 */
    tempreg |= 0 << 8;      /* 捕获所有的帧 */
    tempreg |= 0 << 10;     /* 8位数据格式 */
    DCMI->IER |= 1 << 0;    /* 开启帧中断 */
    tempreg |= 1 << 14;     /* DCMI使能 */
    DCMI->CR = tempreg;     /* 设置CR寄存器 */
    
    sys_nvic_init(2, 2, DCMI_IRQn, 2);  /* 抢占1，子优先级2，组2 */
}

/**
 * @brief       DCMI,启动传输
 * @param       无
 * @retval      无
 */
void dcmi_start(void)
{
    lcd_set_cursor(0, 0);       /* 设置坐标到原点 */
    lcd_write_ram_prepare();    /* 开始写入GRAM */
    DMA1_Stream1->CR |= 1 << 0; /* 开启DMA1,Stream1 */
    DCMI->CR |= 1 << 0;         /* DCMI捕获使能 */
}

/**
 * @brief       DCMI,关闭传输
 * @param       无
 * @retval      无
 */
void dcmi_stop(void)
{
    DCMI->CR &= ~(1 << 0);          /* DCMI捕获关闭 */

    while (DCMI->CR & 0X01);        /* 等待传输结束 */

    DMA1_Stream1->CR &= ~(1 << 0);  /* 关闭DMA1,Stream1 */
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
    lcd_write_ram_prepare();        /* 开始写入GRAM */
    DMA1_Stream1->CR |= 1 << 0;     /* 开启DMA1,Stream1 */
    DCMI->CR |= 1 << 0;             /* DCMI捕获使能 */
}

/**
 * @brief       通过usmart调试,辅助测试用.
 * @param       pclk/hsync/vsync : 三个信号的有效电平设置
 * @retval      无
 */
void dcmi_cr_set(uint8_t pclk, uint8_t hsync, uint8_t vsync)
{
    DCMI->CR = 0;
    DCMI->CR |= pclk << 5;		/* PCLK 有效边沿设置 */
    DCMI->CR |= hsync << 6;		/* HSYNC 有效电平设置 */
    DCMI->CR |= vsync << 7;		/* VSYNC 有效电平设置 */
    DCMI->CR |= 1 << 14; 		/* DCMI使能 */
    DCMI->CR |= 1 << 0; 		/* DCMI捕获使能 */
}

/******************************************************************************************/







