/**
 ****************************************************************************************************
 * @file        stmflash.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-2
 * @brief       STM32内部FLASH读写 驱动代码
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

#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/STMFLASH/stmflash.h"


/**
 * @brief       得到FLASH的错误状态
 * @param       无
 * @retval      错误代码
 *   @arg       0   , 无错误
 *   @arg       其他, 错误编号
 */
static uint8_t stmflash_get_error_status(void)
{
    uint32_t res = 0;
    res = FLASH->SR1;

    if (res & (1 << 17)) return 1;  /* WRPERR=1,写保护错误 */
    if (res & (1 << 18)) return 2;  /* PGSERR=1,编程序列错误 */
    if (res & (1 << 19)) return 3;  /* STRBERR=1,复写错误 */
    if (res & (1 << 21)) return 4;  /* INCERR=1,数据一致性错误 */
    if (res & (1 << 22)) return 5;  /* OPERR=1,写/擦除错误 */
    if (res & (1 << 23)) return 6;  /* RDPERR=1,读保护错误 */
    if (res & (1 << 24)) return 7;  /* RDSERR=1,非法访问加密区错误 */
    if (res & (1 << 25)) return 8;  /* SNECCERR=1,1bit ecc校正错误 */
    if (res & (1 << 26)) return 9;  /* DBECCERR=1,2bit ecc错误 */

    return 0;   /* 没有任何状态/操作完成. */
}

/**
 * @brief       等待操作完成
 * @param       time : 要延时的长短
 * @retval      错误代码
 *   @arg       0   : 已完成
 *   @arg       1~9 : 错误代码
 *   @arg       0XFF: 超时
 */
static uint8_t stmflash_wait_done(uint32_t time)
{
    uint8_t res = 0;
    uint32_t tempreg = 0;

    while (1)
    {
        tempreg = FLASH->SR1;

        if ((tempreg & 0X07) == 0)
        {
            break;  /* BSY=0,WBNE=0,QW=0,则操作完成 */
        }
        
        time--;

        if (time == 0)return 0XFF;
    }

    res = stmflash_get_error_status();

    if (res)
    {
        FLASH->CCR1 = 0X07EE0000;   /* 清所有错误标志 */
    }
    
    return res;
}

/**
 * @brief       擦除扇区0
 * @param       无
 * @retval      错误代码
 *   @arg       0   : 已完成
 *   @arg       1~9 : 错误代码
 *   @arg       0XFF: 超时
 */
static uint8_t stmflash_erase_sector0(void)
{
    uint8_t res = 0;
    res = stmflash_wait_done(0XFFFFFFFF);   /* 等待上次操作结束 */

    if (res == 0)
    {
        FLASH->CR1 &= ~(7 << 8);    /* SNB1[2:0]=0,清除原来的设置 */
        FLASH->CR1 &= ~(3 << 4);    /* PSIZE1[1:0]=0,清除原来的设置 */
        FLASH->CR1 |= (uint32_t)0 << 8; /* 设置要擦除的扇区编号,0 */
        FLASH->CR1 |= 2 << 4;       /* 设置为32bit宽 */
        FLASH->CR1 |= 1 << 2;       /* SER1=1,扇区擦除 */
        FLASH->CR1 |= 1 << 7;       /* START1=1,开始擦除 */
        res = stmflash_wait_done(0XFFFFFFFF);   /* 等待操作结束 */
        FLASH->CR1 &= ~(1 << 2);    /* SER1=0,清除扇区擦除标志 */
    }

    return res;
}

/**
 * @brief       在FLASH指定地址写8个字,即256bit
 *   @note      必须以256bit为单位(32字节)编程!!
 * @param       faddr : 指定地址(此地址必须为4的倍数!!)
 * @param       pdata : 要写入的数据
 * @retval      错误代码
 *   @arg       0   : 写入成功
 *   @arg       其他: 错误代码
 */
static uint8_t stmflash_write_8word(uint32_t faddr, uint32_t *pdata)
{
    volatile uint8_t nword = 8; /* 每次写8个字,256bit */
    uint8_t res;
    res = stmflash_wait_done(0XFFFF);

    if (res == 0)       /* OK */
    {
        FLASH->CR1 &= ~(3 << 4);/* PSIZE1[1:0]=0,清除原来的设置 */
        FLASH->CR1 |= 2 << 4;   /* 设置为32bit宽 */
        FLASH->CR1 |= 1 << 1;   /* PG1=1,编程使能 */

        __DSB();

        while (nword)
        {
            *(volatile uint32_t *)faddr = *pdata;   /* 写入数据 */
            faddr += 4;         /* 写地址+4 */
            pdata++;            /* 偏移到下一个数据首地址 */
            nword--;
        }

        __DSB();                /* 写操作完成后,屏蔽数据同步,使CPU重新执行指令序列 */
        res = stmflash_wait_done(0XFFFF);   /* 等待操作完成,一个字编程,最多100us. */
        FLASH->CR1 &= ~(1 << 1);/* PG1=0,清除扇区擦除标志 */
    }

    return res;
}

