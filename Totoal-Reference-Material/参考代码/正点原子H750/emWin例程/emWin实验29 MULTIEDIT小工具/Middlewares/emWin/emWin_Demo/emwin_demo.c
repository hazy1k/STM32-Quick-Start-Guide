/**
 ****************************************************************************************************
 * @file        emwin_demo.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-01-04
 * @brief       emWin的Multiedit部件使用实验
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 Mini Pro H750开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#include "emwin_demo.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/LED/led.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"

/*****************************************************************************************************/
/*EMWIN*/
#include "GUI.h"
#include "WM.h"
#include "DIALOG.h"
/*****************************************************************************************************/
/*uC-OS3*/
#include "os.h"
#include "cpu.h"


/* 控件ID */
#define ID_FRAMEWIN_0 (GUI_ID_USER + 0x00)
#define ID_MULTIEDIT_0 (GUI_ID_USER + 0x01)
#define ID_EDIT_0 (GUI_ID_USER + 0x02)
#define ID_BUTTON_0 (GUI_ID_USER + 0x03)
#define ID_TEXT_0 (GUI_ID_USER + 0x04)
#define ID_TEXT_1 (GUI_ID_USER + 0x05)

/* 对话框资源表 */
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] =
{
    { FRAMEWIN_CreateIndirect, "Framewin", ID_FRAMEWIN_0, 0, 0, 320, 240, FRAMEWIN_CF_MOVEABLE, 0x64, 0 },
    { MULTIEDIT_CreateIndirect, "Multiedit", ID_MULTIEDIT_0, 20, 35, 130, 150, 0, 0x0, 0 },
    { EDIT_CreateIndirect, "Edit", ID_EDIT_0, 170, 61, 133, 26, 0, 0x64, 0 },
    { BUTTON_CreateIndirect, "Button", ID_BUTTON_0, 188, 129, 92, 46, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "Text", ID_TEXT_0, 30, 5, 107, 20, 0, 0x64, 0 },
    { TEXT_CreateIndirect, "Text", ID_TEXT_1, 198, 5, 80, 20, 0, 0x64, 0 },
};

/**
 * @brief       对话框回调函数
 * @param       pMsg ：消息
 * @retval      无
 */
