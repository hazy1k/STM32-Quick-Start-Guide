/**
 ****************************************************************************************************
 * @file        emwin_demo.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-01-04
 * @brief       emWin的SLIDER部件使用实验
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
#include "./BSP/TIMER/gtim.h"
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
#define ID_FRAMEWIN_0   (GUI_ID_USER + 0x00)
#define ID_SLIDER_0     (GUI_ID_USER + 0x01)
#define ID_SLIDER_1     (GUI_ID_USER + 0x02)
#define ID_SLIDER_2     (GUI_ID_USER + 0x03)
#define ID_SLIDER_3     (GUI_ID_USER + 0x04)
#define ID_EDIT_0       (GUI_ID_USER + 0x05)
#define ID_EDIT_1       (GUI_ID_USER + 0x06)
#define ID_EDIT_2       (GUI_ID_USER + 0x07)
#define ID_TEXT_0       (GUI_ID_USER + 0x08)
#define ID_TEXT_1       (GUI_ID_USER + 0x09)
#define ID_TEXT_2       (GUI_ID_USER + 0x0A)
#define ID_TEXT_3       (GUI_ID_USER + 0x0B)
#define ID_TEXT_4       (GUI_ID_USER + 0x0C)
#define ID_TEXT_5       (GUI_ID_USER + 0x0D)

slider_structer slider_dev;

/* 对话框资源表 */
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] =
{
    { FRAMEWIN_CreateIndirect, "Framewin", ID_FRAMEWIN_0, 0,   0,   800, 480, 0, 0x64, 0 },
    { SLIDER_CreateIndirect,   "Slider",   ID_SLIDER_0,   90,  100, 400, 35,  0, 0x0,  0 },
    { SLIDER_CreateIndirect,   "Slider",   ID_SLIDER_1,   90,  165, 400, 35,  0, 0x0,  0 },
    { SLIDER_CreateIndirect,   "Slider",   ID_SLIDER_2,   90,  230, 400, 35,  0, 0x0,  0 },
    { SLIDER_CreateIndirect,   "Slider",   ID_SLIDER_3,   90,  360, 595, 32,  0, 0x0,  0 },
    { EDIT_CreateIndirect,     "Edit",     ID_EDIT_0,     500, 100, 55,  35,  0, 0x64, 0 },
    { EDIT_CreateIndirect,     "Edit",     ID_EDIT_1,     500, 165, 55,  35,  0, 0x64, 0 },
    { EDIT_CreateIndirect,     "Edit",     ID_EDIT_2,     500, 230, 55,  35,  0, 0x64, 0 },
    { TEXT_CreateIndirect,     "Text",     ID_TEXT_0,     311, 47,  202, 29,  0, 0x64, 0 },
    { TEXT_CreateIndirect,     "Text",     ID_TEXT_1,     268, 314, 279, 31,  0, 0x64, 0 },
    { TEXT_CreateIndirect,     "Text",     ID_TEXT_2,     31,  109, 51,  20,  0, 0x64, 0 },
    { TEXT_CreateIndirect,     "Text",     ID_TEXT_3,     -2,  175, 92,  20,  0, 0x64, 0 },
    { TEXT_CreateIndirect,     "Text",     ID_TEXT_4,     12,  239, 80,  20,  0, 0x64, 0 },
    { TEXT_CreateIndirect,     "Text",     ID_TEXT_5,     23,  366, 66,  20,  0, 0x64, 0 },
};

/**
 * @brief       窗口回掉函数
 * @param       pMsg:消息
 * @retval      无
 */
static void _cbBkWindow(WM_MESSAGE *pMsg)
{
    switch (pMsg->MsgId)
    {
        case WM_PAINT:
            GUI_SetBkColor(GUI_BLUE);
            GUI_Clear();

        default:
            WM_DefaultProc(pMsg);
    }
}

/**
 * @brief       绘制颜色框
 * @param       pMsg:消息
 * @retval      无
 */
void _draw_color_block(void)
{
    GUI_SetColor(slider_dev.slider_value[0]);
    GUI_FillRect(560, 100, 595, 135);
    GUI_SetColor(slider_dev.slider_value[1] << 8);
    GUI_FillRect(560, 165, 595, 200);
    GUI_SetColor((uint32_t)slider_dev.slider_value[2] << 16);
    GUI_FillRect(560, 230, 595, 265);

    GUI_SetColor(slider_dev.slider_value[0] | (((uint32_t)slider_dev.slider_value[1]) << 8) | (((uint32_t)slider_dev.slider_value[2]) << 16));
    GUI_FillRect(600, 100, 765, 265);

}

/**
 * @brief       根据SLIDER的值设置EDIT的值
 * @param       hDlg：窗口句柄
 * @param       Id:小工具ID
 * @retval      无
 */
