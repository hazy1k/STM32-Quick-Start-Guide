/**
 ****************************************************************************************************
 * @file        emwin_demo.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-01-04
 * @brief       emWin的LSTVIEW部件使用实验
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

/*****************************************************************************************************/
/*EMWIN */
#include "GUI.h"
#include "WM.h"
#include "DIALOG.h"
/*****************************************************************************************************/
/*uC-OS3*/
#include "os.h"
#include "cpu.h"


student_info student;/* 学生信息 */
/* 控件ID */
#define ID_FRAMEWIN_0 (GUI_ID_USER + 0x00)
#define ID_LISTVIEW_0 (GUI_ID_USER + 0x01)
#define ID_BUTTON_0 (GUI_ID_USER + 0x02)

/* 用户自定义消息 */
#define WM_DELETEWIN   (WM_USER +0)

int listviewitem = -1;  /* LISTVIEW选中的项目编号 */

/* LISTVIEW列表 */
static const char *ListViewTable[][4] =
{
    {"Brooks", "20141226001", "97", "A"},
    {"Aaron",  "20141226002", "84", "B"},
    {"Faith",  "20141226003", "73", "C"},
    {"Mabel",  "20141226004", "85", "B"},
    {"Sabina", "20141226005", "45", "E"},
    {"Alex",   "20141226006", "98", "A"},
    {"Dana",   "20141226007", "84", "B"},
    {"Wally",  "20141226008", "65", "D"},
};

/* 资源表 */
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] =
{
    { FRAMEWIN_CreateIndirect, "Framewin", ID_FRAMEWIN_0, 0,   0,  320,  240, FRAMEWIN_CF_MOVEABLE, 0x64, 0 },
    { LISTVIEW_CreateIndirect, "Listview", ID_LISTVIEW_0, 45,  15,  220, 140, 0, 0x0, 0 },
    { BUTTON_CreateIndirect,   "Button",   ID_BUTTON_0,   100, 160, 100, 40,  0, 0x0, 0 },
};

/**
 * @brief       对话框回调函数
 * @param       pMsg : 消息
 * @retval      无
 */
static void _cbDialog(WM_MESSAGE *pMsg)
{
    WM_HWIN hItem;
    int     NCode;
    int     Id;
    WM_MESSAGE Msg;
    uint8_t i = 0;

    switch (pMsg->MsgId)
    {
        case WM_INIT_DIALOG:
            /* 初始化FRAMWIN */
            hItem = pMsg->hWin;
            FRAMEWIN_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
            FRAMEWIN_SetFont(hItem, GUI_FONT_20B_ASCII);
            FRAMEWIN_SetTextColor(hItem, GUI_YELLOW);
            FRAMEWIN_SetText(hItem, "ALIENTEK  LISTVIEW");

            /* 初始化LISTVIEW */
            hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0);
            LISTVIEW_SetGridVis(hItem, 1);              /* 网格线可见 */
            LISTVIEW_SetAutoScrollH(hItem, 1);          /* 启用自动水平滚动条 */
            LISTVIEW_SetAutoScrollV(hItem, 1);          /* 启用自动垂直滚动条 */
            LISTVIEW_SetHeaderHeight(hItem, 20);        /* 设置标题的高度 */
            LISTVIEW_SetRowHeight(hItem, 15);           /* 设置LISTVIEW的行高 */
            LISTVIEW_SetBkColor(hItem, 0, 0X00C080FF);  /* 设置背景颜色 */
            /* 向LISTVIEW添加列 */
            LISTVIEW_AddColumn(hItem, 50, "Name", GUI_TA_HCENTER | GUI_TA_VCENTER);
            LISTVIEW_AddColumn(hItem, 80, "Num", GUI_TA_HCENTER | GUI_TA_VCENTER);
            LISTVIEW_AddColumn(hItem, 40, "Score", GUI_TA_HCENTER | GUI_TA_VCENTER);
            LISTVIEW_AddColumn(hItem, 35, "Grade", GUI_TA_HCENTER | GUI_TA_VCENTER);

            /* 向LISTVIEW添加行 */
            for (i = 0; i < GUI_COUNTOF(ListViewTable); i++)
            {
                LISTVIEW_AddRow(hItem, ListViewTable[i]);
            }

            /* 初始化BUTTON */
            hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
            BUTTON_SetFont(hItem, GUI_FONT_COMIC18B_ASCII);
            BUTTON_SetText(hItem, "OK");
            break;

        case WM_NOTIFY_PARENT:
            Id = WM_GetId(pMsg->hWinSrc);
            NCode = pMsg->Data.v;

            switch (Id)
            {
                case ID_LISTVIEW_0:
                    switch (NCode)
                    {
                        case WM_NOTIFICATION_CLICKED:
                            break;

                        case WM_NOTIFICATION_RELEASED:
                            hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0);
                            listviewitem = LISTVIEW_GetSel(hItem); /* 获取选中的项目编号 */
                            /* 返回指定单元格的文本 */
                            LISTVIEW_GetItemText(hItem, 0, listviewitem, (char *)student.name, 20);      /* 获取名字 */
                            LISTVIEW_GetItemText(hItem, 1, listviewitem, (char *)student.num, 20);       /* 获取学号 */
                            LISTVIEW_GetItemText(hItem, 2, listviewitem, (char *)(student.score), 4);    /* 获取分数 */
                            LISTVIEW_GetItemText(hItem, 3, listviewitem, (char *) & (student.grade), 4); /* 获取等级 */
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
                            Msg.MsgId = WM_DELETEWIN;
                            Msg.hWinSrc = pMsg->hWin;
                            WM_SendMessage(WM_HBKWIN, &Msg); /* 发送消息 */
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
 * @param       pMsg : 消息
 * @retval      无
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
            GUI_DispStringAt("ALIENTEK LISTVIEW DEMO", 0, 0);
            GUI_SetColor(GUI_BLACK);
            GUI_SetFont(&GUI_Font20_ASCII);
            GUI_DispStringAt(" Name:", 0, 30);
            GUI_DispStringAt("  Num:", 0, 50);
            GUI_DispStringAt("Score:", 0, 70);
            GUI_DispStringAt("Grade:", 0, 90);

            GUI_SetColor(GUI_RED);
            GUI_DispStringAt((char *)student.name, 65, 30);       /* 显示选中的学生名字 */
            GUI_DispStringAt((char *)student.num, 65, 50);        /* 显示选中的学生学号 */
            GUI_DispStringAt((char *)student.score, 65, 70);      /* 显示选中的学生分数 */
            GUI_DispStringAt((char *) & (student.grade), 65, 90); /* 显示选中的学生的等级 */
            break;

        case WM_DELETEWIN:                         /* 删除窗口 */
            hWin = pMsg->hWinSrc;
            WM_DeleteWindow(hWin);
            WM_InvalidateWindow(WM_HBKWIN);        /* 背景窗口无效 */
            WM_CreateTimer(WM_HBKWIN, 0, 3000, 0); /* 创建定时器 */
            break;

        case WM_TIMER:
            GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);/* 创建对话框 */
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
        GUI_Delay(200);
    }
}
