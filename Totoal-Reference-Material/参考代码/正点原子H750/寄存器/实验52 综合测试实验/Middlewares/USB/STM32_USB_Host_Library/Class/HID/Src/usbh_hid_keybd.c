/**
 ****************************************************************************************************
 * @file        usbh_hid_keybd.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-06-01
 * @brief       USB键盘 驱动代码 - 游戏模式
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

#include "usbh_hid_keybd.h"
#include "usbh_hid_parser.h"


HID_KEYBD_Info_TypeDef     keybd_info;

/* 综合实验时, 需要至少2字节对齐, 否则, 如果数组地址是奇数, 接入手柄在-O2优化的时候就会死机 */
#if !(__ARMCC_VERSION >= 6010050)   /* 不是AC6编译器，即使用AC5编译器时 */
#define __ALIGNED_4     __align(4)  /* AC5使用这个 */
#else                               /* 使用AC6编译器时 */
#define __ALIGNED_4     __ALIGNED(4) /* AC6使用这个 */
#endif

__ALIGNED_4 uint8_t keybd_report_data[HID_QUEUE_SIZE];


/**
 * @brief       USBH 键盘初始化
 * @param       phost       : USBH句柄指针
 * @retval      USB状态
 *   @arg       USBH_OK(0)   , 正常;
 *   @arg       USBH_BUSY(1) , 忙;
 *   @arg       其他         , 失败;
 */
USBH_StatusTypeDef USBH_HID_KeybdInit(USBH_HandleTypeDef *phost)
{
    uint32_t x;
    HID_HandleTypeDef *HID_Handle =  (HID_HandleTypeDef *) phost->pActiveClass->pData;

    for (x = 0; x < (sizeof(keybd_report_data) / sizeof(uint8_t)); x++)
    {
        keybd_report_data[x] = 0;
    }

    if (HID_Handle->length > (sizeof(keybd_report_data) / sizeof(uint8_t)))
    {
        HID_Handle->length = (sizeof(keybd_report_data) / sizeof(uint8_t));
    }

    HID_Handle->pData = (uint8_t *)keybd_report_data;
    USBH_HID_FifoInit(&HID_Handle->fifo, phost->device.Data, HID_QUEUE_SIZE);
    return USBH_OK;
}

/**
 * @brief       USBH 获取键盘信息
 * @param       phost       : USBH句柄指针
 * @retval      键盘信息(HID_KEYBD_Info_TypeDef), 恒定返回NULL, 本例程未用到
 */
HID_KEYBD_Info_TypeDef *USBH_HID_GetKeybdInfo(USBH_HandleTypeDef *phost)
{
    uint8_t len = 0;
    HID_HandleTypeDef *HID_Handle = (HID_HandleTypeDef *) phost->pActiveClass->pData;

    if (HID_Handle->length == 0)return NULL;

    len = USBH_HID_FifoRead(&HID_Handle->fifo, &keybd_report_data, HID_Handle->length);

    if (len == HID_Handle->length)  /* 读取FIFO */
    {
        KEYBRD_Decode(keybd_report_data);
    }

    return NULL;
}

/**
 * 数组元素对应关系:A  A连击  B  B连击 Select Start  上  下   左  右
 *                 |   |     |   |     |       |    |   |   |   |
 *           手柄1:K   I     J   U    空格    回车   W   S   A   D
 *                 |   |     |   |     |       |    |   |   |   |
 *           手柄2:3   6     2   5    空格    回车  ↑   ↓  ←  →
 */
const uint8_t KEYBD_FCPAD1_TABLE[10] = {0X0E, 0X0C, 0X0D, 0X18, 0X2C, 0X28, 0X1A, 0X16, 0X04, 0X07}; /* 模拟手柄1 */
const uint8_t KEYBD_FCPAD2_TABLE[10] = {0X5B, 0X5E, 0X5A, 0X5D, 0X2C, 0X28, 0X52, 0X51, 0X50, 0X4F}; /* 模拟手柄2 */

/**
 * @brief       键盘数据解码成FC手柄数据.
 * @param       fcbuf       : 键盘输入数据
 * @param       mode        : 模式
 *                            0,不更新键盘输入数据;
 *                            1,更新键盘输入数据;
 * @retval      无
 */
