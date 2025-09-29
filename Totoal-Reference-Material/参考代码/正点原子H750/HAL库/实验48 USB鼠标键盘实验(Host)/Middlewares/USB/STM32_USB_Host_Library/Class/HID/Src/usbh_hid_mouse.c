/**
 ****************************************************************************************************
 * @file        usbh_hid_mouse.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-06
 * @brief       USB鼠标 驱动代码
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

#include "usbh_hid_mouse.h"
#include "usbh_hid_parser.h"



HID_MOUSE_Info_TypeDef    mouse_info;           /* 鼠标信息(坐标+按钮状态)*/
uint8_t mouse_report_data[HID_QUEUE_SIZE];      /* 鼠标上报数据长度,最多HID_QUEUE_SIZE个字节*/


/**
 * @brief       USBH 鼠标初始化
 * @param       phost       : USBH句柄指针
 * @retval      USB状态
 *   @arg       USBH_OK(0)   , 正常;
 *   @arg       USBH_BUSY(1) , 忙;
 *   @arg       其他         , 失败;
 */
USBH_StatusTypeDef USBH_HID_MouseInit(USBH_HandleTypeDef *phost)
{
    HID_HandleTypeDef *HID_Handle =  (HID_HandleTypeDef *) phost->pActiveClass->pData;
    mouse_info.x = 0;
    mouse_info.y = 0;
    mouse_info.button = 0;

    if (HID_Handle->length > sizeof(mouse_report_data))
    {
        HID_Handle->length = sizeof(mouse_report_data);
    }

    HID_Handle->pData = (uint8_t *)mouse_report_data;
    USBH_HID_FifoInit(&HID_Handle->fifo, phost->device.Data, HID_QUEUE_SIZE);
    return USBH_OK;
}

/**
 * @brief       USBH 获取鼠标信息
 * @param       phost       : USBH句柄指针
 * @retval      鼠标信息(HID_MOUSE_Info_TypeDef)
 */
HID_MOUSE_Info_TypeDef *USBH_HID_GetMouseInfo(USBH_HandleTypeDef *phost)
{
    if (USBH_HID_MouseDecode(phost) == USBH_OK)
    {
        return &mouse_info;
    }
    else
    {
        return NULL;
    }
}

/**
 * @brief       USBH 鼠标数据解析函数
 * @param       phost       : USBH句柄指针
 * @retval      USB状态
 *   @arg       USBH_OK(0)   , 正常;
 *   @arg       USBH_BUSY(1) , 忙;
 *   @arg       其他         , 失败;
 */
USBH_StatusTypeDef USBH_HID_MouseDecode(USBH_HandleTypeDef *phost)
{
    HID_HandleTypeDef *HID_Handle = (HID_HandleTypeDef *) phost->pActiveClass->pData;

    if (HID_Handle->length == 0)return USBH_FAIL;

    if (USBH_HID_FifoRead(&HID_Handle->fifo, &mouse_report_data, HID_Handle->length) == HID_Handle->length) /* 读取FIFO*/
    {
        mouse_info.button = mouse_report_data[0];
        mouse_info.x      = mouse_report_data[1];
        mouse_info.y      = mouse_report_data[2];
        mouse_info.z      = mouse_report_data[3];
        return USBH_OK;
    }

    return   USBH_FAIL;
}

























