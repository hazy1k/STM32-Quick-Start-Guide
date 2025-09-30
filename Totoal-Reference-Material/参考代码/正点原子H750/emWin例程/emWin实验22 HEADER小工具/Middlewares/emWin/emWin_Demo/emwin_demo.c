/**
 ****************************************************************************************************
 * @file        emwinemwin_headed_demo.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-01-04
 * @brief       emWin的HEADER部件使用实验
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
#include "./MALLOC/malloc.h"
#include "./BSP/TIMER/btim.h"
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


#define MSG_CHANGE_MAIN_TEXT (WM_USER+0)
#define MSG_CHANGE_INFO_TEXT (WM_USER+1)

static HEADER_Handle _hHeader;
static char _acMainText[100];
static char _acInfoText[100];


/**
 * @brief       向背景窗口发送一条消息并且使窗口无效，这样背景窗口的回调函数
                就会显示新的文本信息
 * @param       pStr : 传入字符串
 * @retval      无
 */
static void _ChangeMainText(char *pStr)
{
    WM_MESSAGE Message;
    Message.MsgId = MSG_CHANGE_MAIN_TEXT;   /* 组建消息 */
    Message.Data.p = pStr;
    WM_SendMessage(WM_HBKWIN, &Message);    /* 发送消息 */
}

/**
 * @brief       向背景窗口发送一条消息并且使窗口无效，这样背景窗口的回调函数
                就会显示新的文本信息
 * @param       pStr : 传入字符串
 * @retval      无
 */
static void _ChangeInfoText(char *pStr)
{
    WM_MESSAGE Message;
    Message.MsgId = MSG_CHANGE_INFO_TEXT;   /* 组建消息 */
    Message.Data.p = pStr;
    WM_SendMessage(WM_HBKWIN, &Message);    /* 发送消息 */
}

/**
 * @brief       窗口回调函数
 * @param       pMsg : 消息
 * @retval      无
 */
static void _cbBkWindow(WM_MESSAGE *pMsg)
{
    switch (pMsg->MsgId)
    {
        case MSG_CHANGE_MAIN_TEXT:
            strcpy(_acMainText, pMsg->Data.p);
            WM_InvalidateWindow(pMsg->hWin);    /* 背景窗口无效化 */
            break;

        case MSG_CHANGE_INFO_TEXT:
            strcpy(_acInfoText, pMsg->Data.p);
            WM_InvalidateWindow(pMsg->hWin);    /* 背景窗口无效化 */
            break;

        case WM_PAINT:
            GUI_SetBkColor(GUI_BLACK);          /* 设置黑色背景 */
            GUI_Clear();                        /* 刷新背景 */
            GUI_SetColor(GUI_WHITE);            /* 设置字体颜色 */
            GUI_SetFont(&GUI_Font24_ASCII);     /* 设置字体大小 */
            GUI_DispStringHCenterAt("HEADER Widget - Sample", 160, 5);
            GUI_SetFont(&GUI_Font8x16);
            GUI_DispStringAt(_acMainText, 5, 40); /* 显示_acMainText数据 */
            GUI_SetFont(&GUI_Font8x8);
            GUI_DispStringAt(_acInfoText, 5, 60); /* 显示_acInfoText数据 */
            break;

        default:
            WM_DefaultProc(pMsg);
    }
}

/**
 * @brief       绘制色块
 * @param       无
 * @retval      无
 */
static void _OnPoint(void)
{
    GUI_RECT Rect;
    GUI_SetBkColor(GUI_GRAY);                         /* 设置背景颜色 */
    GUI_Clear();                                      /* 清屏 */
    WM_GetClientRect(&Rect);                          /* 获取客户区大小 */
    Rect.x1 = HEADER_GetItemWidth(_hHeader, 0);       /* 获取header小工具项目0的宽度 */
    GUI_SetColor(GUI_RED);
    GUI_FillRect(Rect.x0, Rect.y0, Rect.x1, Rect.y1); /* 绘制红色矩形 */
    Rect.x0 = Rect.x1;
    Rect.x1 += HEADER_GetItemWidth(_hHeader, 1);      /* 获取header小工具项目1的宽度 */
    GUI_SetColor(GUI_GREEN);
    GUI_FillRect(Rect.x0, Rect.y0, Rect.x1, Rect.y1); /* 绘制绿色矩形 */
    Rect.x0 = Rect.x1;
    Rect.x1 += HEADER_GetItemWidth(_hHeader, 2);      /* 获取header小工具项目2的宽度 */
    GUI_SetColor(GUI_BLUE);
    GUI_FillRect(Rect.x0, Rect.y0, Rect.x1, Rect.y1); /* 绘制蓝色矩形 */
}

/**
 * @brief       窗口回调函数
 * @param       pMsg ：消息
 * @retval      无
 */
static void _cbWindow(WM_MESSAGE *pMsg)
{
    switch (pMsg->MsgId)
    {
        case WM_PAINT:
            _OnPoint();
            break;

        default:
            WM_DefaultProc(pMsg);
    }
}

