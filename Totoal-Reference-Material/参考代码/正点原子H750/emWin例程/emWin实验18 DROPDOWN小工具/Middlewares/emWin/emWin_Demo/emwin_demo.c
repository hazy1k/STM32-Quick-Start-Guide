/**
 ****************************************************************************************************
 * @file        emwin_demo.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-01-04
 * @brief       emWin的DROPDOWN部件使用实验
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


#define ID_FRAMEWIN_0 (GUI_ID_USER + 0x02)
#define ID_DROPDOWN_0 (GUI_ID_USER + 0x04)
/* 对话框资源表 */
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] =
{
    { FRAMEWIN_CreateIndirect, "Framewin", ID_FRAMEWIN_0, 0, 0, 320, 240, FRAMEWIN_CF_MOVEABLE, 0x64, 0 },
    { DROPDOWN_CreateIndirect, "Dropdown", ID_DROPDOWN_0, 88, 24, 109, 60, 0, 0x0, 0 },
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
            hItem = pMsg->hWin;
            /* 初始化FRAMEWIN */
            FRAMEWIN_SetText(hItem, "DROPDOWN ");
            FRAMEWIN_SetFont(hItem, GUI_FONT_16_ASCII);
            FRAMEWIN_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
            /* 初始化DROPDOWN */
            hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_0);
            DROPDOWN_SetFont(hItem, GUI_FONT_13H_ASCII);
            DROPDOWN_SetAutoScroll(hItem, 1);  /* 启用自动使用滚动条 */
            DROPDOWN_AddString(hItem, "China");
            DROPDOWN_AddString(hItem, "Cambodia");
            DROPDOWN_AddString(hItem, "Japan");
            DROPDOWN_AddString(hItem, "Australien");
            DROPDOWN_AddString(hItem, "Canada");
            break;

        case WM_NOTIFY_PARENT:
            Id    = WM_GetId(pMsg->hWinSrc);
            NCode = pMsg->Data.v;

            switch (Id)
            {
                case ID_DROPDOWN_0: /* DROPDOWN通知代码 */
                    switch (NCode)
                    {
                        case WM_NOTIFICATION_CLICKED:
                            break;

                        case WM_NOTIFICATION_RELEASED:
                            break;

                        case WM_NOTIFICATION_SEL_CHANGED:
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
 * @brief       emwin_main(emwin入口)
 * @param       无
 * @return      无
 */
void emwin_main(void) 
{
    GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);

    while (1)
    {
        GUI_Delay(100);
    }
}