void KEYBRD_FCPAD_Decode(uint8_t *fcbuf, uint8_t mode)
{
    static uint8_t pbuf[6];     /* 保存最近一次fc手柄的值 */
    uint8_t j;
    uint8_t da1 = 0, db1 = 0;
    uint8_t da2 = 0, db2 = 0;

    if (mode)
    {
        for (j = 0; j < 6; j++)pbuf[j] = fcbuf[j]; /* 更新数据 */
    }

    for (j = 0; j < 6; j++) /* 先判断A连击键? */
    {
        if (KEYBD_FCPAD1_TABLE[1] == pbuf[j])
        {
            fcpad.b.a = !fcpad.b.a;     /* 手柄1 A键取反 */
            da1 = 1;        /* 标记手柄1 A连击有效 */
        }

        if (KEYBD_FCPAD2_TABLE[1] == pbuf[j])
        {
            fcpad1.b.a = !fcpad1.b.a;   /* 手柄2 A键取反 */
            da2 = 1;        /* 标记手柄2 A连击有效 */
        }

        if (KEYBD_FCPAD1_TABLE[3] == pbuf[j])
        {
            fcpad.b.b = !fcpad.b.b;     /* 手柄1 B键取反 */
            db1 = 1;        /* 标记手柄1 B连击有效 */
        }

        if (KEYBD_FCPAD2_TABLE[3] == pbuf[j])
        {
            fcpad1.b.b = !fcpad1.b.b;   /* 手柄2 B键取反 */
            db2 = 1;        /* 标记手柄2 B连击有效 */
        }
    }

    if (da1 == 0)fcpad.b.a = 0;

    if (da2 == 0)fcpad1.b.a = 0;

    if (db1 == 0)fcpad.b.b = 0;

    if (db2 == 0)fcpad1.b.b = 0;

    fcpad.b.select = 0;
    fcpad1.b.select = 0;
    fcpad.b.start = 0;
    fcpad1.b.start = 0;
    fcpad.b.up = 0;
    fcpad1.b.up = 0;
    fcpad.b.down = 0;
    fcpad1.b.down = 0;
    fcpad.b.left = 0;
    fcpad1.b.left = 0;
    fcpad.b.right = 0;
    fcpad1.b.right = 0;

    for (j = 0; j < 6; j++) /* 先判断A连击键? */
    {
        if (da1 == 0)   /* 手柄1,A连击没有按下 */
        {
            if (KEYBD_FCPAD1_TABLE[0] == pbuf[j])fcpad.b.a = 1;     /* 手柄1 A键按下 */
        }

        if (da2 == 0)   /* 手柄2,A连击没有按下 */
        {
            if (KEYBD_FCPAD2_TABLE[0] == pbuf[j])fcpad1.b.a = 1;    /* 手柄2 A键按下 */
        }

        if (db1 == 0)   /* 手柄1,B连击没有按下 */
        {
            if (KEYBD_FCPAD1_TABLE[2] == pbuf[j])fcpad.b.b = 1;     /* 手柄1 B键按下 */
        }

        if (db2 == 0)   /* 手柄2,B连击没有按下 */
        {
            if (KEYBD_FCPAD2_TABLE[2] == pbuf[j])fcpad1.b.b = 1;    /* 手柄2 B键按下 */
        }

        if (KEYBD_FCPAD1_TABLE[4] == pbuf[j])fcpad.b.select = 1;    /* 手柄1 select按下 */

        if (KEYBD_FCPAD2_TABLE[4] == pbuf[j])fcpad1.b.select = 1;   /* 手柄2 select按下 */

        if (KEYBD_FCPAD1_TABLE[5] == pbuf[j])fcpad.b.start = 1;     /* 手柄1 start按下 */

        if (KEYBD_FCPAD2_TABLE[5] == pbuf[j])fcpad1.b.start = 1;    /* 手柄2 start按下 */

        if (KEYBD_FCPAD1_TABLE[6] == pbuf[j])fcpad.b.up = 1;        /* 手柄1 up按下 */

        if (KEYBD_FCPAD2_TABLE[6] == pbuf[j])fcpad1.b.up = 1;       /* 手柄2 up按下 */

        if (KEYBD_FCPAD1_TABLE[7] == pbuf[j])fcpad.b.down = 1;      /* 手柄1 down按下 */

        if (KEYBD_FCPAD2_TABLE[7] == pbuf[j])fcpad1.b.down = 1;     /* 手柄2 down按下 */

        if (KEYBD_FCPAD1_TABLE[8] == pbuf[j])fcpad.b.left = 1;      /* 手柄1 left按下 */

        if (KEYBD_FCPAD2_TABLE[8] == pbuf[j])fcpad1.b.left = 1;     /* 手柄2 left按下 */

        if (KEYBD_FCPAD1_TABLE[9] == pbuf[j])fcpad.b.right = 1;     /* 手柄1 right按下 */

        if (KEYBD_FCPAD2_TABLE[9] == pbuf[j])fcpad1.b.right = 1;    /* 手柄2 right按下 */
    }

//    printf("pad1:%x\r\n",fcpad);
//    printf("pad2:%x\r\n",fcpad1);
//    for(j=0;j<8;j++)
//    {
//        printf("%02x ",pbuf[j]);
//    }
//    printf("\r\n");
}

/**
 * @brief       键盘数据解码
 * @param       pbuf        : 输入数据
 * @retval      无
 */
void KEYBRD_Decode(uint8_t *pbuf)
{
    static uint8_t fcbuf[6];    /* 保存最近一次fc手柄的值 */
    uint8_t i;
    pbuf += 2;

    for (i = 0; i < 6; i++)if (pbuf[i] != fcbuf[i])break; /* 不相等,说明有新的按键情况变化 */

    if (i == 6)return;              /* 本次按键没有任何变化,直接返回 */

    for (i = 0; i < 6; i++)fcbuf[i] = pbuf[i];  /* 拷贝新的按键值 */

    KEYBRD_FCPAD_Decode(fcbuf, 1);  /* 更新FC手柄数据 */
}























