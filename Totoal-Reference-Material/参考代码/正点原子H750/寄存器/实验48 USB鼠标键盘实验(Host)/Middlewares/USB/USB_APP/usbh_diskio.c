/**
 ****************************************************************************************************
 * @file        usbh_diskio.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-06
 * @brief       usbh diskio 驱动代码
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

#include "usbh_diskio.h"
#include "./FATFS/source/ff.h"
#include "./SYSTEM/usart/usart.h"


extern USBH_HandleTypeDef  hUSBHost;


/**
 * @brief       初始化 USBH
 * @param       无
 * @retval      无
 */
DSTATUS USBH_initialize(void)
{
    return RES_OK;
}

/**
 * @brief       获取U盘状态
 * @param       无
 * @retval      无
 */
DSTATUS USBH_status(void)
{
    DRESULT res=RES_ERROR;
    MSC_HandleTypeDef *MSC_Handle=hUSBHost.pActiveClass->pData;
    if(USBH_MSC_UnitIsReady(&hUSBHost,MSC_Handle->current_lun))
    {
        printf("U盘状态查询成功\r\n");
        res=RES_OK;
    }else
    {
        printf("U盘状态查询失败\r\n");
        res=RES_ERROR;
    }
    return res;
}

/**
 * @brief       U盘读扇区操作
 * @param       buff   : 数据缓冲首地址
 * @param       sector : 扇区地址
 * @param       count  : 需要读取的扇区数
 * @retval      执行结果(详见DRESULT定义)
 */
DRESULT USBH_read(BYTE *buff, DWORD sector, UINT count)
{
    DRESULT res = RES_ERROR;
    MSC_LUNTypeDef info;
    MSC_HandleTypeDef *MSC_Handle=hUSBHost.pActiveClass->pData;
    
    if(USBH_MSC_Read(&hUSBHost,MSC_Handle->current_lun,sector,buff,count)==USBH_OK)
    {
        res = RES_OK;
    }else
    {
        printf("U盘读取失败\r\n");
        USBH_MSC_GetLUNInfo(&hUSBHost,MSC_Handle->current_lun, &info);
        switch (info.sense.asc)
        {
            case SCSI_ASC_LOGICAL_UNIT_NOT_READY:
            case SCSI_ASC_MEDIUM_NOT_PRESENT:
            case SCSI_ASC_NOT_READY_TO_READY_CHANGE:
                USBH_ErrLog("USB Disk is not ready!");
                res = RES_NOTRDY;
                break;
            default:
                res = RES_ERROR;
                break;
        }
    }
    return res;
}

/**
 * @brief       U盘写扇区操作
 * @param       buff   : 数据缓冲首地址
 * @param       sector : 扇区地址
 * @param       count  : 需要写入的扇区数
 * @retval      执行结果(详见DRESULT定义)
 */
DRESULT USBH_write(const BYTE *buff,DWORD sector,UINT count)
{
    DRESULT res = RES_ERROR;
    MSC_LUNTypeDef info;
    MSC_HandleTypeDef *MSC_Handle=hUSBHost.pActiveClass->pData;
    if(USBH_MSC_Write(&hUSBHost,MSC_Handle->current_lun,sector,(BYTE *)buff,count) == USBH_OK)
    {
        res=RES_OK;
    }else
    {
        printf("U盘写入失败\r\n");
        USBH_MSC_GetLUNInfo(&hUSBHost,MSC_Handle->current_lun, &info);
        switch (info.sense.asc)
        {
            case SCSI_ASC_WRITE_PROTECTED:
                USBH_ErrLog("USB Disk is Write protected!");
                res = RES_WRPRT;
                break;
            case SCSI_ASC_LOGICAL_UNIT_NOT_READY:
            case SCSI_ASC_MEDIUM_NOT_PRESENT:
            case SCSI_ASC_NOT_READY_TO_READY_CHANGE:
                USBH_ErrLog("USB Disk is not ready!");
                res = RES_NOTRDY;
                break;
            default:
                res = RES_ERROR;
                break;
        }
    }
    return res;
}

/**
 * @brief       U盘IO控制操作
 * @param       cmd    : 控制命令
 * @param       buff   : 控制数据
 * @retval      执行结果(详见DRESULT定义)
 */
DRESULT USBH_ioctl(BYTE cmd,void *buff)
{
    DRESULT res = RES_ERROR;
    MSC_LUNTypeDef info;
    MSC_HandleTypeDef *MSC_Handle=hUSBHost.pActiveClass->pData;
    switch(cmd)
    {
        case CTRL_SYNC:
            res=RES_OK;
            break;
        case GET_SECTOR_COUNT : //获取扇区数量
            if(USBH_MSC_GetLUNInfo(&hUSBHost,MSC_Handle->current_lun,&info)==USBH_OK)
            {
                *(DWORD*)buff=info.capacity.block_nbr;
                res = RES_OK;
                printf("扇区数量:%d\r\n",info.capacity.block_nbr);
            }else
            {
                res = RES_ERROR;
            }
            break;
        case GET_SECTOR_SIZE ://获取扇区大小
            if(USBH_MSC_GetLUNInfo(&hUSBHost,MSC_Handle->current_lun,&info) == USBH_OK)
            {
                *(DWORD*)buff=info.capacity.block_size;
                res = RES_OK;
                printf("扇区大小:%d\r\n",info.capacity.block_size);
            }else
            {
                res = RES_ERROR;
            }
            break;
        case GET_BLOCK_SIZE :   //获取一个扇区里面擦除块的大小，

            if(USBH_MSC_GetLUNInfo(&hUSBHost,MSC_Handle->current_lun,&info)==USBH_OK)
            {
                *(DWORD*)buff=info.capacity.block_size/USB_DEFAULT_BLOCK_SIZE;
                printf("每个扇区擦除块:%d\r\n",info.capacity.block_size/USB_DEFAULT_BLOCK_SIZE);
                res = RES_OK;
            }else
            {
                res = RES_ERROR;
            }
            break;
        default:
                res = RES_PARERR;
    }
    return res;
}















