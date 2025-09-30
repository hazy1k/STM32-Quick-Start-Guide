/**
 ****************************************************************************************************
 * @file        emwin_demo.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-01-04
 * @brief       emWin重绘背景实验
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
 * 修改说明
 * V1.0 20200417
 * 第一次发布
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
/*****************************************************************************************************/
/*uC-OS3*/
#include "os.h"
#include "cpu.h"


/**
 * @brief       背景窗口回调函数
 * @param       pMsg : 窗口
 * @return      无
 */
static void _cbBkWindow(WM_MESSAGE *pMsg)
{
    switch (pMsg->MsgId) /*根据不同的消息类型处理 */
    {
        case WM_PAINT:   /*WM_PAIMNT消息，窗口重绘自身 */
            GUI_ClearRect(0, 50, 319, 239);
            break;

        default:
            WM_DefaultProc(pMsg); /*此函数可处理未处理的消息 */
    }
}

/**
 * @brief       窗口回调函数
 * @param       pMsg : 窗口
 * @return      无
 */
static void _cbWindow(WM_MESSAGE *pMsg)
{
    GUI_RECT Rect;

    switch (pMsg->MsgId)
    {
        case WM_PAINT:
            WM_GetInsideRect(&Rect);     /* 返回活动窗口减去边界后的尺寸 */
            GUI_SetBkColor(GUI_RED);     /* 红色背景色  */
            GUI_SetColor(GUI_YELLOW);    /* 黄色 */
            GUI_ClearRectEx(&Rect);      /* 清除Rect区域,既绘制红色区域 */
            GUI_DrawRectEx(&Rect);       /* 绘制Rect区域,既绘制黄色框框 */
            GUI_SetColor(GUI_BLACK);     /* 黑色 */
            GUI_SetFont(&GUI_Font16_ASCII); /* 设置字体 */
            GUI_DispStringHCenterAt("Foreground Window", 75, 40);
            break;

        default:
            WM_DefaultProc(pMsg);
    }
}

/**
 * @brief       移动窗口
 * @param       pText : 传入文本
 * @return      无
 */
static void _MoveWindow(const char *pText)
{
    WM_HWIN hWnd;
    int i;
    /*创建前景窗口 */
    hWnd = WM_CreateWindow(10, 50, 150, 100, WM_CF_SHOW, _cbWindow, 0);
    GUI_Delay(500);

    /*移动前景窗口 */
    for (i = 0; i < 40; i++)
    {
        WM_MoveWindow(hWnd, 2, 2); /* 移动窗口 */
        GUI_Delay(10);
    }

    /* 删除窗口之前显示文本 */
    if (pText)
    {
        GUI_DispStringAt(pText, 5, 50);
        GUI_Delay(2500);
    }

    /* 删除前景窗口 */
    WM_DeleteWindow(hWnd);    /* 删除窗口*/
    WM_Invalidate(WM_HBKWIN); /* 背景窗口无效*/
    GUI_Exec();
}

/**
 * @brief       emwin_main(emwin入口)
 * @param       无
 * @return      无
 */
void emwin_main(void) 
{
    WM_CALLBACK *_cbOldBK;
    GUI_SetBkColor(GUI_BLACK);       /*设置背景颜色 */
    GUI_Clear();
    GUI_SetColor(GUI_WHITE);         /*白色 */
    GUI_SetFont(&GUI_Font24_ASCII);  /*设置字体 */
    GUI_DispStringHCenterAt("WM_Redraw-Sample", 160, 5);
    GUI_SetFont(&GUI_Font16B_ASCII); /*设置字体 */

    while (1)
    {
        _MoveWindow("Background has not been redraw"); //移动窗口  */

        /* 清除背景 */
        GUI_ClearRect(0, 50, 319, 239);
        GUI_Delay(1000);

        /* 为背景窗口设置回调函数 */
        _cbOldBK = WM_SetCallback(WM_HBKWIN, _cbBkWindow);

        /* 移动窗口 */
        _MoveWindow("Background has been redraw");

        /* 窗口WM_HBKWIN的回调函数重新设置为上一个回调函数 */
        WM_SetCallback(WM_HBKWIN, _cbOldBK);
    }
}
