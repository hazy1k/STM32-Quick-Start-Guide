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


/**
 * @brief       SPI初始化代码
 *   @note      主机模式,8位数据,禁止硬件片选
 * @param       无
 * @retval      无
 */
void spi2_init(void)
{
    uint32_t tempreg = 0;
    
    SPI2_SPI_CLK_ENABLE();          /* SPI2时钟使能 */
    SPI2_SCK_GPIO_CLK_ENABLE();     /* SPI2_SCK脚时钟使能 */
    SPI2_MISO_GPIO_CLK_ENABLE();    /* SPI2_MISO脚时钟使能 */
    SPI2_MOSI_GPIO_CLK_ENABLE();    /* SPI2_MOSI脚时钟使能 */

    sys_gpio_set(SPI2_SCK_GPIO_PORT, SPI2_SCK_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);   /* SCK引脚模式设置(复用输出) */

    sys_gpio_set(SPI2_MISO_GPIO_PORT, SPI2_MISO_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);   /* MISO引脚模式设置(复用输出) */

    sys_gpio_set(SPI2_MOSI_GPIO_PORT, SPI2_MOSI_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);   /* MOSI引脚模式设置(复用输出) */

    sys_gpio_af_set(SPI2_SCK_GPIO_PORT, SPI2_SCK_GPIO_PIN, SPI2_SCK_GPIO_AF);       /* SCK脚, AF功能设置 */
    sys_gpio_af_set(SPI2_MISO_GPIO_PORT, SPI2_MISO_GPIO_PIN, SPI2_SCK_GPIO_AF);     /* MISO脚, AF功能设置 */
    sys_gpio_af_set(SPI2_MOSI_GPIO_PORT, SPI2_MOSI_GPIO_PIN, SPI2_SCK_GPIO_AF);     /* MOSI脚, AF功能设置 */

    /* 配置SPI1/2/3的时钟源, 选择pll1_q_ck作为时钟源, 240Mhz */
    RCC->D2CCIP1R &= ~(7 << 12);    /* SPI123SEL[2:0]=0,清除原来的设置 */
    RCC->D2CCIP1R |= 0 << 12;       /* SPI123SEL[2:0]=1,选择pll1_q_ck作为SPI1/2/3的时钟源,一般为240Mhz */
 
    /* 这里只针对SPI口初始化 */
    RCC->APB1LRSTR |= 1 << 14;      /* 复位SPI2_SPI */
    RCC->APB1LRSTR &= ~(1 << 14);   /* 停止复位SPI2_SPI */

    SPI2_SPI->CR1 |= 1 << 12;       /* SSI=1,设置内部SS信号为高电平 */
    SPI2_SPI->CFG1 = 7 << 28;       /* MBR[2:0]=7,设置spi_ker_ck为256分频. */
    SPI2_SPI->CFG1 |= 7 << 0;       /* DSIZE[4:0]=7,设置SPI帧格式为8位,即字节传输 */
    tempreg = (uint32_t)1 << 31;    /* AFCNTR=1,SPI保持对IO口的控制 */
    tempreg |= 0 << 29;             /* SSOE=0,禁止硬件NSS输出 */
    tempreg |= 1 << 26;             /* SSM=1,软件管理NSS脚 */
    tempreg |= 1 << 25;             /* CPOL=1,空闲状态下,SCK为高电平 */
    tempreg |= 1 << 24;             /* CPHA=1,数据采样从第2个时间边沿开始 */
    tempreg |= 0 << 23;             /* LSBFRST=0,MSB先传输 */
    tempreg |= 1 << 22;             /* MASTER=1,主机模式 */
    tempreg |= 0 << 19;             /* SP[2:0]=0,摩托罗拉格式 */
    tempreg |= 0 << 17;             /* COMM[1:0]=0,全双工通信 */
    SPI2_SPI->CFG2 = tempreg;       /* 设置CFG2寄存器 */
    SPI2_SPI->I2SCFGR &= ~(1 << 0); /* 选择SPI模式 */
    SPI2_SPI->CR1 |= 1 << 0;        /* SPE=1,使能SPI2_SPI */

    spi2_read_write_byte(0xff);     /* 启动传输 */
}

/**
 * @brief       SPI2速度设置函数
 *   @note      SPI2时钟选择来自pll1_q_ck, 为240Mhz
 *              SPI速度 = spi_ker_ck / 2^(speed + 1)
 * @param       speed: SPI2时钟分频系数
 * @retval      无
 */
void spi2_set_speed(uint8_t speed)
{
    speed &= 0X07;                          /* 限制范围 */
    SPI2_SPI->CR1 &= ~(1 << 0);             /* SPE=0,SPI设备失能 */
    SPI2_SPI->CFG1 &= ~(7 << 28);           /* MBR[2:0]=0,清除原来的分频设置 */
    SPI2_SPI->CFG1 |= (uint32_t)speed << 28;/* MBR[2:0]=SpeedSet,设置SPI2_SPI速度 */
    SPI2_SPI->CR1 |= 1 << 0;                /* SPE=1,SPI设备使能 */
}

/**
 * @brief       SPI2读写一个字节数据
 * @param       txdata: 要发送的数据(1字节)
 * @retval      接收到的数据(1字节)
 */
uint8_t spi2_read_write_byte(uint8_t txdata)
{
    uint8_t rxdata = 0;
    SPI2_SPI->CR1 |= 1 << 0;    /* SPE=1,使能SPI2_SPI */
    SPI2_SPI->CR1 |= 1 << 9;    /* CSTART=1,启动传输 */

    while ((SPI2_SPI->SR & 1 << 1) == 0);   /* 等待发送区空 */

    /* 发送一个byte,以传输长度访问TXDR寄存器 */
    *(volatile uint8_t *)&SPI2_SPI->TXDR = txdata;  


    while ((SPI2_SPI->SR & 1 << 0) == 0);   /* 等待接收完一个byte */

    /* 接收一个byte,以传输长度访问RXDR寄存器 */
    rxdata = *(volatile uint8_t *)&SPI2_SPI->RXDR;  

    SPI2_SPI->IFCR |= 3 << 3;   /* EOTC和TXTFC置1,清除EOT和TXTFC位 */
    SPI2_SPI->CR1 &= ~(1 << 0); /* SPE=0,关闭SPI2_SPI,会执行状态机复位/FIFO重置等操作 */
    return rxdata;              /* 返回收到的数据 */
}






