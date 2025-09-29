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
    uint32_t tempreg = 0;
    
    RCC->AHB3ENR |= 1 << 14;            /* QSPI时钟使能 */
    QSPI_BK1_CLK_GPIO_CLK_ENABLE();     /* QSPI_BK1_CLK IO口时钟使能 */
    QSPI_BK1_NCS_GPIO_CLK_ENABLE();     /* QSPI_BK1_NCS IO口时钟使能 */
    QSPI_BK1_IO0_GPIO_CLK_ENABLE();     /* QSPI_BK1_IO0 IO口时钟使能 */
    QSPI_BK1_IO1_GPIO_CLK_ENABLE();     /* QSPI_BK1_IO1 IO口时钟使能 */
    QSPI_BK1_IO2_GPIO_CLK_ENABLE();     /* QSPI_BK1_IO2 IO口时钟使能 */
    QSPI_BK1_IO3_GPIO_CLK_ENABLE();     /* QSPI_BK1_IO3 IO口时钟使能 */
    
   sys_gpio_set(QSPI_BK1_CLK_GPIO_PORT, QSPI_BK1_CLK_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);   /* QSPI_BK1_CLK引脚模式设置 */

    sys_gpio_set(QSPI_BK1_NCS_GPIO_PORT, QSPI_BK1_NCS_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);   /* QSPI_BK1_NCS引脚模式设置 */

    sys_gpio_set(QSPI_BK1_IO0_GPIO_PORT, QSPI_BK1_IO0_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);   /* QSPI_BK1_IO0引脚模式设置 */

    sys_gpio_set(QSPI_BK1_IO1_GPIO_PORT, QSPI_BK1_IO1_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);   /* QSPI_BK1_IO1引脚模式设置 */

    sys_gpio_set(QSPI_BK1_IO2_GPIO_PORT, QSPI_BK1_IO2_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);   /* QSPI_BK1_IO2引脚模式设置 */

    sys_gpio_set(QSPI_BK1_IO3_GPIO_PORT, QSPI_BK1_IO3_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);   /* QSPI_BK1_IO3引脚模式设置 */
 
    sys_gpio_af_set(QSPI_BK1_CLK_GPIO_PORT, QSPI_BK1_CLK_GPIO_PIN, QSPI_BK1_CLK_GPIO_AF);   /* QSPI_BK1_CLK脚, AF功能设置 */
    sys_gpio_af_set(QSPI_BK1_NCS_GPIO_PORT, QSPI_BK1_NCS_GPIO_PIN, QSPI_BK1_NCS_GPIO_AF);   /* QSPI_BK1_NCS脚, AF功能设置 */
    sys_gpio_af_set(QSPI_BK1_IO0_GPIO_PORT, QSPI_BK1_IO0_GPIO_PIN, QSPI_BK1_IO0_GPIO_AF);   /* QSPI_BK1_IO0脚, AF功能设置 */
    sys_gpio_af_set(QSPI_BK1_IO1_GPIO_PORT, QSPI_BK1_IO1_GPIO_PIN, QSPI_BK1_IO1_GPIO_AF);   /* QSPI_BK1_IO1脚, AF功能设置 */
    sys_gpio_af_set(QSPI_BK1_IO2_GPIO_PORT, QSPI_BK1_IO2_GPIO_PIN, QSPI_BK1_IO2_GPIO_AF);   /* QSPI_BK1_IO2脚, AF功能设置 */
    sys_gpio_af_set(QSPI_BK1_IO3_GPIO_PORT, QSPI_BK1_IO3_GPIO_PIN, QSPI_BK1_IO3_GPIO_AF);   /* QSPI_BK1_IO3脚, AF功能设置 */

    RCC->AHB3RSTR |= 1 << 14;       /* 复位QSPI */
    RCC->AHB3RSTR &= ~(1 << 14);    /* 停止复位QSPI */
    
    if (qspi_wait_flag(1 << 5, 0, 0XFFFF) == 0) /* 等待BUSY空闲 */
    {
        /* 设置QSPI时钟来自pll2_r_ck=220Mhz */
        RCC->D1CCIPR &= ~(3 << 4);  /* 清除QSPISEL[1:0] */
        RCC->D1CCIPR |= 2 << 4;     /* QSPI时钟来自pll2_r_ck=220Mhz */
        tempreg = (2 - 1) << 24;    /* 设置QSPI时钟分频系数为2,即220M/2=110Mhz,9.1ns */
        tempreg |= (4 - 1) << 8;    /* 设置FIFO阈值为4个字节(最大为31,表示32个字节) */
        tempreg |= 0 << 7;          /* 选择FLASH1 */
        tempreg |= 0 << 6;          /* 禁止双闪存模式 */
        tempreg |= 1 << 4;          /* 采样移位半个周期(DDR模式下,必须设置为0) */
        QUADSPI->CR = tempreg;      /* 设置CR寄存器 */
        tempreg = (25 - 1) << 16;   /* 设置FLASH大小为2^25=32MB(为了适配sys.c里面的内存映射设置，实际上这里设置16M也是可以的) */
        tempreg |= (3 - 1) << 8;    /* 片选高电平时间为3个时钟(9.1*3=27.3ns),即手册里面的tSHSL参数 */
        tempreg |= 1 << 0;          /* Mode3,空闲时CLK为高电平 */
        QUADSPI->DCR = tempreg;     /* 设置DCR寄存器 */
        QUADSPI->CR |= 1 << 0;      /* 使能QSPI */
    }
    else
    {
        return 1;   /* 不成功 */
    }
    
    return 0;
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
    uint32_t tempreg = 0;
    uint8_t status;

    if (qspi_wait_flag(1 << 5, 0, 0XFFFF) == 0) /* 等待BUSY空闲 */
    {
        tempreg = 0 << 31;                      /* 禁止DDR模式 */
        tempreg |= 0 << 28;                     /* 每次都发送指令 */
        tempreg |= 0 << 26;                     /* 间接写模式 */
        tempreg |= ((uint32_t)mode >> 6) << 24; /* 设置数据模式 */
        tempreg |= (uint32_t)dmcycle << 18;     /* 设置空指令周期数 */
        tempreg |= ((uint32_t)(mode >> 4) & 0X03) << 12;    /* 设置地址长度 */
        tempreg |= ((uint32_t)(mode >> 2) & 0X03) << 10;    /* 设置地址模式 */
        tempreg |= ((uint32_t)(mode >> 0) & 0X03) << 8;     /* 设置指令模式 */ 
        tempreg |= cmd;                     /* 设置指令 */
        QUADSPI->CCR = tempreg;             /* 设置CCR寄存器 */

        if (mode & 0X0C)                    /* 有指令+地址要发送 */
        {
            QUADSPI->AR = addr;             /* 设置地址寄存器 */
        }

        if ((mode & 0XC0) == 0)             /* 无数据传输,等待指令发送完成 */
        {
            status = qspi_wait_flag(1 << 1, 1, 0XFFFF); /* 等待TCF,即传输完成 */

            if (status == 0)
            {
                QUADSPI->FCR |= 1 << 1;     /* 清除TCF标志位 */
            }
        }
    }
}

