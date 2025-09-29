/**
 ****************************************************************************************************
 * @file        norflash.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-03-29
 * @brief       NOR FLASH(25QXX) 驱动代码
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
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/NORFLASH/norflash.h"


uint16_t g_norflash_type = W25Q128;     /* 默认是W25Q128 */

/* SPI FLASH 地址位宽 */
volatile uint8_t g_norflash_addrw = 2;  /* SPI FLASH地址位宽, 在norflash_read_id函数里面被修改
                                         * 2, 表示24bit地址宽度
                                         * 3, 表示32bit地址宽度
                                         */

/**
 * @brief       初始化SPI NOR FLASH
 * @param       无
 * @retval      无
 */
void norflash_init(void)
{
    uint8_t temp;
    qspi_init();                /* 初始化QSPI */
    norflash_qspi_disable();    /* 退出QPI模式(避免芯片之前进入这个模式,导致下载失败) */
    norflash_qe_enable();       /* 使能QE位 */
    g_norflash_type = norflash_read_id();/* 读取FLASH ID. */

    if (g_norflash_type == W25Q256)   /* SPI FLASH为W25Q256, 必须使能4字节地址模式 */
    {
        temp = norflash_read_sr(3); /* 读取状态寄存器3，判断地址模式 */

        if ((temp & 0X01) == 0)     /* 如果不是4字节地址模式,则进入4字节地址模式 */
        {
            norflash_write_enable();/* 写使能 */
            temp |= 1 << 1;         /* ADP=1, 上电4位地址模式 */
            norflash_write_sr(3, temp);  /* 写SR3 */
            
             norflash_write_enable();/* 写使能 */
             /* QPI,使能4字节地址指令,地址为0,无数据_8位地址_无地址_单线传输指令,无空周期,0个字节数据 */
            qspi_send_cmd(FLASH_Enable4ByteAddr, 0, (0 << 6) | (0 << 4) | (0 << 2) | (1 << 0), 0); 
            
        }
    }

    //printf("ID:%x\r\n", g_norflash_type);
}

/**
 * @brief       等待空闲
 * @param       无
 * @retval      无
 */
static void norflash_wait_busy(void)
{
    while ((norflash_read_sr(1) & 0x01) == 0x01);   /*  等待BUSY位清空 */
}

/**
 * @brief       退出QSPI模式
 * @param       无
 * @retval      无
 */
static void norflash_qspi_disable(void)
{
    /* 写command指令,地址为0,无数据_8位地址_无地址_4线传输指令,无空周期,0个字节数据 */
    qspi_send_cmd(FLASH_ExitQPIMode, 0, (0 << 6) | (0 << 4) | (0 << 2) | (3 << 0), 0);
}

/**
 * @brief       使能FLASH QE位，使能IO2/IO3
 * @param       无
 * @retval      无
 */
static void norflash_qe_enable(void)
{
    uint8_t stareg2 = 0;
    stareg2 = norflash_read_sr(2);  /* 先读出状态寄存器2的原始值 */

    //printf("stareg2:%x\r\n", stareg2);
    if ((stareg2 & 0X02) == 0)      /* QE位未使能 */
    {
        norflash_write_enable();    /* 写使能 */
        stareg2 |= 1 << 1;          /* 使能QE位 */
        norflash_write_sr(2, stareg2);  /* 写状态寄存器2 */
    }
}

/**
 * @brief       25QXX写使能
 *   @note      将S1寄存器的WEL置位
 * @param       无
 * @retval      无
 */
void norflash_write_enable(void)
{
    /* SPI,写使能指令,地址为0,无数据_8位地址_无地址_单线传输指令,无空周期,0个字节数据 */
    qspi_send_cmd(FLASH_WriteEnable, 0, (0 << 6) | (0 << 4) | (0 << 2) | (1 << 0), 0);
}

/**
 * @brief       25QXX写禁止
 *   @note      将S1寄存器的WEL清零
 * @param       无
 * @retval      无
 */
