/**
 ****************************************************************************************************
 * @file        usb_app.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-06-01
 * @brief       USB-APP 代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 STM32开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.0 20220601
 * 第一次发布
 *
 ****************************************************************************************************
 */

#include "usb_app.h"
#include "./SYSTEM/delay/delay.h"
#include "./MALLOC/malloc.h"
#include "./FATFS/exfuns/exfuns.h"
#include "os.h"


USBH_HandleTypeDef  hUSBHost;       /* USB Host处理结构体 */
USBD_HandleTypeDef  hUSBDevice;     /* USB Device处理结构体 */

_usb_app usbx;                      /* USB APP控制器 */


/**
 * @brief       USB OTG 中断服务函数
 * @param       无
 * @retval      无
 */
void OTG_FS_IRQHandler(void)
{
    OSIntEnter();

    if (usbx.mode == 2) /* USB从机模式（USB读卡器） */
    {
        HAL_PCD_IRQHandler(&hpcd);
    }
    else                /* USB主机模式 */
    {
        HAL_HCD_IRQHandler(&hhcd);
    }

    OSIntExit();
}

/**
 * @brief       USB主机通信处理函数
 * @param       phost  : USBH句柄指针
 * @param       id     : USBH当前的用户状态
 * @retval      无
 */
void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id)
{
    uint8_t dev_type = 0XFF;    /* 设备类型,1,键盘;2,鼠标;其他,不支持的设备 */

    switch (id)
    {
        case HOST_USER_SELECT_CONFIGURATION:
            break;

        case HOST_USER_DISCONNECTION:
            break;

        case HOST_USER_CLASS_ACTIVE:        /* 能执行到这一步，说明USB HOST对插入设备已经识别成功 */
            usbx.bDeviceState |= 0X80;      /* 标记已连接 */

            if (usbx.mode == USBH_HID_MODE) /* HID模式下,可以接入:鼠标/键盘/手柄 */
            {
                dev_type = phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].bInterfaceProtocol;

                if (dev_type == HID_KEYBRD_BOOT_CODE)       /* 键盘设备 */
                {
                    usbx.hdevclass = 3;     /* 检测到键盘插入 */
                }
                else if (dev_type == HID_MOUSE_BOOT_CODE)   /* 鼠标设备 */
                {
                    usbx.hdevclass = 2;     /* 检测到鼠标插入 */
                }
                else if (dev_type == 0)     /* 手柄设备 */
                {
                    usbx.hdevclass = 4;     /* 检测到手柄插入 */
                }
                else usbx.hdevclass = 0;    /* 不支持的设备 */
            }
            else usbx.hdevclass = 1;        /* 检测到U盘插入 */

            break;

        case HOST_USER_CONNECTION:
            break;

        default:
            break;
    }
}

/**
 * @brief       USB键盘/鼠标演示demo数据处理
 * @param       phost  : USBH句柄指针
 * @retval      无
 */
void USB_Demo(USBH_HandleTypeDef *phost)
{
    if (usbx.hdevclass == 3)            /* 键盘设备 */
    {
        USBH_HID_GetKeybdInfo(phost);   /* 转换成ASCII码 */
    }
    else if (usbx.hdevclass == 4)       /* USB游戏手柄设备 */
    {
        USBH_HID_GetGamePadInfo(phost); /* 获取手柄信息 */
    }
}

/**
 * @brief       初始化USB
 * @param       无
 * @retval      无
 */
void usbapp_init(void)
{
    usbx.bDeviceState = 0;
    usbx.hdevclass = 0;
    usbx.mode = 0XFF;   /* 设置为一个非法的模式,必须先调用usbapp_mode_set设置模式,才能正常工作 */
}

/**
 * @brief       USB轮询函数
 *  @note       此函数必须周期性的被调用
 * @param       无
 * @retval      无
 */