/**
 * @brief       QSPI接收指定长度的数据
 * @param       buf     : 接收数据缓冲区首地址
 * @param       datalen : 要传输的数据长度
 * @retval      0, 成功; 其他, 错误代码.
 */
uint8_t qspi_receive(uint8_t *buf, uint32_t datalen)
{
    uint32_t tempreg = QUADSPI->CCR;
    uint32_t addrreg = QUADSPI->AR;
    uint8_t status;
    volatile uint32_t *data_reg = &QUADSPI->DR;
    
    QUADSPI->DLR = datalen - 1; /* 设置数据传输长度 */
    tempreg &= ~(3 << 26);      /* 清除FMODE原来的设置 */
    tempreg |= 1 << 26;         /* 设置FMODE为间接读取模式 */
    QUADSPI->FCR |= 1 << 1;     /* 清除TCF标志位 */
    QUADSPI->CCR = tempreg;     /* 回写CCR寄存器 */
    QUADSPI->AR = addrreg;      /* 回写AR寄存器,触发传输 */

    while (datalen)
    {
        status = qspi_wait_flag(3 << 1, 1, 0XFFFF); /* 等到FTF和TCF,即接收到了数据 */

        if (status == 0)        /* 等待成功 */
        {
            *buf++ = *(volatile uint8_t *)data_reg;
            datalen--;
        }
        else
        {
            break;
        }
    }

    if (status == 0)
    {
        QUADSPI->CR |= 1 << 2;  /* 终止传输 */
        status = qspi_wait_flag(1 << 1, 1, 0XFFFF); /* 等待TCF,即数据传输完成 */

        if (status == 0)
        {
            QUADSPI->FCR |= 1 << 1; /* 清除TCF标志位 */
            status = qspi_wait_flag(1 << 5, 0, 0XFFFF); /* 等待BUSY位清零 */
        }
    }

    return status;
}

/**
 * @brief       QSPI发送指定长度的数据
 * @param       buf     : 发送数据缓冲区首地址
 * @param       datalen : 要传输的数据长度
 * @retval      0, 成功; 其他, 错误代码.
 */
uint8_t qspi_transmit(uint8_t *buf, uint32_t datalen)
{
    uint32_t tempreg = QUADSPI->CCR;
    uint32_t addrreg = QUADSPI->AR;
    uint8_t status;
    volatile uint32_t *data_reg = &QUADSPI->DR;
    
    QUADSPI->DLR = datalen - 1; /* 设置数据传输长度 */
    tempreg &= ~(3 << 26);      /* 清除FMODE原来的设置 */
    tempreg |= 0 << 26;         /* 设置FMODE为间接写入模式 */
    QUADSPI->FCR |= 1 << 1;     /* 清除TCF标志位 */
    QUADSPI->CCR = tempreg;     /* 回写CCR寄存器 */

    while (datalen)
    {
        status = qspi_wait_flag(1 << 2, 1, 0XFFFF); /* 等到FTF */

        if (status != 0)        /* 等待成功 */
        {
            break;
        }

        *(volatile uint8_t *)data_reg = *buf++;
        datalen--;
    }

    if (status == 0)
    {
        QUADSPI->CR |= 1 << 2;      /* 终止传输 */
        status = qspi_wait_flag(1 << 1, 1, 0XFFFF);     /* 等待TCF,即数据传输完成 */

        if (status == 0)
        {
            QUADSPI->FCR |= 1 << 1; /* 清除TCF标志位 */
            status = qspi_wait_flag(1 << 5, 0, 0XFFFF); /* 等待BUSY位清零 */
        }
    }

    return status;
}




