void norflash_write_disable(void)
{
    /* SPI,写禁止指令,地址为0,无数据_8位地址_无地址_单线传输指令,无空周期,0个字节数据 */
    qspi_send_cmd(FLASH_WriteDisable, 0, (0 << 6) | (0 << 4) | (0 << 2) | (1 << 0), 0);
}

/**
 * @brief       读取25QXX的状态寄存器，25QXX一共有3个状态寄存器
 *   @note      状态寄存器1：
 *              BIT7  6   5   4   3   2   1   0
 *              SPR   RV  TB BP2 BP1 BP0 WEL BUSY
 *              SPR:默认0,状态寄存器保护位,配合WP使用
 *              TB,BP2,BP1,BP0:FLASH区域写保护设置
 *              WEL:写使能锁定
 *              BUSY:忙标记位(1,忙;0,空闲)
 *              默认:0x00
 *
 *              状态寄存器2：
 *              BIT7  6   5   4   3   2   1   0
 *              SUS   CMP LB3 LB2 LB1 (R) QE  SRP1
 *
 *              状态寄存器3：
 *              BIT7      6    5    4   3   2   1   0
 *              HOLD/RST  DRV1 DRV0 (R) (R) WPS ADP ADS
 *
 * @param       regno: 状态寄存器号，范围:1~3
 * @retval      状态寄存器值
 */
uint8_t norflash_read_sr(uint8_t regno)
{
    uint8_t byte = 0, command = 0;

    switch (regno)
    {
        case 1:
            command = FLASH_ReadStatusReg1;  /* 读状态寄存器1指令 */
            break;

        case 2:
            command = FLASH_ReadStatusReg2;  /* 读状态寄存器2指令 */
            break;

        case 3:
            command = FLASH_ReadStatusReg3;  /* 读状态寄存器3指令 */
            break;

        default:
            command = FLASH_ReadStatusReg1;
            break;
    }

    /* SPI,写command指令,地址为0,单线传数据_8位地址_无地址_单线传输指令,无空周期,1个字节数据 */
    qspi_send_cmd(command, 0, (1 << 6) | (0 << 4) | (0 << 2) | (1 << 0), 0);
    qspi_receive(&byte, 1);
    return byte;
}

/**
 * @brief       写25QXX状态寄存器
 *   @note      寄存器说明见norflash_read_sr函数说明
 * @param       regno: 状态寄存器号，范围:1~3
 * @param       sr   : 要写入状态寄存器的值
 * @retval      无
 */
void norflash_write_sr(uint8_t regno, uint8_t sr)
{
    uint8_t command = 0;

    switch (regno)
    {
        case 1:
            command = FLASH_WriteStatusReg1;  /* 写状态寄存器1指令 */
            break;

        case 2:
            command = FLASH_WriteStatusReg2;  /* 写状态寄存器2指令 */
            break;

        case 3:
            command = FLASH_WriteStatusReg3;  /* 写状态寄存器3指令 */
            break;

        default:
            command = FLASH_WriteStatusReg1;
            break;
    }

    /* SPI,写command指令,地址为0,单线传数据_8位地址_无地址_单线传输指令,无空周期,1个字节数据 */
    qspi_send_cmd(command, 0, (1 << 6) | (0 << 4) | (0 << 2) | (1 << 0), 0);
    qspi_transmit(&sr, 1);
}

/**
 * @brief       读取芯片ID
 * @param       无
 * @retval      FLASH芯片ID
 *   @note      芯片ID列表见: norflash.h, 芯片列表部分
 */
uint16_t norflash_read_id(void)
{
    uint8_t temp[2];
    uint16_t deviceid;
    qspi_init();          /* 进行库函数调用前要先初始化 */
    /* SPI,读id,地址为0,单线传输数据_24位地址_单线传输地址_单线传输指令,无空周期,2个字节数据 */
    qspi_send_cmd(FLASH_ManufactDeviceID, 0, (1 << 6) | (2 << 4) | (1 << 2) | (1 << 0), 0);
    qspi_receive(temp, 2);
    deviceid = (temp[0] << 8) | temp[1];

    if (deviceid == W25Q256)
    {
        g_norflash_addrw = 3;   /* 如果是W25Q256, 标记32bit地址宽度 */
    }

    return deviceid;
}

