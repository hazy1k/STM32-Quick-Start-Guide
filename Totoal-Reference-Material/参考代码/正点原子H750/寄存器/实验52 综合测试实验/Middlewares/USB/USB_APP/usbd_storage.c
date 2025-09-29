/**
 ****************************************************************************************************
 * @file        usbd_storage.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-06
 * @brief       usbd_storage 驱动代码
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
 * V1.0 20200406
 * 第一次发布
 *
 ****************************************************************************************************
 */

#include "usbd_storage.h"
#include "./BSP/SDMMC/sdmmc_sdcard.h"
#include "./BSP/NORFLASH/norflash_ex.h"
#include "usb_app.h"


/* 文件系统在外部FLASH的起始地址
 * 2240 * 4096是怎么来的, 请看FATFS实验, diskio.c的说明
 */
#define USB_STORAGE_FLASH_BASE  2240*4096


/* 自己定义的一个标记USB状态的寄存器, 方便判断USB状态
 * bit0 : 表示电脑正在向SD卡写入数据
 * bit1 : 表示电脑正从SD卡读出数据
 * bit2 : SD卡写数据错误标志位
 * bit3 : SD卡读数据错误标志位
 * bit4 : 1,表示电脑有轮询操作(表明连接还保持着)
 */
//volatile uint8_t g_usb_state_reg = 0;



/* USB Mass storage 标准查询数据(每个lun占36字节) */
const int8_t  STORAGE_Inquirydata[] =
{

    /* LUN 0 */
    0x00,
    0x80,
    0x02,
    0x02,
    (STANDARD_INQUIRY_DATA_LEN - 4),
    0x00,
    0x00,
    0x00,
    /* Vendor Identification */
    'A', 'L', 'I', 'E', 'N', 'T', 'E', 'K', ' ',/* 9字节 */
    /* Product Identification */
    'S', 'P', 'I', ' ', 'F', 'l', 'a', 's', 'h',/* 15字节 */
    ' ', 'D', 'i', 's', 'k', ' ',
    /* Product Revision Level */
    '1', '.', '0', ' ',							/* 4字节 */

    /* LUN 1 */
    0x00,
    0x80,
    0x02,
    0x02,
    (STANDARD_INQUIRY_DATA_LEN - 4),
    0x00,
    0x00,
    0x00,
    /* Vendor Identification */
    'A', 'L', 'I', 'E', 'N', 'T', 'E', 'K', ' ',	/* 9字节 */
    /* Product Identification */
    'S', 'D', ' ', 'F', 'l', 'a', 's', 'h', ' ',/* 15字节 */
    'D', 'i', 's', 'k', ' ', ' ',
    /* Product Revision Level */
    '1', '.', '0', ' ',                      	/* 4字节 */
};

/* Private function prototypes ----------------------------------------------- */
int8_t STORAGE_Init(uint8_t lun);
int8_t STORAGE_GetCapacity(uint8_t lun, uint32_t *block_num, uint16_t *block_size);
int8_t STORAGE_IsReady(uint8_t lun);
int8_t STORAGE_IsWriteProtected(uint8_t lun);
int8_t STORAGE_Read(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);
int8_t STORAGE_Write(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);
int8_t STORAGE_GetMaxLun(void);

USBD_StorageTypeDef USBD_DISK_fops =
{
    STORAGE_Init,
    STORAGE_GetCapacity,
    STORAGE_IsReady,
    STORAGE_IsWriteProtected,
    STORAGE_Read,
    STORAGE_Write,
    STORAGE_GetMaxLun,
    (int8_t *)STORAGE_Inquirydata,
};

/**
 * @brief       初始化存储设备
 * @param       lun        : 逻辑单元编号
 *   @arg                  0, SD卡
 *   @arg                  1, SPI FLASH
 * @retval      操作结果
 *   @arg       0    , 成功
 *   @arg       其他 , 错误代码
 */
int8_t STORAGE_Init (uint8_t lun)
{
    uint8_t res = 0;

    switch (lun)
    {
        case 0: /* SPI FLASH */
            /* 没有初始化函数！不能调用：NORFLASH_Init！调用该函数将导致QSPI内存映射模式失效,程序运行可能异常
             * 因为在sys.c里面,开启内存映射的时候，已经初始化QSPI接口了 
             */
            break;

        case 1: /* SD卡 */
            res = sd_init();
            break;
    }

    return res;
}