void _set_edit_value(WM_HWIN hDlg, int Id)
{
    WM_HWIN hSlider;
    WM_HWIN hEdit;
    uint8_t index;

    index = Id - ID_SLIDER_0;
    hSlider = WM_GetDialogItem(hDlg, ID_SLIDER_0 + index);     /* 获取那个滑块在移动的句柄 */
    hEdit = WM_GetDialogItem(hDlg, ID_EDIT_0 + index);         /* 获取滑块对应的EDIT区域 */
    slider_dev.slider_value[index] = SLIDER_GetValue(hSlider); /* 获取滑块的值并存如结构体slider_value */
    EDIT_SetValue(hEdit, slider_dev.slider_value[index]);      /* 设置EDIT值 */
    /* 最后使客户端窗口无效 */
    WM_InvalidateWindow(WM_GetClientWindow(hDlg));
}

/**
 * @brief       对话框回调函数
 * @param       pMsg:消息
 * @retval      无
 */
static void _cbDialog(WM_MESSAGE *pMsg)
{
    WM_HWIN hItem;
    int     NCode;
    int     Id;
    uint8_t      i;

    switch (pMsg->MsgId)
    {
        case WM_PAINT:
            _draw_color_block();
            return;

        case WM_INIT_DIALOG:
            /* 初始化FRAMEWIN */
            hItem = pMsg->hWin;
            FRAMEWIN_SetText(hItem, "ALIENTEK SLIDER");
            FRAMEWIN_SetFont(hItem, GUI_FONT_24_ASCII);
            FRAMEWIN_SetTextColor(hItem, 0x0080FFFF);
            FRAMEWIN_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);

            /* 初始化TEXT0 */
            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
            TEXT_SetText(hItem, "RGB BLOCK CONTROL");
            TEXT_SetFont(hItem, GUI_FONT_COMIC18B_ASCII);
            TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
            /* 初始化TEXT1 */
            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
            TEXT_SetText(hItem, "LED1  BRIGHTNESS CONTROL");
            TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
            TEXT_SetFont(hItem, GUI_FONT_COMIC18B_ASCII);
            /* 初始化TEXT2 */
            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_2);
            TEXT_SetText(hItem, "RED:");
            TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
            TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
            /* 初始化TEXT3 */
            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_3);
            TEXT_SetText(hItem, "GREEN:");
            TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
            TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
            /* 初始化TEXT4 */
            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_4);
            TEXT_SetText(hItem, "BLUE:");
            TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
            TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
            /* 初始化TEXT5 */
            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_5);
            TEXT_SetText(hItem, "LED:");
            TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
            TEXT_SetFont(hItem, GUI_FONT_24_ASCII);

            /* 初始化EDIT0 */
            for (i = 0; i < 3; i++)   /* 设置3个EDIT控件 */
            {
                hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_0 + i); 
                EDIT_SetDecMode(hItem, slider_dev.slider_value[i], 0, 255, 0, 0); /* 设置十进制模式最大值为255 */
                EDIT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);/* EDIT的对齐模式 */
                EDIT_SetFont(hItem, GUI_FONT_24_ASCII); /* 设置字体 */
            }

            /* 初始化SLIDER */
            for (i = 0; i < 3; i++) /* 设置3个SLIDER控件 */
            {
                hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_0 + i);/* 获取控件的句柄 */
                SLIDER_SetRange(hItem, 0, 255);  /* 设置控件的最大值为255 */
                SLIDER_SetValue(hItem, slider_dev.slider_value[i]); /* 设置滑块的位置 */
            }

            hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_3); /* 获取ID_SLIDER_3控件句柄 */
            SLIDER_SetRange(hItem, 0, 5000); /* 设置最大值为300 */
            SLIDER_SetValue(hItem, 5000);    /* 设置滑块当前值为300，此时LED0最亮 */
            break;

        case WM_NOTIFY_PARENT:
            Id    = WM_GetId(pMsg->hWinSrc);
            NCode = pMsg->Data.v;

            switch (Id)
            {
                case ID_SLIDER_0:
                case ID_SLIDER_1:
                case ID_SLIDER_2:
                    switch (NCode)
                    {
                        case WM_NOTIFICATION_CLICKED:
                            break;

                        case WM_NOTIFICATION_RELEASED:
                            break;

                        case WM_NOTIFICATION_VALUE_CHANGED:
                            _set_edit_value(pMsg->hWin, Id);
                            break;
                    }

                case ID_SLIDER_3:
                    switch (NCode)
                    {
                        case WM_NOTIFICATION_CLICKED:
                            break;

                        case WM_NOTIFICATION_RELEASED:
                            break;

                        case WM_NOTIFICATION_VALUE_CHANGED:
                            hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_3);
                            slider_dev.slider_value[3] = SLIDER_GetValue(hItem);
                            
                            break;
                    }
            }

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
    slider_dev.slider_value[0] = 0;
    slider_dev.slider_value[1] = 127;
    slider_dev.slider_value[2] = 255;
    slider_dev.slider_value[3] = 5000;
    WM_SetCallback(WM_HBKWIN, _cbBkWindow);
    slider_dev.slider = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
    
    while(1)
    {
        GUI_Delay(100);
    }
}