/**
 * @brief       读取SPI FLASH,仅支持QSPI模式
 *   @note      在指定地址开始读取指定长度的数据
 * @param       pbuf    : 数据存储区
 * @param       addr    : 开始读取的地址(最大32bit)
 * @param       datalen : 要读取的字节数(最大65535)
 * @retval      无
 */
void norflash_read(uint8_t *pbuf, uint32_t addr, uint16_t datalen)
{
    /* QSPI,快速读数据,地址为addr,4线传输数据_24/32位地址_4线传输地址_1线传输指令,6空周期,datalen个数据 */
    qspi_send_cmd(FLASH_FastReadQuad, addr, (3 << 6) | (g_norflash_addrw << 4) | (3 << 2) | (1 << 0), 6);
    qspi_receive(pbuf, datalen);
}

/**
 * @brief       SPI在一页(0~65535)内写入少于256个字节的数据
 *   @note      在指定地址开始写入最大256字节的数据
 * @param       pbuf    : 数据存储区
 * @param       addr    : 开始写入的地址(最大32bit)
 * @param       datalen : 要写入的字节数(最大256),该数不应该超过该页的剩余字节数!!!
 * @retval      无
 */
static void norflash_write_page(uint8_t *pbuf, uint32_t addr, uint16_t datalen)
{
    norflash_write_enable();        /* 写使能 */

    /* QSPI,页写指令,地址为addr,4线传输数据_24/32位地址_1线传输地址_1线传输指令,无空周期,datalen个数据 */
    qspi_send_cmd(FLASH_PageProgramQuad, addr, (3 << 6) | (g_norflash_addrw << 4) | (1 << 2) | (1 << 0), 0);

    qspi_transmit(pbuf, datalen);   /* 发送数据 */
    norflash_wait_busy();           /* 等待写入结束 */
}

/**
 * @brief       无检验写SPI FLASH
 *   @note      必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
 *              具有自动换页功能
 *              在指定地址开始写入指定长度的数据,但是要确保地址不越界!
 *
 * @param       pbuf    : 数据存储区
 * @param       addr    : 开始写入的地址(最大32bit)
 * @param       datalen : 要写入的字节数(最大65535)
 * @retval      无
 */
static void norflash_write_nocheck(uint8_t *pbuf, uint32_t addr, uint16_t datalen)
{
    uint16_t pageremain;
    pageremain = 256 - addr % 256;  /* 单页剩余的字节数 */

    if (datalen <= pageremain)      /* 不大于256个字节 */
    {
        pageremain = datalen;
    }

    while (1)
    {
        /* 当写入字节比页内剩余地址还少的时候, 一次性写完
         * 当写入直接比页内剩余地址还多的时候, 先写完整个页内剩余地址, 然后根据剩余长度进行不同处理
         */
        norflash_write_page(pbuf, addr, pageremain);

        if (datalen == pageremain)   /* 写入结束了 */
        {
            break;
        }
        else     /* datalen > pageremain */
        {
            pbuf += pageremain;         /* pbuf指针地址偏移,前面已经写了pageremain字节 */
            addr += pageremain;         /* 写地址偏移,前面已经写了pageremain字节 */
            datalen -= pageremain;      /* 写入总长度减去已经写入了的字节数 */

            if (datalen > 256)          /* 剩余数据还大于一页,可以一次写一页 */
            {
                pageremain = 256;       /* 一次可以写入256个字节 */
            }
            else     /* 剩余数据小于一页,可以一次写完 */
            {
                pageremain = datalen;   /* 不够256个字节了 */
            }
        }
    }
}

/**
 * @brief       写SPI FLASH
 *   @note      在指定地址开始写入指定长度的数据 , 该函数带擦除操作!
 *              SPI FLASH 一般是: 256个字节为一个Page, 4Kbytes为一个Sector, 16个扇区为1个Block
 *              擦除的最小单位为Sector.
 *
 * @param       pbuf    : 数据存储区
 * @param       addr    : 开始写入的地址(最大32bit)
 * @param       datalen : 要写入的字节数(最大65535)
 * @retval      无
 */
