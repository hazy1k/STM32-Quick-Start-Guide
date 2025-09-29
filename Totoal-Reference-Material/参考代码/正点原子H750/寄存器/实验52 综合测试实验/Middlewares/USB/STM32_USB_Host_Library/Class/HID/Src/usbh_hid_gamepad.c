/**
 ****************************************************************************************************
 * @file        usbh_hid_gamepad.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-06-01
 * @brief       USB游戏手柄 驱动代码
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

#include "usbh_hid_gamepad.h"
#include "usbh_hid_parser.h"
#include "./SYSTEM/usart/usart.h"


/**
 * FC游戏手柄模拟
 * 读取fcpad.ctrlval,即可得到当前手柄格式转换成FC手柄格式.
 */
FC_GamePad_TypeDef fcpad;
FC_GamePad_TypeDef fcpad1;


/* 综合实验时, 需要至少2字节对齐, 否则, 如果数组地址是奇数, 接入手柄在-O2优化的时候就会死机 */
#if !(__ARMCC_VERSION >= 6010050)   /* 不是AC6编译器，即使用AC5编译器时 */
#define __ALIGNED_4     __align(4)  /* AC5使用这个 */
#else                               /* 使用AC6编译器时 */
#define __ALIGNED_4     __ALIGNED(4) /* AC6使用这个 */
#endif

__ALIGNED_4 uint8_t gamepad_report_data[HID_QUEUE_SIZE];    /* 鼠标上报数据长度,最多HID_QUEUE_SIZE个字节 */


/**
 * @brief       USBH 游戏手柄初始化
 * @param       phost       : USBH句柄指针
 * @retval      USB状态
 *   @arg       USBH_OK(0)   , 正常;
 *   @arg       USBH_BUSY(1) , 忙;
 *   @arg       其他         , 失败;
 */
USBH_StatusTypeDef USBH_HID_GamePadInit(USBH_HandleTypeDef *phost)
{
    HID_HandleTypeDef *HID_Handle =  (HID_HandleTypeDef *) phost->pActiveClass->pData;

    if (HID_Handle->length > sizeof(gamepad_report_data))
    {
        HID_Handle->length = sizeof(gamepad_report_data);
    }

    HID_Handle->pData = (uint8_t *)gamepad_report_data;
    USBH_HID_FifoInit(&HID_Handle->fifo, phost->device.Data, HID_QUEUE_SIZE);
    return USBH_OK;
}

/**
 * @brief       USBH 获取手柄信息
 * @param       phost       : USBH句柄指针
 * @retval      手柄信息(FC_GamePad_TypeDef)
 */
FC_GamePad_TypeDef *USBH_HID_GetGamePadInfo(USBH_HandleTypeDef *phost)
{
    uint8_t len = 0;
    HID_HandleTypeDef *HID_Handle = (HID_HandleTypeDef *) phost->pActiveClass->pData;

    if (HID_Handle->length == 0)return NULL;

    len = USBH_HID_FifoRead(&HID_Handle->fifo, &gamepad_report_data, HID_Handle->length);

    if (len == HID_Handle->length)  /* 读取FIFO */
    {
        USBH_HID_GamePad_Decode(gamepad_report_data);
        return &fcpad;
    }
    else
    {
        return NULL;
    }
}

/**
 * @brief       通用USB方向键解码,结果存放在fcpad里面
 * @param       data0,data1 : USB得到的方向数据
 *  @note                     data0:00,左键按下;ff,右键按下,7F,没有按键按下
 *                            data1:00,左键按下;ff,右键按下,7F,没有按键按下
 * @retval      无
 */
void USBH_HID_GamePad_nDir_Decode(uint8_t data0, uint8_t data1)
{
    switch (data0)
    {
        case 0X7F:
            fcpad.b.left = 0;
            fcpad.b.right = 0;
            break;

        case 0X00:
            fcpad.b.left = 1;
            break;

        case 0XFF:
            fcpad.b.right = 1;
            break;
    }

    switch (data1)
    {
        case 0X7F:
            fcpad.b.up = 0;
            fcpad.b.down = 0;
            break;

        case 0X00:
            fcpad.b.up = 1;
            break;

        case 0XFF:
            fcpad.b.down = 1;
            break;
    }
}

