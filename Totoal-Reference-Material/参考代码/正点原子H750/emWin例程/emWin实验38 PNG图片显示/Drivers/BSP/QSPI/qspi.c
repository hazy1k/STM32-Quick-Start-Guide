/**
 ****************************************************************************************************
 * @file        qspi.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-03-29
 * @brief       QSPI 驱动代码
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
 * V1.0 20200329
 * 第一次发布
 *
 ****************************************************************************************************
 */

#include "./BSP/QSPI/qspi.h"


QSPI_HandleTypeDef qspi_handle;    /* QSPI句柄 */

/**
 * @brief       等待状态标志
 * @param       flag : 需要等待的标志位
 * @param       sta  : 需要等待的状态
 * @param       wtime: 等待时间
 * @retval      0, 等待成功; 1, 等待失败.
 */
uint8_t qspi_wait_flag(uint32_t flag, uint8_t sta, uint32_t wtime)
{
    uint8_t flagsta = 0;

    while (wtime)
    {
        flagsta = (QUADSPI->SR & flag) ? 1 : 0; /* 获取状态标志 */

        if (flagsta == sta)break;

        wtime--;
    }

    if (wtime)return 0;
    else return 1;
}

/**
 * @brief       初始化QSPI接口
 * @param       无
 * @retval      0, 成功; 1, 失败.
 */
uint8_t qspi_init(void)
{
    qspi_handle.Instance = QUADSPI;                                  /* QSPI */
    qspi_handle.Init.ClockPrescaler = 1;                             /* QPSI分频比，BY25Q128最大频率为108M，
                                                                        所以此处应该为2，QSPI频率就为220/(1+1)=110MHZ
                                                                        稍微有点超频，可以正常就好，不行就只能降低频率 */
    qspi_handle.Init.FifoThreshold = 4;                              /* FIFO阈值为4个字节 */
    qspi_handle.Init.SampleShifting = QSPI_SAMPLE_SHIFTING_HALFCYCLE;/* 采样移位半个周期(DDR模式下,必须设置为0) */
    qspi_handle.Init.FlashSize = POSITION_VAL(0X2000000)-1;          /* SPI FLASH大小，BY25Q128大小为32M字节 */
    qspi_handle.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_3_CYCLE; /* 片选高电平时间为3个时钟(9.1*3=27.3ns),即手册里面的tSHSL参数 */
    qspi_handle.Init.ClockMode = QSPI_CLOCK_MODE_3;                  /* 模式3 */
    qspi_handle.Init.FlashID = QSPI_FLASH_ID_1;                      /* 第一片flash */
    qspi_handle.Init.DualFlash = QSPI_DUALFLASH_DISABLE;             /* 禁止双闪存模式 */
    
    if(HAL_QSPI_Init(&qspi_handle) == HAL_OK) 
    {
        return 0;      /* QSPI初始化成功 */
    }
    else
    {
        return 1;
    }
}

/**
 * @brief       QSPI底层驱动,引脚配置，时钟使能
 * @param       hqspi:QSPI句柄
 * @note        此函数会被HAL_QSPI_Init()调用
 * @retval      0, 成功; 1, 失败.
 */
void HAL_QSPI_MspInit(QSPI_HandleTypeDef *hqspi)
{
    GPIO_InitTypeDef gpio_init_struct;
    
    __HAL_RCC_QSPI_CLK_ENABLE();      /* 使能QSPI时钟 */
    __HAL_RCC_GPIOB_CLK_ENABLE();     /* GPIOB时钟使能 */
    __HAL_RCC_GPIOD_CLK_ENABLE();     /* GPIOD时钟使能 */
    __HAL_RCC_GPIOE_CLK_ENABLE();     /* GPIOE时钟使能 */

    gpio_init_struct.Pin = QSPI_BK1_NCS_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_AF_PP;                     /* 复用 */
    gpio_init_struct.Pull = GPIO_PULLUP;                         /* 上拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;          /* 高速 */
    gpio_init_struct.Alternate = GPIO_AF10_QUADSPI;              /* 复用为QSPI */
    HAL_GPIO_Init(QSPI_BK1_NCS_GPIO_PORT, &gpio_init_struct);    /* 初始化QSPI_BK1_NCS引脚 */

    gpio_init_struct.Pin = QSPI_BK1_CLK_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_AF_PP;                     /* 复用 */
    gpio_init_struct.Pull = GPIO_PULLUP;                         /* 上拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;          /* 高速 */
    gpio_init_struct.Alternate = GPIO_AF9_QUADSPI;               /* 复用为QSPI */
    HAL_GPIO_Init(QSPI_BK1_CLK_GPIO_PORT, &gpio_init_struct);    /* 初始化QSPI_BK1_CLK引脚 */
    
    gpio_init_struct.Pin = QSPI_BK1_IO0_GPIO_PIN;
    HAL_GPIO_Init(QSPI_BK1_IO0_GPIO_PORT, &gpio_init_struct);    /* 初始化QSPI_BK1_IO0引脚 */
    
    gpio_init_struct.Pin = QSPI_BK1_IO1_GPIO_PIN;
    HAL_GPIO_Init(QSPI_BK1_IO1_GPIO_PORT, &gpio_init_struct);    /* 初始化QSPI_BK1_IO1引脚 */
    
    gpio_init_struct.Pin = QSPI_BK1_IO2_GPIO_PIN;
    HAL_GPIO_Init(QSPI_BK1_IO2_GPIO_PORT, &gpio_init_struct);    /* 初始化QSPI_BK1_IO2引脚 */
    
    gpio_init_struct.Pin = QSPI_BK1_IO3_GPIO_PIN;
    HAL_GPIO_Init(QSPI_BK1_IO3_GPIO_PORT, &gpio_init_struct);    /* 初始化QSPI_BK1_IO3引脚 */
}

