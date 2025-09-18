#include "quadspi.h"

QSPI_HandleTypeDef g_qspi_handle;    /* QSPI句柄 */

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
    g_qspi_handle.Instance = QUADSPI;                                  /* QSPI */
    g_qspi_handle.Init.ClockPrescaler = 2;                             /* QPSI分频比，BY25Q128最大频率为108M，
                                                                          所以此处应该为2，QSPI频率就为220/(1+1)=110MHZ
                                                                          稍微有点超频，可以正常就好，不行就只能降低频率 */
    g_qspi_handle.Init.FifoThreshold = 4;                              /* FIFO阈值为4个字节 */
    g_qspi_handle.Init.SampleShifting = QSPI_SAMPLE_SHIFTING_HALFCYCLE;/* 采样移位半个周期(DDR模式下,必须设置为0) */
    g_qspi_handle.Init.FlashSize = 25-1;                               /* SPI FLASH大小，BY25Q128大小为32M字节,2^25，所以取权值25-1=24 */
    g_qspi_handle.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_3_CYCLE; /* 片选高电平时间为3个时钟(9.1*3=27.3ns),即手册里面的tSHSL参数 */
    g_qspi_handle.Init.ClockMode = QSPI_CLOCK_MODE_3;                  /* 模式3 */
    g_qspi_handle.Init.FlashID = QSPI_FLASH_ID_1;                      /* 第一片flash */
    g_qspi_handle.Init.DualFlash = QSPI_DUALFLASH_DISABLE;             /* 禁止双闪存模式 */

    if(HAL_QSPI_Init(&g_qspi_handle) == HAL_OK)
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

    HAL_QSPI_Command(&g_qspi_handle, &qspi_command_handle, 5000);
}

/**
 * @brief       QSPI接收指定长度的数据
 * @param       buf     : 接收数据缓冲区首地址
 * @param       datalen : 要传输的数据长度
 * @retval      0, 成功; 其他, 错误代码.
 */
