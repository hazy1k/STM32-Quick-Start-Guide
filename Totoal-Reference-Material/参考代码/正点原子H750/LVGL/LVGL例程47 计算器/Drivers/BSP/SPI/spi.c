/**
 ****************************************************************************************************
 * @file        spi.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-2
 * @brief       SPI 驱动代码
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
 * V1.0 20200402
 * 第一次发布
 *
 ****************************************************************************************************
 */

#include "./BSP/SPI/spi.h"


SPI_HandleTypeDef g_spi_handle;  /* SPI句柄 */

/**
 * @brief       SPI初始化代码
 *   @note      主机模式,8位数据,禁止硬件片选
 * @param       无
 * @retval      无
 */
void spi2_init(void)
{
    g_spi_handle.Instance = SPI2;                                           /* SP2 */
    g_spi_handle.Init.Mode = SPI_MODE_MASTER;                               /* 设置SPI工作模式，设置为主模式 */
    g_spi_handle.Init.Direction = SPI_DIRECTION_2LINES;                     /* 设置SPI单向或者双向的数据模式:SPI设置为双线模式 */
    g_spi_handle.Init.DataSize = SPI_DATASIZE_8BIT;                         /* 设置SPI的数据大小:SPI发送接收8位帧结构 */
    g_spi_handle.Init.CLKPolarity = SPI_POLARITY_HIGH;                      /* 串行同步时钟的空闲状态为高电平 */
    g_spi_handle.Init.CLKPhase = SPI_PHASE_2EDGE;                           /* 串行同步时钟的第二个跳变沿（上升或下降）数据被采样 */
    g_spi_handle.Init.NSS = SPI_NSS_SOFT;                                   /* NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制 */
    g_spi_handle.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;                     /* NSS信号脉冲失能 */
    g_spi_handle.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_ENABLE;  /* SPI主模式IO状态保持使能 */
    g_spi_handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;        /* 定义波特率预分频的值:波特率预分频值为256 */
    g_spi_handle.Init.FirstBit = SPI_FIRSTBIT_MSB;                          /* 指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始 */
    g_spi_handle.Init.TIMode = SPI_TIMODE_DISABLE;                          /* 关闭TI模式 */
    g_spi_handle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;          /* 关闭硬件CRC校验 */
    g_spi_handle.Init.CRCPolynomial = 7;                                    /* CRC值计算的多项式 */
    HAL_SPI_Init(&g_spi_handle);
    
    __HAL_SPI_ENABLE(&g_spi_handle);                                        /* 使能SPI2 */
    spi2_read_write_byte(0Xff);                                             /* 启动传输 */
}

/**
 * @brief       SPI底层驱动，时钟使能，引脚配置
 * @param       hspi:SPI句柄
 * @note        此函数会被HAL_SPI_Init()调用
 * @retval      无
 */
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
    GPIO_InitTypeDef gpio_init_struct;
    
    RCC_PeriphCLKInitTypeDef rcc_periph_clk_init;

    SPI2_SPI_CLK_ENABLE();          /* SPI2时钟使能 */
    SPI2_SCK_GPIO_CLK_ENABLE();     /* SPI2_SCK脚时钟使能 */
    SPI2_MISO_GPIO_CLK_ENABLE();    /* SPI2_MISO脚时钟使能 */
    SPI2_MOSI_GPIO_CLK_ENABLE();    /* SPI2_MOSI脚时钟使能 */

    /* 设置SPI2的时钟源 */
    rcc_periph_clk_init.PeriphClockSelection = RCC_PERIPHCLK_SPI2;      /* 设置SPI2时钟源 */
    rcc_periph_clk_init.Spi123ClockSelection = RCC_SPI123CLKSOURCE_PLL; /* SPI2时钟源使用PLL1Q */
    HAL_RCCEx_PeriphCLKConfig(&rcc_periph_clk_init);

    gpio_init_struct.Pin = SPI2_SCK_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_AF_PP;                /* 复用推挽输出 */
    gpio_init_struct.Pull = GPIO_PULLUP;                    /* 上拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;     /* 高速 */
    gpio_init_struct.Alternate = SPI2_SCK_GPIO_AF;          /* 复用 */
    HAL_GPIO_Init(SPI2_SCK_GPIO_PORT, &gpio_init_struct);   /* 初始化SCK引脚 */

    gpio_init_struct.Pin = SPI2_MISO_GPIO_PIN;
    gpio_init_struct.Alternate = SPI2_MISO_GPIO_AF;         /* 复用 */
    HAL_GPIO_Init(SPI2_MISO_GPIO_PORT, &gpio_init_struct);  /* 初始化MISO引脚 */
    
    gpio_init_struct.Pin = SPI2_MOSI_GPIO_PIN;
    gpio_init_struct.Alternate = SPI2_MOSI_GPIO_AF;         /* 复用 */
    HAL_GPIO_Init(SPI2_MOSI_GPIO_PORT, &gpio_init_struct);  /* 初始化MOSI引脚 */
}

/**
 * @brief       SPI2速度设置函数
 *   @note      SPI2时钟选择来自pll1_q_ck, 为240Mhz
 *              SPI速度 = spi_ker_ck / 2^(speed + 1)
 * @param       speed: SPI2时钟分频系数,SPI_BAUDRATEPRESCALER_2~SPI_BAUDRATEPRESCALER_256
 * @retval      无
 */
void spi2_set_speed(uint32_t speed)
{
    assert_param(IS_SPI_BAUDRATE_PRESCALER(speed)); /* 判断有效性 */
    __HAL_SPI_DISABLE(&g_spi_handle);               /* 关闭SPI */
    g_spi_handle.Instance->CFG1 &= ~(0X7<<28);      /* 位30-28清零，用来设置波特率 */
    g_spi_handle.Instance->CFG1 |= speed;           /* 设置SPI速度 */
    __HAL_SPI_ENABLE(&g_spi_handle);                /* 使能SPI */
}

/**
 * @brief       SPI2读写一个字节数据
 * @param       txdata: 要发送的数据(1字节)
 * @retval      接收到的数据(1字节)
 */
uint8_t spi2_read_write_byte(uint8_t txdata)
{
    uint8_t rxdata;
    HAL_SPI_TransmitReceive(&g_spi_handle,&txdata,&rxdata,1, 1000);
    return rxdata;    /* 返回收到的数据 */
}













