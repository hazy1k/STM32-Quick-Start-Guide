/**
 ****************************************************************************************************
 * @file        emwin_demo.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-01-04
 * @brief       emWin的LISTWHELL部件使用实验
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


DateStruct datedemo = {"2020", "January", "01"}; /* 日期,默认初始化为1990年1月1日 */

uint8_t listwheelindex[3];   /* LISTWHEEL选中的项目号 */
/* 年 */
const char *_apYear[] =
{
    "1990", "1991", "1992", "1993", "1994", "1995", "1996",
    "1997", "1998", "1999", "2000", "2001", "2002", "2003",
    "2004", "2005", "2006", "2007", "2008", "2009", "2010",
    "2011", "2012", "2013", "2014", "2015", "2016", "2017",
    "2018", "2019", "2020", "2021", "2022", "2023", "2024",
};

/* 月 */
const char *_apMonth[] =
{
    "January",      "February",     "March",    "April",
    "May",          "June",         "July",     "August",
    "September",    "October",      "November", "December",
};

/* 日 */
const char *_apDay[] =
{
    "01", "02", "03", "04", "05", "06",
    "07", "08", "09", "10", "11", "12",
    "13", "14", "15", "16", "17", "18",
    "19", "20", "21", "22", "23", "24",
    "25", "26", "27", "28", "29", "30", "31",
};

/* 控件ID */
#define ID_FRAMEWIN_0  (GUI_ID_USER + 0x00)
#define ID_BUTTON_0    (GUI_ID_USER + 0x02)
#define ID_TEXT_0      (GUI_ID_USER + 0x03)
#define ID_TEXT_1      (GUI_ID_USER + 0x04)
#define ID_TEXT_2      (GUI_ID_USER + 0x05)
#define ID_LISTWHEEL_0 (GUI_ID_USER + 0x06)
#define ID_LISTWHEEL_1 (GUI_ID_USER + 0x07)
#define ID_LISTWHEEL_2 (GUI_ID_USER + 0x08)

/* 对话框资源表 */
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] =
{
    { FRAMEWIN_CreateIndirect,  "Framewin",  ID_FRAMEWIN_0,  0,   0,   320, 240, FRAMEWIN_CF_MOVEABLE, 0x64, 0 },
    { BUTTON_CreateIndirect,    "Button",    ID_BUTTON_0,    192, 150, 100, 40,  0, 0x0,  0 },
    { TEXT_CreateIndirect,      "Text",      ID_TEXT_0,      190, 40,   51, 20,  0, 0x64, 0 },
    { TEXT_CreateIndirect,      "Text",      ID_TEXT_1,      178, 60,   66, 20,  0, 0x64, 0 },
    { TEXT_CreateIndirect,      "Text",      ID_TEXT_2,      196, 80,   53, 20,  0, 0x64, 0 },
    { LISTWHEEL_CreateIndirect, "Listwheel", ID_LISTWHEEL_0, 10,  30,   50, 150, 0, 0x0,  0 },
    { LISTWHEEL_CreateIndirect, "Listwheel", ID_LISTWHEEL_1, 60,  30,   80, 150, 0, 0x0,  0 },
    { LISTWHEEL_CreateIndirect, "Listwheel", ID_LISTWHEEL_2, 140, 30,   30, 150, 0, 0x0,  0 },
};

/**
 * @brief       在LISTWHEEL0上绘制红色指示行
 * @param       pDrawItemInfo : 绘制项的信息
 * @retval      无
 */
static int _ListWheel0_OwnerDraw(const WIDGET_ITEM_DRAW_INFO *pDrawItemInfo)
{
    switch (pDrawItemInfo->Cmd)
    {
        case WIDGET_ITEM_DRAW_OVERLAY:
            GUI_SetColor(GUI_RED);
            GUI_DrawHLine(60, 0, 50);
            GUI_DrawHLine(90, 0, 50);
            break;

        default:
            return LISTWHEEL_OwnerDraw(pDrawItemInfo);
    }

    return 0;
}

/**
 * @brief       在LISTWHEEL1上绘制红色指示行
 * @param       pDrawItemInfo : 绘制项的信息
 * @retval      无
 */