uint8_t g_norflash_buf[4096];   /* 扇区缓存 */

void norflash_write(uint8_t *pbuf, uint32_t addr, uint16_t datalen)
{
    uint32_t secpos;
    uint16_t secoff;
    uint16_t secremain;
    uint16_t i;
    uint8_t *norflash_buf;

    norflash_buf = g_norflash_buf;
    secpos = addr / 4096;       /* 扇区地址 */
    secoff = addr % 4096;       /* 在扇区内的偏移 */
    secremain = 4096 - secoff;  /* 扇区剩余空间大小 */

    //printf("ad:%X,nb:%X\r\n", addr, datalen); /* 测试用 */
    if (datalen <= secremain)
    {
        secremain = datalen;    /* 不大于4096个字节 */
    }

    while (1)
    {
        norflash_read(norflash_buf, secpos * 4096, 4096);   /* 读出整个扇区的内容 */

        for (i = 0; i < secremain; i++)   /* 校验数据 */
        {
            if (norflash_buf[secoff + i] != 0XFF)
            {
                break;      /* 需要擦除, 直接退出for循环 */
            }
        }

        if (i < secremain)   /* 需要擦除 */
        {
            norflash_erase_sector(secpos);  /* 擦除这个扇区 */

            for (i = 0; i < secremain; i++)   /* 复制 */
            {
                norflash_buf[i + secoff] = pbuf[i];
            }

            norflash_write_nocheck(norflash_buf, secpos * 4096, 4096);  /* 写入整个扇区 */
        }
        else        /* 写已经擦除了的,直接写入扇区剩余区间. */
        {
            norflash_write_nocheck(pbuf, addr, secremain);  /* 直接写扇区 */
        }

        if (datalen == secremain)
        {
            break;  /* 写入结束了 */
        }
        else        /* 写入未结束 */
        {
            secpos++;               /* 扇区地址增1 */
            secoff = 0;             /* 偏移位置为0 */

            pbuf += secremain;      /* 指针偏移 */
            addr += secremain;      /* 写地址偏移 */
            datalen -= secremain;   /* 字节数递减 */

            if (datalen > 4096)
            {
                secremain = 4096;   /* 下一个扇区还是写不完 */
            }
            else
            {
                secremain = datalen;/* 下一个扇区可以写完了 */
            }
        }
    }
}

/**
 * @brief       擦除整个芯片
 *   @note      等待时间超长...
 * @param       无
 * @retval      无
 */
void norflash_erase_chip(void)
{
    norflash_write_enable();    /* 写使能 */
    norflash_wait_busy();       /* 等待空闲 */
    /* QPI,写全片擦除指令,地址为0,无数据_8位地址_无地址_1线传输指令,无空周期,0个字节数据 */
    qspi_send_cmd(FLASH_ChipErase, 0, (0 << 6) | (0 << 4) | (0 << 2) | (1 << 0), 0);
    norflash_wait_busy();       /* 等待芯片擦除结束 */
}

/**
 * @brief       擦除一个扇区
 *   @note      注意,这里是扇区地址,不是字节地址!!
 *              擦除一个扇区的最少时间:150ms
 *
 * @param       saddr : 扇区地址 根据实际容量设置
 * @retval      无
 */
void norflash_erase_sector(uint32_t saddr)
{
    //printf("fe:%x\r\n", saddr);   /* 监视falsh擦除情况,测试用 */
    saddr *= 4096;
    norflash_write_enable();        /* 写使能 */
    norflash_wait_busy();           /* 等待空闲 */

    /* QPI,写扇区擦除指令,地址为0,无数据_24/32位地址_1线传输地址_1线传输指令,无空周期,0个字节数据 */
    qspi_send_cmd(FLASH_SectorErase, saddr, (0 << 6) | (g_norflash_addrw << 4) | (1 << 2) | (1 << 0), 0);

    norflash_wait_busy();           /* 等待擦除完成 */
}

