/**
 * @brief       通用USB AB键解码,结果存放在fcpad里面
 * @param       data        : USB得到的1/2/3/4键数据
 *  @note                     数据格式如下:
 *                            data:最高4位有效
 *                            b4=1,1键按下(FC:B键连按)
 *                            b5=1,2键按下(FC:A键连按)
 *                            b6=1,3键按下(FC:A键)
 *                            b7=1,4键按下(FC:B键)
 * @retval      无
 */
void USBH_HID_GamePad_nAB_Decode(uint8_t data)
{
    if (data & 0X10)fcpad.b.b = !fcpad.b.b; /* B键取反 */
    else
    {
        if (data & 0X80)fcpad.b.b = 1;      /* B键按下 */
        else fcpad.b.b = 0;                 /* B键松开 */
    }

    if (data & 0X20)fcpad.b.a = !fcpad.b.a; /* A键取反 */
    else
    {
        if (data & 0X40)fcpad.b.a = 1;      /* A键按下 */
        else fcpad.b.a = 0;                 /* A键松开 */
    }
}

/**
 * @brief       通用USB 功能键解码,结果存放在fcpad里面
 * @param       data        : USB得到的Select/Start键数据
 *  @note                     数据格式如下:
 *                            data:b4,b5有效.
 *                            b4=1,Select键按下
 *                            b5=1,Start键按下
 * @retval      无
 */
void USBH_HID_GamePad_nFun_Decode(uint8_t data)
{
    if (data & 0X10)fcpad.b.select = 1; /* Select键按下 */
    else fcpad.b.select = 0;            /* Select键松开 */

    if (data & 0X20)fcpad.b.start = 1;  /* Start键按下 */
    else fcpad.b.start = 0;             /* Start键松开 */
}

/**
 * @brief       Game Pad 数据解析
 * @param       data        : 输入数据
 * @retval      无
 */
void USBH_HID_GamePad_Decode(uint8_t *data)
{
    //uint8_t i;
    if (data[2] == 0X7F)    /* 无摇杆手柄手柄,第三个字节为0X7F */
    {
        USBH_HID_GamePad_nDir_Decode(data[3], data[4]); /* 解码方向键 */
        USBH_HID_GamePad_nAB_Decode(data[5]);           /* 解码AB键 */
        USBH_HID_GamePad_nFun_Decode(data[6]);          /* 解码功能键 */
        //USR_GAMEPAD_ProcessData(fcpad.ctrlval);       /* 显示结果 */
    }
    else    /* 有摇杆的手柄 */
    {
        USBH_HID_GamePad_nDir_Decode(data[0], data[1]); /* 解码方向键 */
        USBH_HID_GamePad_nAB_Decode(data[5]);           /* 解码AB键 */
        USBH_HID_GamePad_nFun_Decode(data[6]);          /* 解码功能键 */
        //USR_GAMEPAD_ProcessData(fcpad.ctrlval);       /* 显示结果 */
    }

    //for(i=0;i<8;i++)printf("%02x ",data[i]);
    //printf("\r\n");
}

    
uint8_t *const JOYPAD_SYMBOL_TBL[8] =
{"Right", "Left", "Down", "Up", "Start", "Select", "A", "B"}; /* 手柄按键符号定义 */

/**
 * @brief       USB键盘数据处理
 * @param       data        : 输入数据
 * @retval      无
 */
void  USR_GAMEPAD_ProcessData(uint8_t data)
{
    uint8_t i;

    if (data)
    {
        for (i = 0; i < 8; i++)
        {
            if (data & (0X80 >> i))
            {
                printf("key:%s\r\n", (uint8_t *)JOYPAD_SYMBOL_TBL[i]);
            }
        }
    }
}



