static void _cbDialog(WM_MESSAGE *pMsg)
{
    WM_HWIN hItem;
    int     NCode;
    int     Id;
    uint8_t databuff[50];
    int datalen = 0;

    switch (pMsg->MsgId)
    {
        case WM_PAINT:
            GUI_DrawGradientV(162, 10, 165, 200, GUI_RED, GUI_BLUE); /* 绘制垂直梯度填充矩形 */
            GUI_SetColor(GUI_RED);          /* 设置为红色 */
            GUI_SetFont(&GUI_Font8x18);     /* 设置字体 */
            break;

        case WM_INIT_DIALOG:

            /* FRAMWIN初始化 */
            hItem = pMsg->hWin;
            FRAMEWIN_SetFont(hItem, GUI_FONT_20_ASCII);
            FRAMEWIN_SetTextColor(hItem, 0x0000FFFF);
            FRAMEWIN_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
            FRAMEWIN_SetText(hItem, "ALIENTEK MULTIEDIT");

            /* MULTIEDIT初始化 */
            hItem = WM_GetDialogItem(pMsg->hWin, ID_MULTIEDIT_0);
            MULTIEDIT_SetFont(hItem, GUI_FONT_8X18);
            MULTIEDIT_SetTextColor(hItem, MULTIEDIT_CI_EDIT, 0x00FF0000);
            MULTIEDIT_SetAutoScrollH(hItem, 1);     /* 激活自动水平滚动条 */
            MULTIEDIT_SetAutoScrollV(hItem, 1);     /* 激活自动垂直滚动条 */
            MULTIEDIT_EnableBlink(hItem, 500, 1);   /* 开启光标闪烁 */

            /* EDIT初始化 */
            hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_0);
            EDIT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
            EDIT_SetFont(hItem, GUI_FONT_16B_ASCII);
            EDIT_SetText(hItem, "www.openedv.com");
            EDIT_EnableBlink(hItem, 500, 1);        /* 开启光标闪烁 */

            /* BUTTON初始化 */
            hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
            BUTTON_SetFont(hItem, GUI_FONT_COMIC18B_ASCII);
            BUTTON_SetText(hItem, "SEND");

            /* TEXT初始化 */
            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
            TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
            TEXT_SetFont(hItem, GUI_FONT_16B_ASCII);
            TEXT_SetTextColor(hItem, 0x000000FF);
            TEXT_SetText(hItem, "Receive Area");

            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
            TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
            TEXT_SetFont(hItem, GUI_FONT_16B_ASCII);
            TEXT_SetTextColor(hItem, 0x000000FF);
            TEXT_SetText(hItem, "Send Area");
            break;

        case WM_NOTIFY_PARENT:
            Id    = WM_GetId(pMsg->hWinSrc);
            NCode = pMsg->Data.v;

            switch (Id)
            {
                case ID_MULTIEDIT_0:
                    switch (NCode)
                    {
                        case WM_NOTIFICATION_CLICKED:
                            break;

                        case WM_NOTIFICATION_RELEASED:
                            break;

                        case WM_NOTIFICATION_VALUE_CHANGED:
                            break;
                    }

                    break;

                case ID_EDIT_0:
                    switch (NCode)
                    {
                        case WM_NOTIFICATION_CLICKED:
                            break;

                        case WM_NOTIFICATION_RELEASED:
                            break;

                        case WM_NOTIFICATION_VALUE_CHANGED:
                            break;
                    }

                    break;

                case ID_BUTTON_0:
                    switch (NCode)
                    {
                        case WM_NOTIFICATION_CLICKED:
                            break;

                        case WM_NOTIFICATION_RELEASED:
                            hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_0);
                            datalen = EDIT_GetNumChars(hItem) + 1;          /* 获取EDIT中的文本长度，注意要加一个字节的结束符'\0' */
                            EDIT_GetText(hItem, (char *)databuff, datalen); /* 获取EDIT中的文本 */
                            printf("%s\r\n", databuff);                     /* 通过串口发送出去 */
                            break;
                    }

                    break;
            }

            break;

        default:
            WM_DefaultProc(pMsg);
            break;
    }
}

/**
 * @brief       背景窗口WM_HBKWIN回调函数
 * @param       pMsg ：消息
 * @retval      无
 */
static void _cbBkWindow(WM_MESSAGE *pMsg)
{
    switch (pMsg->MsgId)
    {
        case WM_PAINT:
            GUI_SetBkColor(GUI_BLUE);
            GUI_Clear();
            GUI_SetColor(GUI_YELLOW);
            GUI_SetFont(&GUI_Font24B_ASCII);
            GUI_DispStringAt("ALIENTEK MULIEDT DEMO", 0, 0);

        default:
            WM_DefaultProc(pMsg);
    }
}

/**
 * @brief       emwin_main(emwin入口)
 * @param       无
 * @retval      无
 */
void emwin_main(void)
{
    WM_HWIN hWin;
    WM_HWIN hItem;
    uint8_t len;
    WM_SetCallback(WM_HBKWIN, _cbBkWindow);     /* 设置背景窗口回调函数 */
    hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
    hItem = WM_GetDialogItem(hWin, ID_MULTIEDIT_0); /* 获取MULTIEDIT控件句柄 */

    while (1)
    {
        if (g_usart_rx_sta & 0x8000)          /* 收到数据 */
        {
            len = g_usart_rx_sta & 0x3fff;    /* 得到此次接收到的数据长度 */
            g_usart_rx_buf[len] = '\r';       /* 添加换行符 */
            g_usart_rx_buf[len + 1] = '\n';
            MULTIEDIT_AddText(hItem, (char *)g_usart_rx_buf); /* 在MULTIEDIT中显示数据 */
            g_usart_rx_sta = 0;
        }
        else
        {
            GUI_Delay(100);
        }
    }
}
