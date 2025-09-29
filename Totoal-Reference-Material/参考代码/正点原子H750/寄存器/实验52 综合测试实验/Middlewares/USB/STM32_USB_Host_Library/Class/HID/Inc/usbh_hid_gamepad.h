/**
 ****************************************************************************************************
 * @file        usbh_hid_gamepad.h
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

#ifndef __USBH_HID_GAMEPAD_H
#define __USBH_HID_GAMEPAD_H

#include "usbh_hid.h"


/**
 * FC游戏手柄数据格式定义
 * 1,表示没有按下,0表示按下
 */
typedef union _FC_GamePad_TypeDef
{
    uint8_t ctrlval;
    struct
    {
        uint8_t a: 1;       /* A键 */
        uint8_t b: 1;       /* B键 */
        uint8_t select: 1;  /* Select键 */
        uint8_t start: 1;   /* Start键 */
        uint8_t up: 1;      /* 上 */
        uint8_t down: 1;    /* 下 */
        uint8_t left: 1;    /* 左 */
        uint8_t right: 1;   /* 右 */
    } b;
} FC_GamePad_TypeDef ;

extern FC_GamePad_TypeDef fcpad;    /* fc游戏手柄1 */
extern FC_GamePad_TypeDef fcpad1;   /* fc游戏手柄2 */




USBH_StatusTypeDef USBH_HID_GamePadInit(USBH_HandleTypeDef *phost);
void USBH_HID_GamePad_Decode(uint8_t *data);
FC_GamePad_TypeDef *USBH_HID_GetGamePadInfo(USBH_HandleTypeDef *phost);


void USR_GAMEPAD_Init(void);
void USR_GAMEPAD_ProcessData(uint8_t data);



#endif






















