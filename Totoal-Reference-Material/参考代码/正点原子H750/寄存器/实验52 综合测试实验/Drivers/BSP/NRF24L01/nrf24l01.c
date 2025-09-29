/**
 ****************************************************************************************************
 * @file        nrf24l01.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-2
 * @brief       NRF24L01 驱动代码
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
#include "./BSP/NRF24L01/nrf24l01.h"
#include "./SYSTEM/delay/delay.h"


const uint8_t TX_ADDRESS[TX_ADR_WIDTH] = {0x34, 0x43, 0x10, 0x10, 0x01};    /* 发送地址 */
const uint8_t RX_ADDRESS[RX_ADR_WIDTH] = {0x34, 0x43, 0x10, 0x10, 0x01};    /* 发送地址 */

/**
 * @brief       初始化24L01的IO口
 *   @note      将SPI2模式改成SCK空闲低电平,及SPI 模式0
 * @param       无
 * @retval      无
 */
void nrf24l01_init(void)
{
    NRF24L01_CE_GPIO_CLK_ENABLE();  /* CE脚时钟使能 */
    NRF24L01_CSN_GPIO_CLK_ENABLE(); /* CSN脚时钟使能 */
    NRF24L01_IRQ_GPIO_CLK_ENABLE(); /* IRQ脚时钟使能 */

    sys_gpio_set(NRF24L01_CE_GPIO_PORT, NRF24L01_CE_GPIO_PIN,
             SYS_GPIO_MODE_OUT, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);  /* CE引脚模式设置(推挽输出) */

    sys_gpio_set(NRF24L01_CSN_GPIO_PORT, NRF24L01_CSN_GPIO_PIN,
         SYS_GPIO_MODE_OUT, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);      /* CSN引脚模式设置(推挽输出) */

    sys_gpio_set(NRF24L01_IRQ_GPIO_PORT, NRF24L01_IRQ_GPIO_PIN,
             SYS_GPIO_MODE_IN, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);   /* IRQ引脚模式设置(上拉输入) */

    spi2_init();    /* 初始化SPI2 */

    /* 针对NRF的特点修改SPI的设置 */
    SPI2->CR1 &= ~(1 << 0);     /* SPE=0,SPI设备失能 */
    SPI2->CFG2 &= ~(1 << 25);   /* CPOL=0,空闲状态下,SCK为低电平 */
    SPI2->CFG2 &= ~(1 << 24);   /* CPHA=0,数据采样从第1个时间边沿开始 */
    SPI2->CR1 |= 1 << 0;        /* SPE=1,SPI设备使能 */

    NRF24L01_CE(0);             /* 使能24L01 */
    NRF24L01_CSN(1);            /* SPI片选取消 */
}

/**
 * @brief       检测24L01是否存在
 * @param       无
 * @retval      0, 成功; 1, 失败;
 */
uint8_t nrf24l01_check(void)
{
    uint8_t buf[5] = {0XA5, 0XA5, 0XA5, 0XA5, 0XA5};
    uint8_t i;
    spi2_set_speed(SPI_SPEED_32);                           /* spi速度为7.5Mhz（24L01的最大SPI时钟为10Mhz） */
    nrf24l01_write_buf(NRF_WRITE_REG + TX_ADDR, buf, 5);    /* 写入5个字节的地址. */
    nrf24l01_read_buf(TX_ADDR, buf, 5);                     /* 读出写入的地址 */

    for (i = 0; i < 5; i++)
    {
        if (buf[i] != 0XA5) break;
    }
    
    if (i != 5) return 1;   /* 检测24L01错误 */

    return 0;               /* 检测到24L01 */
}

/**
 * @brief       NRF24L01写寄存器
 * @param       reg   : 寄存器地址
 * @param       value : 写入寄存器的值
 * @retval      状态寄存器值
 */
static uint8_t nrf24l01_write_reg(uint8_t reg, uint8_t value)
{
    uint8_t status;
    NRF24L01_CSN(0);                    /* 使能SPI传输 */
    status = spi2_read_write_byte(reg); /* 发送寄存器号 */
    spi2_read_write_byte(value);        /* 写入寄存器的值 */
    NRF24L01_CSN(1);                    /* 禁止SPI传输 */
    return status;                      /* 返回状态值 */
}