/**
 * @brief       演示
 * @param       无
 * @retval      无
 */
static void _Demo(void)
{
    int Cnt = 10;
    char acInfoText[] = "-- sec to play with header control";
    _ChangeInfoText("HEADER_ADDItem");
    HEADER_AddItem(_hHeader, 100, "Red", GUI_TA_VCENTER | GUI_TA_HCENTER); /* 添加项目Red */
    HEADER_AddItem(_hHeader, 0, "Green", GUI_TA_VCENTER | GUI_TA_HCENTER); /* 添加项目Green */
    HEADER_AddItem(_hHeader, 0, ":-)", GUI_TA_VCENTER | GUI_TA_HCENTER);   /* 添加项目:- */
    GUI_Delay(750);

    _ChangeInfoText("HEADER_SetItemWidth");                             /* 调整项目1宽度 */
    HEADER_SetItemWidth(_hHeader, 1, 60);
    GUI_Delay(750);

    _ChangeInfoText("HEADER_SetItemText");                              /* 调整项目2宽度 */
    HEADER_SetItemWidth(_hHeader, 2, 100);
    HEADER_SetItemText(_hHeader, 2, "Blue");
    GUI_Delay(750);

    _ChangeInfoText("HEADER_SetFont");
    HEADER_SetFont(_hHeader, &GUI_Font8x8);                             /* 设置字体 */
    GUI_Delay(750);

    _ChangeInfoText("HEADER_SetHeight");                                /* 设置高度 */
    HEADER_SetHeight(_hHeader, 50);
    GUI_Delay(750);

    _ChangeInfoText("HEADER_SetTextColor");
    HEADER_SetTextColor(_hHeader, GUI_YELLOW);                          /* 设置字体颜色 */
    GUI_Delay(750);

    _ChangeInfoText("HEADER_SetBkColor");
    HEADER_SetBkColor(_hHeader, GUI_DARKGRAY);                          /* 设置背景颜色 */
    GUI_Delay(750);

    _ChangeInfoText("HEADER_SetTextAlign");
    HEADER_SetTextAlign(_hHeader, 0, GUI_TA_HCENTER);                   /* 设置对齐方式 */

    while (Cnt > 0)
    {
        acInfoText[0] = '0' + (Cnt / 10);
        acInfoText[1] = '0' + (Cnt-- % 10);
        _ChangeInfoText(acInfoText);
        GUI_Delay(1000);
    }
}

/**
 * @brief       在FRAMEWIN控件中使用header
 * @param       无
 * @retval      无
 */
static void _DemoHeaderFrameWin(void)
{
    FRAMEWIN_Handle hFrameWin;
    _ChangeMainText("HEADER control inside a FRAMEWIN");
    hFrameWin = FRAMEWIN_Create("Title", _cbWindow, WM_CF_SHOW, 10, 80, 300, 140); /* 创建FRAMEWIN  */
    FRAMEWIN_SetActive(hFrameWin, 1);                                              /* 激活FRAMEWIN  */
    _hHeader = HEADER_CreateAttached(WM_GetClientWindow(hFrameWin), 1234, 0);      /* 创建附加于FRAMEWIN的HEADER小工具 */
    _Demo();
    WM_DeleteWindow(hFrameWin);
}

/**
 * @brief       在窗口中使用header
 * @param       无
 * @retval      无
 */
static void _DemoHeaderWin(void)
{
    WM_HWIN hWin;
    _ChangeMainText("HEADER control inside a window");
    hWin = WM_CreateWindow(10, 80, 300, 140, WM_CF_SHOW, _cbWindow, 0);     /* 创建窗口 */
    _hHeader = HEADER_CreateAttached(hWin, 1234, 0);                        /* 创建附加于窗口的HEADER小工具 */
    _Demo();
    WM_DeleteWindow(hWin);                                                  /* 删除窗口 */
}

/**
 * @brief       直接使用HEADER
 * @param       无
 * @retval      无
 */
static void _DemoHeader(void)
{
    _ChangeMainText("HEADER control without parent");
    _hHeader = HEADER_Create(10, 80, 300, 0, 0, 1234, WM_CF_SHOW, 0);       /* 创建HEADER */
    _Demo();
    WM_DeleteWindow(_hHeader);
}

/**
 * @brief       emwin_main(emwin入口)
 * @param       无
 * @return      无
 */
void emwin_main(void)
{
    WM_EnableMemdev(WM_HBKWIN);             /* 使能存储设备 */
    WM_SetCreateFlags(WM_CF_MEMDEV);
    WM_SetCallback(WM_HBKWIN, _cbBkWindow); /* 设置背景窗口回掉函数 */
    GUI_CURSOR_Show();

    while (1)                               /* Demo演示*/
    {
        _DemoHeaderFrameWin();
        _DemoHeaderWin();
        _DemoHeader();
    }
}
