/**
 ****************************************************************************************************
 * @file        emwin_demo.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-01-04
 * @brief       emWin的TEXT部件使用实验
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
#include "./BSP/RTC/rtc.h"
#include "./BSP/ADC/adc.h"
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
/*UCOSIII*/
#include "os.h"
#include "cpu.h"


/*控件ID */
#define ID_FRAMEWIN_0 (GUI_ID_USER + 0x00)
#define ID_TEXT_0 (GUI_ID_USER + 0x01)
#define ID_TEXT_1 (GUI_ID_USER + 0x02)
#define ID_TEXT_2 (GUI_ID_USER + 0x03)

WM_HWIN TextDialoghWin;     /*文本对话框窗口 */

/*对话框的资源表*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] =
{
    { FRAMEWIN_CreateIndirect, "Framewin", ID_FRAMEWIN_0, 0, 0, 320, 240, FRAMEWIN_CF_MOVEABLE, 0x64, 0 },
    { TEXT_CreateIndirect, "AD VALUE:", ID_TEXT_0, 20, 50, 120, 25, 0, 0x64, 0 },
    { TEXT_CreateIndirect, "DATE:",     ID_TEXT_1, 68, 80, 69, 20, 0, 0x64, 0 },
    { TEXT_CreateIndirect, "TIME:",     ID_TEXT_2, 75, 110, 65, 20, 0, 0x64, 0 },
};

/**
 * @brief       对话框回调函数
 * @param       pMsg : 消息
 * @retval      无
 */
static void _cbDialog(WM_MESSAGE *pMsg)
{
    WM_HWIN hItem;
    uint8_t hour,min,sec,ampm;
    uint8_t year,month,date,week;
    static uint16_t adcvalue = 0;
    uint8_t tbuf[40];

    switch (pMsg->MsgId)
    {
        case WM_PAINT:
            GUI_SetColor(GUI_RED);
            GUI_SetFont(GUI_FONT_24B_ASCII);

            /*显示AD值*/
            adcvalue = adc_get_result_average(ADC_CHANNEL_5, 10); /*获取AD值*/
            GUI_DispDecAt(adcvalue, 140, 50, 4);
            /*显示日期*/
            rtc_get_time(&hour, &min, &sec, &ampm);
            rtc_get_date(&year, &month, &date, &week);
            sprintf((char *)tbuf, "%04d-%02d-%02d", year, month, date);
            GUI_DispStringAt((const char *)tbuf, 140, 80); /*显示日期*/

            sprintf((char *)tbuf, "%02d:%02d:%02d", hour, min, sec);
            GUI_DispStringAt((const char *)tbuf, 140, 110); /*显示日期*/
        
            sprintf((char *)tbuf, "Week:%d", week);
            GUI_DispStringAt((const char *)tbuf,140,140);
            break;

        case WM_INIT_DIALOG:
            /*FRAMWIN初始化*/
            hItem = pMsg->hWin;
            FRAMEWIN_SetFont(hItem, GUI_FONT_20B_ASCII);
            FRAMEWIN_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
            FRAMEWIN_SetText(hItem, "ALIENTEK TEXT");
            FRAMEWIN_SetTextColor(hItem, 0x0000FFFF);

            /*TEXT初始化*/
            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
            TEXT_SetFont(hItem, GUI_FONT_24B_ASCII);
            TEXT_SetText(hItem, "AD VALUE:");

            /*日期初始化*/
            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
            TEXT_SetFont(hItem, GUI_FONT_24B_ASCII);
            TEXT_SetText(hItem, "DATE:");

            /*时间初始化*/
            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_2);
            TEXT_SetTextColor(hItem, 0x00000000);
            TEXT_SetFont(hItem, GUI_FONT_24B_ASCII);
            TEXT_SetText(hItem, "TIME:");
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
    WM_SetCallback(WM_HBKWIN, _cbBkWindow); /*设置背景窗口回调函数*/
    TextDialoghWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);

    while (1)
    {
        GUI_Delay(1000);
        WM_InvalidateWindow(TextDialoghWin); /*对话框窗口无效*/
    }
}