static int _ListWheel1_OwnerDraw(const WIDGET_ITEM_DRAW_INFO *pDrawItemInfo)
{
    switch (pDrawItemInfo->Cmd)
    {
        case WIDGET_ITEM_DRAW_OVERLAY:
            GUI_SetColor(GUI_RED);
            GUI_DrawHLine(60, 0, 80);
            GUI_DrawHLine(90, 0, 80);
            break;

        default:
            return LISTWHEEL_OwnerDraw(pDrawItemInfo);
    }

    return 0;
}

/**
 * @brief       在LISTWHEEL2上绘制红色指示行
 * @param       pDrawItemInfo : 绘制项的信息
 * @retval      无
 */
static int _ListWheel2_OwnerDraw(const WIDGET_ITEM_DRAW_INFO *pDrawItemInfo)
{
    switch (pDrawItemInfo->Cmd)
    {
        case WIDGET_ITEM_DRAW_OVERLAY:
            GUI_SetColor(GUI_RED);
            GUI_DrawHLine(60, 0, 30);
            GUI_DrawHLine(90, 0, 30);
            break;

        default:
            return LISTWHEEL_OwnerDraw(pDrawItemInfo);
    }

    return 0;
}

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
    int     i = 0;
    static uint8_t LineHeight = 30;

    switch (pMsg->MsgId)
    {
        case WM_PAINT:
            GUI_DrawGradientV(172, 10, 175, 200, GUI_RED, GUI_BLUE);       /* 绘制垂直梯度填充矩形 */
            GUI_SetColor(GUI_RED);          /* 设置为红色 */
            GUI_SetFont(&GUI_Font8x18);     /* 设置字体 */
            GUI_DispStringAt(datedemo.year, 230, 40); /* 显示年 */
            GUI_DispStringAt(datedemo.month, 230, 60); /* 显示月 */
            GUI_DispStringAt(datedemo.day, 230, 80); /* 显示日 */
            break;

        case WM_INIT_DIALOG:
            /* 初始化FRAMEWIN */
            hItem = pMsg->hWin;
            FRAMEWIN_SetFont(hItem, GUI_FONT_20_ASCII);
            FRAMEWIN_SetText(hItem, "ALIENTEK LISTWHEEL");
            FRAMEWIN_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
            FRAMEWIN_SetTextColor(hItem, GUI_YELLOW);

            /* 初始化LISTWHEEL */
            for (i = 0; i < 3; i++)
            {
                hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_0 + i);
                LISTWHEEL_SetFont(hItem, &GUI_FontComic18B_ASCII);          /* 设置字体 */
                LISTWHEEL_SetTextAlign(hItem, GUI_TA_VCENTER | GUI_TA_HCENTER); /* 对齐方式 */
                LISTWHEEL_SetSnapPosition(hItem, (150 - LineHeight) / 2);   /* 设置吸附的位置 */
                LISTWHEEL_SetLineHeight(hItem, LineHeight);                 /* 设置绘制数据项所使用的行高 */
                LISTWHEEL_SetTextColor(hItem, LISTWHEEL_CI_SEL, 0xEE8822);  /* 设置选中的条目的文本颜色 */
            }

            /* 设置Year的LISTWHEEL */
            hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_0);
            LISTWHEEL_SetOwnerDraw(hItem, _ListWheel0_OwnerDraw);        /* 绘制红色指示行 */

            for (i = 0; i < GUI_COUNTOF(_apYear); i++) /* 添加数据项 */
            {
                LISTWHEEL_AddString(hItem, *(_apYear + i));
            }

            /* 设置Month的LISTWHEEL */
            hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_1);
            LISTWHEEL_SetOwnerDraw(hItem, _ListWheel1_OwnerDraw);        /* 绘制红色指示行 */

            for (i = 0; i < GUI_COUNTOF(_apMonth); i++) /* 添加数据项 */
            {
                LISTWHEEL_AddString(hItem, *(_apMonth + i));
            }

            /* 设置Day的LISTWHEEL */
            hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_2);
            LISTWHEEL_SetOwnerDraw(hItem, _ListWheel2_OwnerDraw);        /* 绘制红色指示行 */

            for (i = 0; i < GUI_COUNTOF(_apDay); i++) /* 添加数据项 */
            {
                LISTWHEEL_AddString(hItem, *(_apDay + i));
            }

            /* 初始化BUTTON */
            hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
            BUTTON_SetFont(hItem, GUI_FONT_COMIC18B_1);
            BUTTON_SetText(hItem, "ok");

            /* 初始化TEXT */
            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
            TEXT_SetFont(hItem, &GUI_Font8x18);
            TEXT_SetText(hItem, "Year:");

            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
            TEXT_SetFont(hItem, &GUI_Font8x18);
            TEXT_SetText(hItem, "Month:");

            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_2);
            TEXT_SetFont(hItem, &GUI_Font8x18);
            TEXT_SetText(hItem, "Day:");
            break;

        case WM_NOTIFY_PARENT:
            Id    = WM_GetId(pMsg->hWinSrc);
            NCode = pMsg->Data.v;

            switch (Id)
            {
                case ID_BUTTON_0:
                    switch (NCode)
                    {
                        case WM_NOTIFICATION_CLICKED:
                            break;

                        case WM_NOTIFICATION_RELEASED:
                            WM_InvalidateWindow(pMsg->hWin);     /* FRAMEWIN窗口无效 */
                            break;
                    }

                    break;

                case ID_LISTWHEEL_0:
                    switch (NCode)
                    {
                        case WM_NOTIFICATION_CLICKED:
                            break;

                        case WM_NOTIFICATION_RELEASED:
                            break;

                        case WM_NOTIFICATION_SEL_CHANGED:
                            hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_0);
                            listwheelindex[0] = LISTWHEEL_GetPos(hItem);    /* 获取当前所选数据项的索引 */
                            /* 注意：LISTWHEEL_GetPos()只是获取到滚轮停下来所选中的那个项目编号，但是此时并没有选中这个 */
                            /* 项目，必须调用LISTWHEEL_SetSel()来选中这个项目。 */
                            LISTWHEEL_SetSel(hItem, listwheelindex[0]);     /* 设置当前所选数据项 */
                            LISTWHEEL_GetItemText(hItem, listwheelindex[0], datedemo.year, 5); /* 获取所选中的项目的文本 */
                            break;
                    }

                    break;

                case ID_LISTWHEEL_1:
                    switch (NCode)
                    {
                        case WM_NOTIFICATION_CLICKED:
                            break;

                        case WM_NOTIFICATION_RELEASED:
                            break;

                        case WM_NOTIFICATION_SEL_CHANGED:
                            hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_1);
                            listwheelindex[1] = LISTWHEEL_GetPos(hItem); /* 获取当前所选数据项的索引 */
                            LISTWHEEL_SetSel(hItem, listwheelindex[1]); /* 设置当前所选数据项 */
                            LISTWHEEL_GetItemText(hItem, listwheelindex[1], datedemo.month, 20); /* 获取所选中的项目的文本 */
                            break;
                    }

                    break;

                case ID_LISTWHEEL_2:
                    switch (NCode)
                    {
                        case WM_NOTIFICATION_CLICKED:
                            break;

                        case WM_NOTIFICATION_RELEASED:
                            break;

                        case WM_NOTIFICATION_SEL_CHANGED:
                            hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_2);
                            listwheelindex[2] = LISTWHEEL_GetPos(hItem); /* 获取当前所选数据项的索引 */
                            LISTWHEEL_SetSel(hItem, listwheelindex[2]); /* 设置当前所选数据项 */
                            LISTWHEEL_GetItemText(hItem, listwheelindex[2], datedemo.day, 5); /* 获取所选中的项目的文本 */
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
    switch (pMsg->MsgId)
    {
        case WM_PAINT:
            GUI_SetBkColor(GUI_BLUE);
            GUI_Clear();
            GUI_SetColor(GUI_YELLOW);
            GUI_SetFont(&GUI_Font24B_ASCII);
            GUI_DispStringAt("ALIENTEK LISTWHEEL DEMO", 0, 0);

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