void usbapp_pulling(void)
{
    switch (usbx.mode)
    {
        case USBH_MSC_MODE:     /* USB HOST U盘事务处理 */
            USBH_Process(&hUSBHost);

            if (hUSBHost.gState == HOST_ABORT_STATE)    /* 防止插入手柄/键盘,引起usbapp_pulling猛查询 */
            {
                usbx.bDeviceState = 0;
                usbx.hdevclass = 0;
            }

            break;

        case USBH_HID_MODE:     /* USB HOST HID事务处理 */
            USBH_Process(&hUSBHost);
            USB_Demo(&hUSBHost);
            break;

        case USBD_MSC_MODE:
            break;
    }
}

/**
 * @brief       USB结束当前工作模式
 * @param       无
 * @retval      无
 */
void usbapp_mode_stop(void)
{
    USBH_ClassTypeDef *pac =  hUSBHost.pActiveClass;

    switch (usbx.mode)
    {
        case USBH_MSC_MODE:
            if (pac)USBH_MSC_InterfaceDeInit(&hUSBHost);    /* 复位USB HOST MSC类 */

            USBH_DeInit(&hUSBHost);     /* 复位USB HOST */
            break;

        case USBH_HID_MODE:
            if (pac)USBH_HID_InterfaceDeInit(&hUSBHost);    /* 复位USB HOST HID类 */

            USBH_DeInit(&hUSBHost);     /* 复位USB HOST */
            break;

        case USBD_MSC_MODE:
            USBD_MSC_DeInit(&hUSBDevice, 0);    /* 复位USB SLAVE MSC类 */
            USBD_DeInit(&hUSBDevice);           /* 复位USB SLAVE */
            break;
    }

    RCC->AHB1RSTR |= 1 << 27;       /* USB2 OTG FS 复位 */
    delay_ms(5);
    RCC->AHB1RSTR &= ~(1 << 27);    /* 复位结束 */
    memset(&hUSBHost, 0, sizeof(USBH_HandleTypeDef));
    memset(&hUSBDevice, 0, sizeof(USBD_HandleTypeDef));
    usbx.mode = 0XFF;
    usbx.bDeviceState = 0;
    usbx.hdevclass = 0;
}

/**
 * @brief       设置USB工作模式
 * @param       mode   : 工作模式
 *                       0,USB HOST MSC模式(默认模式,接U盘)
 *                       1,USB HOST HID模式(驱动鼠标键盘等)
 *                       2,USB Device MSC模式(USB读卡器)
 * @retval      无
 */
void usbapp_mode_set(uint8_t mode)
{
    usbapp_mode_stop();/* 先停止当前USB工作模式 */
    usbx.mode = mode;

    switch (usbx.mode)
    {
        case USBH_MSC_MODE:
            USBH_Init(&hUSBHost, USBH_UserProcess, 0);      /* 初始化USB HOST */
            USBH_RegisterClass(&hUSBHost, USBH_MSC_CLASS);  /* 添加USB HOST MSC类 */
            USBH_Start(&hUSBHost);                          /* 开启USB HOST */
            break;

        case USBH_HID_MODE:
            USBH_Init(&hUSBHost, USBH_UserProcess, 0);      /* 初始化USB HOST */
            USBH_RegisterClass(&hUSBHost, USBH_HID_CLASS);  /* 添加USB HOST HID类 */
            USBH_Start(&hUSBHost);                          /* 开启USB HOST */
            break;

        case USBD_MSC_MODE:
            USBD_Init(&hUSBDevice, &MSC_Desc, 0);           /* 初始化USB SLAVE */
            USBD_RegisterClass(&hUSBDevice, USBD_MSC_CLASS);/* 添加USB SLAVE MSC类 */
            USBD_MSC_RegisterStorage(&hUSBDevice, &USBD_DISK_fops); /* 为USB SLAVE MSC类添加回调函数 */
            USBD_Start(&hUSBDevice);                        /* 开启USB SLAVE */
            break;
    }
}







