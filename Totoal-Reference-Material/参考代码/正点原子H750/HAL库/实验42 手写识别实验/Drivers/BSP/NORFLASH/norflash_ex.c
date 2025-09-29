/**
 ****************************************************************************************************
 * @file        norflash_ex.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-03-29
 * @brief       QSPI 驱动代码
 *              因为STM32H7不支持QSPI接口读时写,因此该代码用于实现QSPI FLASH的数据写入,原理是：
 *              qspi.c、norflash.c和norflash_ex.c等3部分代码全部存储在H7的内部FLASH,保证操作
 *              QSPI FLASH的时候,不需要访问QSPI FLASH的代码. 从而实现QSPI FLASH数据写入.
 *
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
#include "./BSP/NORFLASH/norflash.h"
#include "./BSP/NORFLASH/norflash_ex.h"


extern uint8_t g_norflash_addrw;    /* 表示当前是24bit/32bit数据位宽, 在norflash.c里面定义 */

/**
 * @brief       QSPI接口进入内存映射模式
 *   @note      调用该函数之前务必已经初始化了QSPI接口
 *              sys_qspi_enable_memmapmode or norflash_init
 * @param       无
 * @retval      无
 */
static void norflash_ex_enter_mmap(void)
{
    uint32_t tempreg = 0;

    /* BY/W25QXX 写使能（0X06指令） */
    while (QUADSPI->SR & (1 << 5)); /* 等待BUSY位清零 */

    QUADSPI->CCR = 0X00000106;      /* 发送0X06指令，BY/W25QXX写使能 */

    while ((QUADSPI->SR & (1 << 1)) == 0);  /* 等待指令发送完成 */

    QUADSPI->FCR |= 1 << 1;

    if (qspi_wait_flag(1 << 5, 0, 0XFFFF) == 0) /* 等待BUSY空闲 */
    {
        tempreg = 0XEB;         /* INSTRUCTION[7:0]=0XEB,发送0XEB指令（Fast Read QUAD I/O） */
        tempreg |= 1 << 8;      /* IMODE[1:0]=1,单线传输指令 */
        tempreg |= 3 << 10;     /* ADDRESS[1:0]=3,四线传输地址 */ 
        tempreg |= (uint32_t)g_norflash_addrw << 12;    /* ADSIZE[1:0]=2,24/32位地址长度 */
        tempreg |= 3 << 14;     /* ABMODE[1:0]=3,四线传输交替字节 */
        tempreg |= 0 << 16;     /* ABSIZE[1:0]=0,8位交替字节(M0~M7) */
        tempreg |= 4 << 18;     /* DCYC[4:0]=4,4个dummy周期 */
        tempreg |= 3 << 24;     /* DMODE[1:0]=3,四线传输数据 */
        tempreg |= 3 << 26;     /* FMODE[1:0]=3,内存映射模式 */
        QUADSPI->CCR = tempreg; /* 设置CCR寄存器 */
    }

    sys_intx_enable();          /* 开启中断 */
}

/**
 * @brief       QSPI接口退出内存映射模式
 *   @note      调用该函数之前务必已经初始化了QSPI接口
 *              sys_qspi_enable_memmapmode or norflash_init
 * @param       无
 * @retval      0, OK;  其他, 错误代码
 */
static uint8_t norflash_ex_exit_mmap(void)
{
    uint8_t res = 0;

    sys_intx_disable();         /* 关闭中断 */
    SCB_InvalidateICache();     /* 清空I CACHE */
    SCB_InvalidateDCache();     /* 清空D CACHE */
    QUADSPI->CR &= ~(1 << 0);   /* 关闭 QSPI 接口 */
    QUADSPI->CR |= 1 << 1;      /* 退出MEMMAPED模式 */
    res = qspi_wait_flag(1 << 5, 0, 0XFFFF);    /* 等待BUSY空闲 */

    if (res == 0)
    {
        QUADSPI->CCR = 0;       /* CCR寄存器清零 */
        QUADSPI->CR |= 1 << 0;  /* 使能 QSPI 接口 */
    }

    return res;
}

/**
 * @brief       往 QSPI FLASH写入数据
 *   @note      在指定地址开始写入指定长度的数据
 *              该函数带擦除操作!
 * @param       pbuf    : 数据存储区
 * @param       addr    : 开始写入的地址(最大32bit)
 * @param       datalen : 要写入的字节数(最大65535)
 * @retval      0, OK;  其他, 错误代码
 */
uint8_t norflash_ex_write(uint8_t *pbuf, uint32_t addr, uint16_t datalen)
{
    uint8_t res = 0;
    res = norflash_ex_exit_mmap();  /* 退出内存映射模式 */

    if (res == 0)
    {
        norflash_write(pbuf, addr, datalen);
    }

    norflash_ex_enter_mmap();       /* 进入内存映射模式 */
    return res;
}

/**
 * @brief       从 QSPI FLASH 读取数据
 *   @note      在指定地址开始读取指定长度的数据（必须处于内存映射模式下，才可以执行）
 *
 * @param       pbuf    : 数据存储区
 * @param       addr    : 开始读取的地址(最大32bit)
 * @param       datalen : 要读取的字节数(最大65535)
 * @retval      0, OK;  其他, 错误代码
 */
void norflash_ex_read(uint8_t *pbuf, uint32_t addr, uint16_t datalen)
{
    uint16_t i = 0;
    addr += 0X90000000;     /* 使用内存映射模式读取，QSPI的基址是0X90000000，所以这里要加上基址 */
    sys_intx_disable();     /* 关闭中断 */

    for (i = 0; i < datalen; i++)
    {
        pbuf[i] = *(volatile uint8_t *)(addr + i);
    }

    sys_intx_enable();      /* 开启中断 */
}

/**
 * @brief       读取QSPI FLASH的ID
 * @param       无
 * @retval      NOR FLASH ID
 */
uint16_t norflash_ex_read_id(void)
{
    uint8_t res = 0;
    uint16_t id = 0; 
    res = norflash_ex_exit_mmap();  /* 退出内存映射模式 */

    if (res == 0)
    {
        id = norflash_read_id();
    }

    norflash_ex_enter_mmap();       /* 进入内存映射模式 */
    return id;
}

/**
 * @brief       擦除QSPI FLASH的某个扇区
 *   @note      注意,这里是扇区地址,不是字节地址!!
 *              擦除一个扇区的最少时间:150ms
 *
 * @param       saddr: 扇区地址
 * @retval      无
 */
void norflash_ex_erase_sector(uint32_t addr)
{
    uint8_t res = 0;
    res = norflash_ex_exit_mmap();  /* 退出内存映射模式 */

    if (res == 0)
    {
        norflash_erase_sector(addr);
    }

    norflash_ex_enter_mmap();       /* 进入内存映射模式 */
}

/**
 * @brief       擦除QSPI FLASH整个芯片
 *   @note      等待时间超长...
 *
 * @param       无
 * @retval      无
 */
void norflash_ex_erase_chip(void)
{
    uint8_t res = 0;
    res = norflash_ex_exit_mmap();  /* 退出内存映射模式 */

    if (res == 0)
    {
        norflash_erase_chip();
    }

    norflash_ex_enter_mmap();       /* 进入内存映射模式 */
}