/**
 * @brief       获取存储设备的容量和块大小
 * @param       lun        : 逻辑单元编号
 *   @arg                  0, SD卡
 *   @arg                  1, SPI FLASH
 * @param       block_num  : 块数量(扇区数)
 * @param       block_size : 块大小(扇区大小)
 * @retval      操作结果
 *   @arg       0    , 成功
 *   @arg       其他 , 错误代码
 */
int8_t STORAGE_GetCapacity (uint8_t lun, uint32_t *block_num, uint16_t *block_size)
{
    switch (lun)
    {
        case 0: /* SPI FLASH */
            *block_size = 512;
            *block_num = (1024 * 7424) / 512;   /* SPI FLASH的7424KB字节,文件系统用 */
            break;

        case 1: /* SD卡 */
            *block_size = 512;
            *block_num = g_sd_card_info.CardCapacity / 512;
            break;
    }

    return 0;
}

/**
 * @brief       查看存储设备是否就绪
 * @param       lun        : 逻辑单元编号
 *   @arg                  0, SD卡
 *   @arg                  1, SPI FLASH
 * @retval      就绪状态
 *   @arg       0    , 就绪
 *   @arg       其他 , 未就绪
 */
int8_t  STORAGE_IsReady (uint8_t lun)
{
    usbx.bDeviceState |= 0X10;  /* 标记轮询 */
    return 0;
}

/**
 * @brief       查看存储设备是否写保护
 * @param       lun        : 逻辑单元编号
 *   @arg                  0, SD卡
 *   @arg                  1, SPI FLASH
 * @retval      读保护状态
 *   @arg       0    , 没有读保护
 *   @arg       其他 , 有读保护
 */
int8_t  STORAGE_IsWriteProtected (uint8_t lun)
{
    return  0;
}

/**
 * @brief       从存储设备读取数据
 * @param       lun        : 逻辑单元编号
 *   @arg                  0, SD卡
 *   @arg                  1, SPI FLASH
 * @param       buf        : 数据存储区首地址指针
 * @param       blk_addr   : 要读取的地址(扇区地址)
 * @param       blk_len    : 要读取的块数(扇区数)
 * @retval      操作结果
 *   @arg       0    , 成功
 *   @arg       其他 , 错误代码
 */
int8_t STORAGE_Read (uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len)
{
    int8_t res = 0;
    usbx.bDeviceState |= 0X02;  /* 标记正在读数据 */

    switch (lun)
    {
        case 0: /* SPI FLASH */
            norflash_ex_read(buf, USB_STORAGE_FLASH_BASE + blk_addr * 512, blk_len * 512);
            break;

        case 1: /* SD卡 */
            res = sd_read_disk(buf, blk_addr, blk_len);
            break;
    }

    if (res)
    {
        printf("rerr:%d,%d", lun, res);
        usbx.bDeviceState |= 0X08;  /* 读错误! */
    }

    return res;
}

/**
 * @brief       向存储设备写数据
 * @param       lun        : 逻辑单元编号
 *   @arg                  0, SD卡
 *   @arg                  1, SPI FLASH
 * @param       buf        : 数据存储区首地址指针
 * @param       blk_addr   : 要写入的地址(扇区地址)
 * @param       blk_len    : 要写入的块数(扇区数)
 * @retval      操作结果
 *   @arg       0    , 成功
 *   @arg       其他 , 错误代码
 */
int8_t STORAGE_Write (uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len)
{
    int8_t res = 0;
    usbx.bDeviceState |= 0X01;      /* 标记正在写数据 */

    switch (lun)
    {
        case 0: /* SPI FLASH */
            norflash_ex_write(buf, USB_STORAGE_FLASH_BASE + blk_addr * 512, blk_len * 512);
            break;

        case 1: /* SD卡 */
            res = sd_write_disk(buf, blk_addr, blk_len);
            break;
    }

    if (res)
    {
        usbx.bDeviceState |= 0X04;  /* 写错误! */
        printf("werr:%d,%d", lun, res);
    }

    return res;
}

/**
 * @brief       获取支持的最大逻辑单元个数
 *   @note      注意, 这里返回的逻辑单元个数是减去了1的.
 *              0, 就表示1个; 1, 表示2个; 以此类推
 * @param       无
 * @retval      支持的逻辑单元个数 - 1
 */
int8_t STORAGE_GetMaxLun (void)
{
    /* STORAGE_LUN_NBR 在usbd_conf.h里面定义, 默认是2 */
    if (g_sd_card_info.CardCapacity)    /* 如果SD卡正常, 则支持2个磁盘 */
    {
        return STORAGE_LUN_NBR - 1;
    }
    else    /* SD卡不正常, 则只支持1个磁盘 */
    {
        return STORAGE_LUN_NBR - 2;
    }
}






