/**
 * @brief       读取指定地址的一个字(32位数据)
 * @param       faddr : 要读取的地址
 * @retval      读取到的数据
 */
uint32_t stmflash_read_word(uint32_t faddr)
{
    return *(volatile uint32_t *)faddr;
}

/**
 * @brief       从指定地址开始写入指定长度的数据
 *   @note      特别注意：因为STM32H750只有一个扇区（128K）,因此我们规定：前16K留作BootLoader用
 *              后112K用作APP用，我们要做写入测试，尽量使用16K以后的地址，否则容易出问题
 *              另外，由于写数据时，必须是0XFF才可以写入数据，因此不可避免的需要擦除扇区
 *              所以在擦除时需要先对前16K数据做备份保存（读取到RAM），然后再写入，以保证
 *              前16K数据的完整性。且执行写入操作的时候，不能发生任何中断（凡是在写入时执
 *              行内部FLASH代码，必将导致hardfault）。
 * @param       waddr : 起始地址(此地址必须为32的倍数!!,否则写入出错!)
 * @param       pbuf  : 数据指针
 * @param       length: 字(32位)数(就是要写入的32位数据的个数,一次至少写入32字节,即8个字)
 * @retval      无
 */

/* FLASH 写入数据缓存 */
uint32_t g_flashbuf[BOOT_FLASH_SIZE / 4];

void stmflash_write(uint32_t waddr, uint32_t *pbuf, uint32_t length)
{
    uint8_t status = 0;
    uint32_t addrx = 0;
    uint32_t endaddr = 0;
    uint16_t wbfcyc = BOOT_FLASH_SIZE/32;    /* 写bootflashbuf时,需要执行的循环数 */
    uint32_t *wbfptr;
    uint32_t wbfaddr;

    if (waddr < (STM32_FLASH_BASE + BOOT_FLASH_SIZE))return;    /* 写入地址小于STM32_FLASH_BASE+BOOT_FLASH_SIZE,非法. */

    if (waddr > (STM32_FLASH_BASE + STM32_FLASH_SIZE))return;   /* 写入地址大于STM32总FLASH地址范围,非法. */

    if (waddr % 32)return;          /* 写入地址不是32字节倍数,非法. */

    HAL_FLASH_Unlock();             /* 解锁 */
    addrx = waddr;                  /* 写入的起始地址 */
    endaddr = waddr + length * 4;   /* 写入的结束地址 */

    while (addrx < endaddr)         /* 扫清一切障碍.(对非FFFFFFFF的地方,先擦除) */
    {
        if (stmflash_read_word(addrx) != 0XFFFFFFFF) /* 有非0XFFFFFFFF的地方,要擦除这个扇区 */
        {   
            stmflash_read(STM32_FLASH_BASE, g_flashbuf, BOOT_FLASH_SIZE / 4);  /* 读出BOOT_FLASH_SIZE大小数据 */
            sys_intx_disable(); /* 禁止所有中断 */

            status = stmflash_erase_sector0();
            
            if (status) /* 发生错误了 */
            {
                sys_intx_enable();    /* 允许中断 */
                break;                /* 发生错误了 */
            }

            SCB_CleanInvalidateDCache();    /* 清除无效的D-Cache */
            wbfptr = g_flashbuf;            /* 指向g_flashbuf首地址 */
            wbfaddr = STM32_FLASH_BASE;     /* 指向STM32 FLASH首地址 */

            while (wbfcyc)                  /* 写数据 */
            {
                if (stmflash_write_8word(wbfaddr, wbfptr))  /* 写入数据 */
                {
                    break;  /* 写入异常 */
                }
                wbfaddr += 32;
                wbfptr += 8;
                wbfcyc--;
            }
            sys_intx_enable();  /* 允许中断 */
        }
        else
        {
            addrx += 4;         /* 偏移到下一个位置 */
        }
    }

    if (status == HAL_OK)
    {
        while (waddr < endaddr) /* 写数据 */
        {
            if (stmflash_write_8word(waddr, pbuf))  /* 写入数据 */
            {
                break;  /* 写入异常 */
            }

            waddr += 32;
            pbuf += 8;
        }
    }

    HAL_FLASH_Lock();           /* 上锁 */
}

/**
 * @brief       从指定地址开始读出指定长度的数据
 * @param       raddr : 起始地址
 * @param       pbuf  : 数据指针
 * @param       length: 要读取的字(32)数,即4个字节的整数倍
 * @retval      无
 */
void stmflash_read(uint32_t raddr, uint32_t *pbuf, uint32_t length)
{
    uint32_t i;

    for (i = 0; i < length; i++)
    {
        pbuf[i] = stmflash_read_word(raddr);    /* 读取4个字节. */
        raddr += 4; /* 偏移4个字节. */
    }
}

/******************************************************************************************/
/* 测试用代码 */

/**
 * @brief       测试写数据(写1个字)
 * @param       waddr : 起始地址
 * @param       wdata : 要写入的数据
 * @retval      读取到的数据
 */
void test_write(uint32_t waddr, uint32_t wdata)
{
    stmflash_write(waddr, &wdata, 1);   /* 写入一个字 */
}














