/**
 ****************************************************************************************************
 * @file        emwin_demo.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-01-04
 * @brief       emWin的CONVIEW部件使用实验
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
#include "iconviewdemo.h"
#include "iconbitmap.h"
#include "background.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"
#include "DIALOG.h"
#include "FRAMEWIN.h"


#define ID_ICONVIEW_0   (GUI_ID_USER + 0x01)

WM_HWIN IconviewWin;    /* ICONVIEW控件窗口 */

/* app图标描述结构体 */
typedef struct{
    const GUI_BITMAP    *pBitmap;   /* app图标 */
    const char          *pText;     /* app图标名字 */
}BITMAP_ITEM;

/* ICONVIEW显示的APP图标 */
static const BITMAP_ITEM BitmapItemTab1[]={
    {&bmbag,            "Bag"},
    {&bmbook,           "Book"},
    {&bmbookshelf,      "Booksh"},
    {&bmbrowser,        "Brower"},
    {&bmcalendar,       "Calendar"},
    {&bmclipboard,      "Clipboard"},
    {&bmcompass,        "Compass"},
    {&bmcompose,        "Cmpose"},
    {&bmdocument,       "Cument"},
    {&bmfolder,         "Folder"},
    {&bmmail,           "Mail"},
    {&bmpaint,          "Paint"},
    {&bmpin,            "Pin"},
    {&bmsettings,       "Set"},
};

/**
 * @brief       桌面窗口WM_HBKWIN回调函数
 * @param       pMsg:消息结构体
 * @return      无
 */
void _cb_BkWindow(WM_MESSAGE *pMsg)
{
    int Id;
    int NCode;
    static int Iconview0_Sel;
    
    switch(pMsg->MsgId)
    {
        case WM_PAINT:
            GUI_SetBkColor(GUI_BLACK);
            GUI_Clear();
            GUI_DrawBitmap(&bmbackground,0,0);  /* 绘制背景图片 */
            break;
        case WM_NOTIFY_PARENT:
            Id = WM_GetId(pMsg->hWinSrc);
            NCode = pMsg->Data.v;
            switch(Id)
            {
                case ID_ICONVIEW_0: /* ICONVIEW0控件 */
                    switch(NCode)   /* 通知代码 */
                    {
                        case WM_NOTIFICATION_CLICKED:
                            break;
                        case WM_NOTIFICATION_RELEASED:
                            Iconview0_Sel = ICONVIEW_GetSel(pMsg->hWinSrc);
                            switch(Iconview0_Sel)
                            {
                                case 0: /* APP0 */
                                    if(BagAPPWin == NULL)CreateBagAPP();          /* 创建BagAPP */
                                    break;
                                case 1: /* APP1 */
                                    if(BookAPPWin == NULL)CreateBookAPP();        /* 创建BookAPP */
                                    break;
                                case 2: /* APP2 */
                                    if(BookshAPPWin == NULL)CreateBookshAPP();    /* 创建BookshAPP */
                                    break;
                                case 3: /* APP3 */
                                    if(BrowerAPPWin == NULL)CreateBrowerAPP();    /* 创建BrowerAPP */
                                    break;
                            }
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
    int i;
    
    /* 设置换肤  */
    PROGBAR_SetDefaultSkin(PROGBAR_SKIN_FLEX);
    FRAMEWIN_SetDefaultSkin(FRAMEWIN_SKIN_FLEX);
    PROGBAR_SetDefaultSkin(PROGBAR_SKIN_FLEX);
    BUTTON_SetDefaultSkin(BUTTON_SKIN_FLEX);
    CHECKBOX_SetDefaultSkin(CHECKBOX_SKIN_FLEX);
    DROPDOWN_SetDefaultSkin(DROPDOWN_SKIN_FLEX);
    SCROLLBAR_SetDefaultSkin(SCROLLBAR_SKIN_FLEX);
    SLIDER_SetDefaultSkin(SLIDER_SKIN_FLEX);
    HEADER_SetDefaultSkin(HEADER_SKIN_FLEX);
    RADIO_SetDefaultSkin(RADIO_SKIN_FLEX);
    MULTIPAGE_SetDefaultSkin(MULTIPAGE_SKIN_FLEX);
    
    WM_SetCallback(WM_HBKWIN,_cb_BkWindow); /* 设置桌面窗口WM_HBKWIN的回调函数 */
    /* 建立一个ICONVIEW作为主界面 */
    IconviewWin = ICONVIEW_CreateEx(  0,0,                          /* 左上角坐标 */
                                    800,250,                        /* 小工具的水平和垂直尺寸 */
                                    WM_HBKWIN,                      /* 父窗口为背景窗口 */
                                    WM_CF_SHOW|WM_CF_HASTRANS,      
                                    ICONVIEW_CF_AUTOSCROLLBAR_V,    /* 自动增加垂直滚动条 */
                                    ID_ICONVIEW_0,                  /* 小工具ID */
                                    100,                            /* 图标的水平尺寸为100 */
                                    100);                           /* 图标的垂直尺寸为100 */
    ICONVIEW_SetFont(IconviewWin,&GUI_Font16_ASCII);
    ICONVIEW_SetBkColor(IconviewWin,ICONVIEW_CI_SEL,GUI_WHITE|0X70000000);
    ICONVIEW_SetSpace(IconviewWin,GUI_COORD_X,15);
    ICONVIEW_SetSpace(IconviewWin,GUI_COORD_Y,15);
    ICONVIEW_SetIconAlign(IconviewWin,ICONVIEW_IA_HCENTER|ICONVIEW_IA_VCENTER);
    
    for(i = 0; i < (GUI_COUNTOF(BitmapItemTab1)); i ++)
    {
        ICONVIEW_AddBitmapItem(IconviewWin,BitmapItemTab1[i].pBitmap,BitmapItemTab1[i].pText);
    }
    
    while(1)
    {
        GUI_Delay(100);
    }
}