uint8_t qspi_receive(uint8_t *buf, uint32_t datalen)
{
    g_qspi_handle.Instance->DLR = datalen - 1;   /* 配置数据长度 */
    if (HAL_QSPI_Receive(&g_qspi_handle, buf, 5000) == HAL_OK)
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
    g_qspi_handle.Instance->DLR = datalen - 1; /* 配置数据长度 */
    if (HAL_QSPI_Transmit(&g_qspi_handle, buf, 5000) == HAL_OK)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

/**
 * @brief       QSPI进入内存映射模式（执行QSPI代码必备前提）
 *   @note      必须根据所使用QSPI FLASH的容量设置正确的ftype值!
 * @param       ftype: flash类型
 *   @arg           0, 普通FLASH, 容量在128Mbit及以内的
 *   @arg           1, 大容量FLASH, 容量在256Mbit及以上的.
 * @retval      无
 */
void sys_qspi_enable_memmapmode(uint8_t ftype)
{
    uint32_t tempreg = 0;
    GPIO_InitTypeDef qspi_gpio;

    __HAL_RCC_GPIOB_CLK_ENABLE();                            /* 使能PORTB时钟 */
    __HAL_RCC_GPIOD_CLK_ENABLE();                            /* 使能PORTD时钟 */
    __HAL_RCC_GPIOE_CLK_ENABLE();                            /* 使能PORTE时钟 */
    __HAL_RCC_QSPI_CLK_ENABLE();                             /* QSPI时钟使能 */

    qspi_gpio.Pin = GPIO_PIN_6;                              /* PB6 AF10 */
    qspi_gpio.Mode = GPIO_MODE_AF_PP;
    qspi_gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    qspi_gpio.Pull = GPIO_PULLUP;
    qspi_gpio.Alternate = GPIO_AF10_QUADSPI;
    HAL_GPIO_Init(GPIOB, &qspi_gpio);

    qspi_gpio.Pin = GPIO_PIN_2;                              /* PB2 AF9 */
    qspi_gpio.Alternate = GPIO_AF9_QUADSPI;
    HAL_GPIO_Init(GPIOB, &qspi_gpio);

    qspi_gpio.Pin = GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13; /* PD11,12,13 AF9 */
    qspi_gpio.Alternate = GPIO_AF9_QUADSPI;
    HAL_GPIO_Init(GPIOD, &qspi_gpio);

    qspi_gpio.Pin = GPIO_PIN_2;                              /* PE2 AF9 */
    qspi_gpio.Alternate = GPIO_AF9_QUADSPI;
    HAL_GPIO_Init(GPIOE, &qspi_gpio);

    /* QSPI设置，参考QSPI实验的QSPI_Init函数 */
    RCC->AHB3RSTR |= 1 << 14;       /* 复位QSPI */
    RCC->AHB3RSTR &= ~(1 << 14);    /* 停止复位QSPI */

    while (QUADSPI->SR & (1 << 5)); /* 等待BUSY位清零 */

    /* QSPI时钟源已经在sys_stm32_clock_init()函数中设置 */
    QUADSPI->CR = 0X01000310;       /* 设置CR寄存器, 这些值怎么来的，请参考QSPI实验/看H750参考手册寄存器描述分析 */
    QUADSPI->DCR = 0X00180201;      /* 设置DCR寄存器(FLASH容量32M(最大容量设置为32M, 默认用16M的), tSHSL = 3个时钟) */
    QUADSPI->CR |= 1 << 0;          /* 使能QSPI */

    /*
     *  注意:QSPI QE位的使能，在QSPI烧写算法里面，就已经设置了
     *  所以, 这里可以不用设置QE位，否则需要加入对QE位置1的代码
     *  不过, 代码必须通过仿真器下载, 直接烧录到外部QSPI FLASH, 是不可用的
     *  如果想直接烧录到外部QSPI FLASH也可以用, 则需要在这里添加QE位置1的代码
     *
     *  另外, 对与W25Q256,还需要使能4字节地址模式,或者设置S3的ADP位为1.
     *  我们在QSPI烧写算法里面已经设置了ADP=1(上电即32位地址模式),因此这里也
     *  不需要发送进入4字节地址模式指令/设置ADP=1了, 否则还需要设置ADP=1
     */

    /* BY/W25QXX 写使能（0X06指令）*/
    while (QUADSPI->SR & (1 << 5)); /* 等待BUSY位清零 */

    QUADSPI->CCR = 0X00000106;      /* 发送0X06指令，BY/W25QXX写使能 */

    while ((QUADSPI->SR & (1 << 1)) == 0);/* 等待指令发送完成 */

    QUADSPI->FCR |= 1 << 1;         /* 清除发送完成标志位 */

    /* MemroyMap 模式设置 */
    while (QUADSPI->SR & (1 << 5)); /* 等待BUSY位清零 */

    QUADSPI->ABR = 0;               /* 交替字节设置为0，实际上就是25QXX 0XEB指令的, M0~M7 = 0 */
    tempreg = 0XEB;                 /* INSTRUCTION[7:0] = 0XEB, 发送0XEB指令（Fast Read QUAD I/O） */
    tempreg |= 1 << 8;              /* IMODE[1:0] = 1, 单线传输指令 */
    tempreg |= 3 << 10;             /* ADDRESS[1:0] = 3, 四线传输地址 */
    tempreg |= (2 + ftype) << 12;   /* ADSIZE[1:0] = 2/3, 24位(ftype = 0) / 32位(ftype = 1)地址长度 */
    tempreg |= 3 << 14;             /* ABMODE[1:0] = 3, 四线传输交替字节 */
    tempreg |= 0 << 16;             /* ABSIZE[1:0] = 0, 8位交替字节(M0~M7) */
    tempreg |= 4 << 18;             /* DCYC[4:0] = 4, 4个dummy周期 */
    tempreg |= 3 << 24;             /* DMODE[1:0] = 3, 四线传输数据 */
    tempreg |= 3 << 26;             /* FMODE[1:0] = 3, 内存映射模式 */
    QUADSPI->CCR = tempreg;         /* 设置CCR寄存器 */

    /* 设置QSPI FLASH空间的MPU保护 */
    SCB->SHCSR &= ~(1 << 16);       /* 禁止MemManage */
    MPU->CTRL &= ~(1 << 0);         /* 禁止MPU */
    MPU->RNR = 0;                   /* 设置保护区域编号为0(1~7可以给其他内存用) */
    MPU->RBAR = 0X90000000;         /* 基地址为0X9000 000, 即QSPI的起始地址 */
    MPU->RASR = 0X0303002D;         /* 设置相关保护参数(禁止共用, 允许cache, 允许缓冲), 详见MPU实验的解析 */
    MPU->CTRL = (1 << 2) | (1 << 0);/* 使能PRIVDEFENA, 使能MPU */
    SCB->SHCSR |= 1 << 16;          /* 使能MemManage */
}