/**
 * @brief       NRF24L01读寄存器
 * @param       reg   : 寄存器地址
 * @retval      读取到的寄存器值;
 */
static uint8_t nrf24l01_read_reg(uint8_t reg)
{
    uint8_t reg_val;
    NRF24L01_CSN(0);            /* 使能SPI传输 */
    spi2_read_write_byte(reg);  /* 发送寄存器号 */
    reg_val = spi2_read_write_byte(0XFF);   /* 读取寄存器内容 */
    NRF24L01_CSN(1);            /* 禁止SPI传输 */
    return reg_val;             /* 返回状态值 */
}

/**
 * @brief       在指定位置读出指定长度的数据
 * @param       reg   : 寄存器地址
 * @param       pbuf  : 数据指针
 * @param       len   : 数据长度
 * @retval      状态寄存器值
 */
static uint8_t nrf24l01_read_buf(uint8_t reg, uint8_t *pbuf, uint8_t len)
{
    uint8_t status, i;
    NRF24L01_CSN(0);    /* 使能SPI传输 */
    status = spi2_read_write_byte(reg);         /* 发送寄存器值(位置),并读取状态值 */

    for (i = 0; i < len; i++)
    {
        pbuf[i] = spi2_read_write_byte(0XFF);   /* 读出数据 */
    }
    
    NRF24L01_CSN(1);    /* 关闭SPI传输 */
    return status;      /* 返回读到的状态值 */
}

/**
 * @brief       在指定位置写指定长度的数据
 * @param       reg   : 寄存器地址
 * @param       pbuf  : 数据指针
 * @param       len   : 数据长度
 * @retval      状态寄存器值
 */
static uint8_t nrf24l01_write_buf(uint8_t reg, uint8_t *pbuf, uint8_t len)
{
    uint8_t status, i;
    NRF24L01_CSN(0);    /* 使能SPI传输 */
    status = spi2_read_write_byte(reg);/* 发送寄存器值(位置),并读取状态值 */

    for (i = 0; i < len; i++)
    {
        spi2_read_write_byte(*pbuf++); /* 写入数据 */
    }
    
    NRF24L01_CSN(1);    /* 关闭SPI传输 */
    return status;      /* 返回读到的状态值 */
}

/**
 * @brief       启动NRF24L01发送一次数据(数据长度 = TX_PLOAD_WIDTH)
 * @param       ptxbuf : 待发送数据首地址
 * @retval      发送完成状态
 *   @arg       0    : 发送成功
 *   @arg       1    : 达到最大发送次数,失败
 *   @arg       0XFF : 其他错误
 */
uint8_t nrf24l01_tx_packet(uint8_t *ptxbuf)
{
    uint8_t sta;
    uint8_t rval = 0XFF;
    
    NRF24L01_CE(0);
    nrf24l01_write_buf(WR_TX_PLOAD, ptxbuf, TX_PLOAD_WIDTH);    /* 写数据到TX BUF  TX_PLOAD_WIDTH个字节 */
    NRF24L01_CE(1); /* 启动发送 */

    while (NRF24L01_IRQ != 0);          /* 等待发送完成 */

    sta = nrf24l01_read_reg(STATUS);    /* 读取状态寄存器的值 */
    nrf24l01_write_reg(NRF_WRITE_REG + STATUS, sta);    /* 清除TX_DS或MAX_RT中断标志 */

    if (sta & MAX_TX)   /* 达到最大重发次数 */
    {
        nrf24l01_write_reg(FLUSH_TX, 0xff); /* 清除TX FIFO寄存器 */
        rval = 1;
    }

    if (sta & TX_OK)/* 发送完成 */
    {
        rval = 0;   /* 标记发送成功 */
    }

    return rval;    /* 返回结果 */
}

/**
 * @brief       启动NRF24L01接收一次数据(数据长度 = RX_PLOAD_WIDTH)
 * @param       prxbuf : 接收数据缓冲区首地址
 * @retval      接收完成状态
 *   @arg       0 : 接收成功
 *   @arg       1 : 失败
 */
