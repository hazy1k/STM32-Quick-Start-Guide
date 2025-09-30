/**
 ****************************************************************************************************
 * @file        emwin_demo.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-01-04
 * @brief       emWin的Button部件使用实验
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
#include "./BSP/BEEP/beep.h"
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


#define ID_FRAMEWIN_0 (GUI_ID_USER + 0x00)
#define ID_BUTTON_0 (GUI_ID_USER + 0x01)
#define ID_BUTTON_1 (GUI_ID_USER + 0x02)

/* 对话框资源表 */
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] =
{
    { FRAMEWIN_CreateIndirect, "Framewin", ID_FRAMEWIN_0, 0, 0, 800, 480, 0, 0x64, 0 },
    { BUTTON_CreateIndirect, "Button", ID_BUTTON_0, 300, 122, 150, 50, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "Button", ID_BUTTON_1, 300, 251, 150, 50, 0, 0x0, 0 },
};

/**
 * @brief       对话框回调函数 
 * @param       无
 * @return      无
 */
static void _cbDialog(WM_MESSAGE *pMsg)
{
    WM_HWIN hItem;
    int     NCode;
    int     Id;

    switch (pMsg->MsgId)
    {
        case WM_INIT_DIALOG:
            /* 初始化对话框 */
            hItem = pMsg->hWin;
            FRAMEWIN_SetTitleHeight(hItem, 30);
            FRAMEWIN_SetText(hItem, "ALIENTE BUTTON USE");
            FRAMEWIN_SetFont(hItem, GUI_FONT_24_ASCII);
            FRAMEWIN_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
            FRAMEWIN_SetTextColor(hItem, 0x0000FFFF);

            /* 初始化BUTTON0 */
            hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
            BUTTON_SetFont(hItem, GUI_FONT_24_ASCII);
            BUTTON_SetText(hItem, "LED1");

            /* 初始化BUTTON1 */
            hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
            BUTTON_SetText(hItem, "BEEP");
            BUTTON_SetFont(hItem, GUI_FONT_24_ASCII);
            break;

        case WM_NOTIFY_PARENT:
            Id    = WM_GetId(pMsg->hWinSrc);
            NCode = pMsg->Data.v;

            switch (Id)
            {
                case ID_BUTTON_0: /* BUTTON_0的通知代码，控制LED1 */
                    switch (NCode)
                    {
                        case WM_NOTIFICATION_CLICKED:
                            break;

                        case WM_NOTIFICATION_RELEASED: /* 按钮被按下并释放 */
                            LED1_TOGGLE();             /* LED1反转 */
                            break;
                    }

                    break;

                case ID_BUTTON_1: /* BUTTON_1的通知代码，控制BEEP */
                    switch (NCode)
                    {
                        case WM_NOTIFICATION_CLICKED:
                            break;

                        case WM_NOTIFICATION_RELEASED:
                            BEEP_TOGGLE(); /* 蜂鸣器反转 */
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
 * @brief       创建一个对话框 
 * @param       无
 * @return      无
 */
WM_HWIN _CreateFramewin(void)
{
    WM_HWIN hWin;

    hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
    return hWin;
}

/**
 * @brief       emwin_main(emwin入口)
 * @param       无
 * @return      无
 */
void emwin_main(void) 
{
    _CreateFramewin();

    while (1)
    {
        GUI_Delay(100);
    }
}