/**
 * @brief       QSPI发送命令
 * @param       cmd : 要发送的指令
 * @param       addr: 发送到的目的地址
 * @param       mode: 模式,详细位定义如下:
 *   @arg       mode[1:0]: 指令模式; 00,无指令;  01,单线传输指令; 10,双线传输指令; 11,四线传输指令.
 *   @arg       mode[3:2]: 地址模式; 00,无地址;  01,单线传输地址; 10,双线传输地址; 11,四线传输地址.
 *   @arg       mode[5:4]: 地址长度; 00,8位地址; 01,16位地址;     10,24位地址;     11,32位地址.
 *   @arg       mode[7:6]: 数据模式; 00,无数据;  01,单线传输数据; 10,双线传输数据; 11,四线传输数据.
 * @param       dmcycle: 空指令周期数
 * @retval      无
 */
void qspi_send_cmd(uint8_t cmd, uint32_t addr, uint8_t mode, uint8_t dmcycle)
{
    QSPI_CommandTypeDef qspi_command_handle;

    qspi_command_handle.Instruction = cmd;                              /* 指令 */
    qspi_command_handle.Address = addr;                                 /* 地址 */
    qspi_command_handle.DummyCycles = dmcycle;                          /* 设置空指令周期数 */

    if(((mode >> 0) & 0x03) == 0)
    qspi_command_handle.InstructionMode = QSPI_INSTRUCTION_NONE;        /* 指令模式 */
    else if(((mode >> 0) & 0x03) == 1)
    qspi_command_handle.InstructionMode = QSPI_INSTRUCTION_1_LINE;      /* 指令模式 */
    else if(((mode >> 0) & 0x03) == 2)
    qspi_command_handle.InstructionMode = QSPI_INSTRUCTION_2_LINES;     /* 指令模式 */
    else if(((mode >> 0) & 0x03) == 3)
    qspi_command_handle.InstructionMode = QSPI_INSTRUCTION_4_LINES;     /* 指令模式 */

    if(((mode >> 2) & 0x03) == 0)
    qspi_command_handle.AddressMode = QSPI_ADDRESS_NONE;                /* 地址模式 */
    else if(((mode >> 2) & 0x03) == 1)
    qspi_command_handle.AddressMode = QSPI_ADDRESS_1_LINE;              /* 地址模式 */
    else if(((mode >> 2) & 0x03) == 2)
    qspi_command_handle.AddressMode = QSPI_ADDRESS_2_LINES;             /* 地址模式 */
    else if(((mode >> 2) & 0x03) == 3)
    qspi_command_handle.AddressMode = QSPI_ADDRESS_4_LINES;             /* 地址模式 */

    if(((mode >> 4)&0x03) == 0)
    qspi_command_handle.AddressSize = QSPI_ADDRESS_8_BITS;              /* 地址长度 */
    else if(((mode >> 4) & 0x03) == 1)
    qspi_command_handle.AddressSize = QSPI_ADDRESS_16_BITS;             /* 地址长度 */
    else if(((mode >> 4) & 0x03) == 2)
    qspi_command_handle.AddressSize = QSPI_ADDRESS_24_BITS;             /* 地址长度 */
    else if(((mode >> 4) & 0x03) == 3)
    qspi_command_handle.AddressSize = QSPI_ADDRESS_32_BITS;             /* 地址长度 */

    if(((mode >> 6) & 0x03) == 0)
    qspi_command_handle.DataMode=QSPI_DATA_NONE;                        /* 数据模式 */
    else if(((mode >> 6) & 0x03) == 1)
    qspi_command_handle.DataMode = QSPI_DATA_1_LINE;                    /* 数据模式 */
    else if(((mode >> 6) & 0x03) == 2)
    qspi_command_handle.DataMode = QSPI_DATA_2_LINES;                   /* 数据模式 */
    else if(((mode >> 6) & 0x03) == 3)
    qspi_command_handle.DataMode = QSPI_DATA_4_LINES;                   /* 数据模式 */

    qspi_command_handle.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;            /* 每次都发送指令 */
    qspi_command_handle.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;  /* 无交替字节 */
    qspi_command_handle.DdrMode = QSPI_DDR_MODE_DISABLE;                /* 关闭DDR模式 */
    qspi_command_handle.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;

    HAL_QSPI_Command(&qspi_handle, &qspi_command_handle, 5000);
}

/**
 * @brief       QSPI接收指定长度的数据
 * @param       buf     : 接收数据缓冲区首地址
 * @param       datalen : 要传输的数据长度
 * @retval      0, 成功; 其他, 错误代码.
 */
uint8_t qspi_receive(uint8_t *buf, uint32_t datalen)
{
    qspi_handle.Instance->DLR = datalen - 1;   /* 配置数据长度 */
    if (HAL_QSPI_Receive(&qspi_handle, buf, 5000) == HAL_OK) 
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

/**
 * @brief       QSPI发送指定长度的数据
 * @param       buf     : 发送数据缓冲区首地址
 * @param       datalen : 要传输的数据长度
 * @retval      0, 成功; 其他, 错误代码.
 */
uint8_t qspi_transmit(uint8_t *buf, uint32_t datalen)
{
    qspi_handle.Instance->DLR = datalen - 1; /* 配置数据长度 */
    if (HAL_QSPI_Transmit(&qspi_handle, buf, 5000) == HAL_OK)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}


