uint8_t nrf24l01_rx_packet(uint8_t *prxbuf)
{
    uint8_t sta;
    uint8_t rval = 1;
    
    sta = nrf24l01_read_reg(STATUS); /* 读取状态寄存器的值 */
    nrf24l01_write_reg(NRF_WRITE_REG + STATUS, sta); /* 清除RX_OK中断标志 */

    if (sta & RX_OK)    /* 接收到数据 */
    {
        nrf24l01_read_buf(RD_RX_PLOAD, prxbuf, RX_PLOAD_WIDTH); /* 读取数据 */
        nrf24l01_write_reg(FLUSH_RX, 0xff); /* 清除RX FIFO寄存器 */
        rval = 0;       /* 标记接收完成 */
    }

    return rval;    /* 返回结果 */
}

/**
 * @brief       NRF24L01进入接收模式
 *   @note      设置RX地址,写RX数据宽度,选择RF频道,波特率
 *              当CE变高后,即进入RX模式,并可以接收数据了
 * @param       无
 * @retval      无
 */
void nrf24l01_rx_mode(void)
{
    NRF24L01_CE(0);
    nrf24l01_write_buf(NRF_WRITE_REG + RX_ADDR_P0, (uint8_t *)RX_ADDRESS, RX_ADR_WIDTH);    /* 写RX节点地址 */

    nrf24l01_write_reg(NRF_WRITE_REG + EN_AA, 0x01);        /* 使能通道0的自动应答 */
    nrf24l01_write_reg(NRF_WRITE_REG + EN_RXADDR, 0x01);    /* 使能通道0的接收地址 */
    nrf24l01_write_reg(NRF_WRITE_REG + RF_CH, 40);          /* 设置RF通信频率 */
    nrf24l01_write_reg(NRF_WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH);   /* 选择通道0的有效数据宽度 */
    nrf24l01_write_reg(NRF_WRITE_REG + RF_SETUP, 0x0f);     /* 设置TX发射参数,0db增益,2Mbps */
    nrf24l01_write_reg(NRF_WRITE_REG + CONFIG, 0x0f);       /* 配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式 */
    NRF24L01_CE(1); /* CE为高,进入接收模式 */
}

/**
 * @brief       NRF24L01进入发送模式
 *   @note      设置TX地址,写TX数据宽度,设置RX自动应答的地址,填充TX发送数据,选择RF频道,波特率和
 *              PWR_UP,CRC使能
 *              当CE变高后,即进入TX模式,并可以发送数据了, CE为高大于10us,则启动发送.
 * @param       无
 * @retval      无
 */
void nrf24l01_tx_mode(void)
{
    NRF24L01_CE(0);
    nrf24l01_write_buf(NRF_WRITE_REG + TX_ADDR, (uint8_t *)TX_ADDRESS, TX_ADR_WIDTH);       /* 写TX节点地址 */
    nrf24l01_write_buf(NRF_WRITE_REG + RX_ADDR_P0, (uint8_t *)RX_ADDRESS, RX_ADR_WIDTH);    /* 设置RX节点地址,主要为了使能ACK */

    nrf24l01_write_reg(NRF_WRITE_REG + EN_AA, 0x01);        /* 使能通道0的自动应答 */
    nrf24l01_write_reg(NRF_WRITE_REG + EN_RXADDR, 0x01);    /* 使能通道0的接收地址 */
    nrf24l01_write_reg(NRF_WRITE_REG + SETUP_RETR, 0x1a);   /* 设置自动重发间隔时间:500us + 86us;最大自动重发次数:10次 */
    nrf24l01_write_reg(NRF_WRITE_REG + RF_CH, 40);          /* 设置RF通道为40 */
    nrf24l01_write_reg(NRF_WRITE_REG + RF_SETUP, 0x0f);     /* 设置TX发射参数,0db增益,2Mbps */
    nrf24l01_write_reg(NRF_WRITE_REG + CONFIG, 0x0e);       /* 配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式,开启所有中断 */
    NRF24L01_CE(1); /* CE为高,10us后启动发送 */
}










