/**
 ****************************************************************************************************
 * @file        emwin_demo.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-01-04
 * @brief       emWin的Listbox部件使用实验
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
#include "./BSP/BEEP/beep.h"
#include "./BSP/LED/led.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"

/**************************************************************************************************** */
/*EMWIN */
#include "GUI.h"
#include "WM.h"
#include "DIALOG.h"
/**************************************************************************************************** */
/*uC-OS3*/
#include "os.h"
#include "cpu.h"


/* 控件ID  */
#define ID_FRAMEWIN_0   (GUI_ID_USER + 0x00)
#define ID_LISTBOX_0    (GUI_ID_USER + 0x01)
#define ID_TEXT_0       (GUI_ID_USER + 0x02)
#define ID_BUTTON_0     (GUI_ID_USER + 0x04)
#define ID_BUTTON_1     (GUI_ID_USER + 0x05)

/* 用户自定义消息 */
#define WM_DELETEWIN   (WM_USER +0)

int listboxitem = -1;        /* LISTBOX所选中的项目标号 */

/* 对话框资源表 */
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] =
{
    { FRAMEWIN_CreateIndirect, "Framewin", ID_FRAMEWIN_0, 0, 0, 320, 240, FRAMEWIN_CF_MOVEABLE, 0x64, 0 },
    { LISTBOX_CreateIndirect, "Listbox", ID_LISTBOX_0, 21, 20, 115, 128, 0, 0x0, 0 },
    { TEXT_CreateIndirect,  "Text", ID_TEXT_0, 170, 70, 133, 35, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "Button", ID_BUTTON_0, 30, 160, 90, 40, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "Button", ID_BUTTON_1, 175, 160, 90, 40, 0, 0x0, 0 },
};

/**
 * @brief       对话框回调函数
 * @param       pMsg : 传入消息
 * @return      无
 */
static void _cbDialog(WM_MESSAGE *pMsg)
{
    WM_HWIN hItem;
    int     NCode;
    int     Id;
    static char textbuf[20];
    WM_MESSAGE Msg;

    switch (pMsg->MsgId)
    {
        case WM_INIT_DIALOG:
            /* FRAMWIN控件初始化 */
            hItem = pMsg->hWin;
            FRAMEWIN_SetFont(hItem, GUI_FONT_20B_ASCII);
            FRAMEWIN_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
            FRAMEWIN_SetText(hItem, "ALIENTEK LISTBOX");
            FRAMEWIN_SetTextColor(hItem, GUI_YELLOW);

            /* LISTBOX控件初始化 */
            hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTBOX_0);
            LISTBOX_SetFont(hItem, GUI_FONT_20_ASCII);
            LISTBOX_SetAutoScrollH(hItem, 1);              /* 激活自动使用水平滚动条 */
            LISTBOX_SetAutoScrollV(hItem, 1);              /* 激活自动使用垂直滚动条 */
            LISTBOX_AddString(hItem, "LED1 ON");
            LISTBOX_AddString(hItem, "LED1 OFF");
            LISTBOX_AddString(hItem, "BEEP ON");
            LISTBOX_AddString(hItem, "BEEP OFF");
            LISTBOX_AddString(hItem, "ALL ON");
            LISTBOX_AddString(hItem, "ALL OFF");

            /* CHECKBOX控件初始化 */
            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
            TEXT_SetTextColor(hItem, GUI_RED);
            TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
            TEXT_SetText(hItem, "NO SELECT!");

            /* BUTTON控件初始化 */
            hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
            BUTTON_SetText(hItem, "OK");
            BUTTON_SetFont(hItem, GUI_FONT_COMIC18B_ASCII);

            hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
            BUTTON_SetFont(hItem, GUI_FONT_COMIC18B_ASCII);
            BUTTON_SetText(hItem, "CANCEL");
            break;

        case WM_NOTIFY_PARENT:
            Id = WM_GetId(pMsg->hWinSrc);
            NCode = pMsg->Data.v;

            switch (Id)
            {
                case ID_LISTBOX_0:
                    switch (NCode)
                    {
                        case WM_NOTIFICATION_CLICKED:
                            break;

                        case WM_NOTIFICATION_RELEASED:
                            hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTBOX_0);
                            listboxitem = LISTBOX_GetSel(hItem);                  /* 获取选中的项目标号 */
                            LISTBOX_GetItemText(hItem, listboxitem, textbuf, 10); /* 获取选中的条目的内容 */
                            break;

                        case WM_NOTIFICATION_SEL_CHANGED:
                            break;
                    }

                    break;

                case ID_BUTTON_0:
                    switch (NCode)
                    {
                        case WM_NOTIFICATION_CLICKED:
                            break;

                        case WM_NOTIFICATION_RELEASED:
                            switch (listboxitem)
                            {
                                case 0:
                                    LED1(0);    /* LED1打开 */
                                    break;

                                case 1:
                                    LED1(1);    /* LED1关闭 */
                                    break;

                                case 2:
                                    BEEP(1);   /* 蜂鸣器打开 */
                                    break;

                                case 3:
                                    BEEP(0);   /* 蜂鸣器关闭 */
                                    break;

                                case 4:
                                    LED1(0);   /* LED1打开 */
                                    BEEP(1);   /* 蜂鸣器打开 */
                                    break;

                                case 5:
                                    LED1(1);   /* LED1关闭 */
                                    BEEP(0);   /* 蜂鸣器关闭 */
                                    break;
                            }

                            if (listboxitem >= 0)
                            {
                                hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0); /* 在TEXT控件中显示选中的内容 */
                                TEXT_SetText(hItem, textbuf);
                            }

                            break;
                    }

                    break;

                case ID_BUTTON_1:
                    switch (NCode)
                    {
                        case WM_NOTIFICATION_CLICKED:
                            break;

                        case WM_NOTIFICATION_RELEASED:
                            Msg.MsgId = WM_DELETEWIN;
                            Msg.hWinSrc = pMsg->hWin;
                            WM_SendMessage(WM_HBKWIN, &Msg);
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
 * @param       pMsg : 传入消息
 * @return      无
 */
static void _cbBkWindow(WM_MESSAGE *pMsg)
{
    static WM_HWIN hWin;

    switch (pMsg->MsgId)
    {
        case WM_PAINT:
            GUI_SetBkColor(GUI_BLUE);
            GUI_Clear();
            GUI_SetColor(GUI_YELLOW);
            GUI_SetFont(&GUI_Font24B_ASCII);
            GUI_DispStringAt("ALIENTEK LISTBOX DEMO", 0, 0);
            break;

        case WM_DELETEWIN:                         /* 删除窗口 */
            hWin = pMsg->hWinSrc;
            WM_DeleteWindow(hWin);
            WM_InvalidateWindow(WM_HBKWIN);        /* 背景窗口无效 */
            WM_CreateTimer(WM_HBKWIN, 0, 3000, 0); /* 创建定时器 */
            break;

        case WM_TIMER:
            GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0); /* 创建对话框 */
            break;

        default:
            WM_DefaultProc(pMsg);
    }
}

/**
 * @brief       emwin_main(emwin入口)
 * @param       无
 * @return      无
 */
void emwin_main(void) 
{
    WM_SetCallback(WM_HBKWIN, _cbBkWindow); /* 设置背景窗口回调函数 */
    GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
    
    while(1)
    {
        GUI_Delay(100);
    }
}
