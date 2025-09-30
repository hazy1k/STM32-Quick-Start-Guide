/**
 ****************************************************************************************************
 * @file        iconviewdemo.c
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

#include "iconviewdemo.h"
#include "iconbitmap.h"
#include "background.h"
#include "DIALOG.h"
#include "FRAMEWIN.h"
#include "emwin_demo.h"


/* BagAPP内部小控件ID */
#define ID_FRAMEWIN_0   (GUI_ID_USER + 0x02)
#define ID_TEXT_0       (GUI_ID_USER + 0x03)

/* BookAPP内部小控件ID */
#define ID_FRAMEWIN_1   (GUI_ID_USER + 0x04)
#define ID_TEXT_1       (GUI_ID_USER + 0x05)

/* BookshAPP内部小控件ID */
#define ID_FRAMEWIN_2   (GUI_ID_USER + 0x06)
#define ID_TEXT_2       (GUI_ID_USER + 0x07)

/* BrowerAPP内部小控件ID */
#define ID_FRAMEWIN_3   (GUI_ID_USER + 0x08)
#define ID_TEXT_3       (GUI_ID_USER + 0x09)

WM_HWIN BagAPPWin=NULL;         /* BagAPP窗口 */
WM_HWIN BookAPPWin=NULL;        /* BookAPP窗口 */
WM_HWIN BookshAPPWin=NULL;      /* BookshAPP窗口 */
WM_HWIN BrowerAPPWin=NULL;      /* BrowerAPP窗口 */

/* BagAPP资源表 */
static const GUI_WIDGET_CREATE_INFO BagAPPDialogCreate[]=
{
    { FRAMEWIN_CreateIndirect, "Framewin", ID_FRAMEWIN_0, 0, 0, 800, 480,0,0x64},
    { TEXT_CreateIndirect, "Text", ID_TEXT_0, 200, 200, 200, 20, 0, 0x64, 0 },
};

/* BookAPP资源表 */
static const GUI_WIDGET_CREATE_INFO BookAPPDialogCreate[]=
{
    { FRAMEWIN_CreateIndirect, "Framewin", ID_FRAMEWIN_1, 0, 0, 320, 240,FRAMEWIN_CF_MOVEABLE,0x64},
    { TEXT_CreateIndirect, "Text", ID_TEXT_1, 70,85, 200, 20, 0, 0x64, 0 },
};

/* BookshAPP资源表 */
static const GUI_WIDGET_CREATE_INFO BookshAPPDialogCreate[]=
{
    { FRAMEWIN_CreateIndirect, "Framewin", ID_FRAMEWIN_2, 0, 0, 480, 272,FRAMEWIN_CF_MOVEABLE,0x64},
    { TEXT_CreateIndirect, "Text", ID_TEXT_2,150,100, 200, 20, 0, 0x64, 0 },
};

/* BrowerAPP资源表 */
static const GUI_WIDGET_CREATE_INFO BrowerAPPDialogCreate[]=
{
    { FRAMEWIN_CreateIndirect, "Framewin", ID_FRAMEWIN_3, 80, 0, 640, 480,0,0x64},
    { TEXT_CreateIndirect, "Text", ID_TEXT_3,200, 230, 200, 20, 0, 0x64, 0 },
};

/**
 * @brief       BagAPP的回调函数
 * @param       pMsg:消息结构体
 * @return      无
 */
static void _cbBagAPPDialog(WM_MESSAGE * pMsg) 
{
    WM_HWIN hItem;
    int     NCode;
    switch (pMsg->MsgId) 
    {
        case WM_INIT_DIALOG:
            /* 初始化FRAMWIN */
            hItem=pMsg->hWin;
            FRAMEWIN_SetText(hItem, "Bag APP");
            FRAMEWIN_SetFont(hItem,GUI_FONT_20_ASCII);
            FRAMEWIN_SetTextAlign(hItem, GUI_TA_HCENTER|GUI_TA_VCENTER);
            FRAMEWIN_AddCloseButton(hItem,FRAMEWIN_BUTTON_RIGHT,2); /* 添加关闭按钮 */
            FRAMEWIN_AddMaxButton(hItem,FRAMEWIN_BUTTON_RIGHT,2);   /* 添加最大化按钮 */
            FRAMEWIN_AddMinButton(hItem,FRAMEWIN_BUTTON_RIGHT,2);   /* 添加最小化按钮 */
        
            /* 初始化TEXT */
            hItem=WM_GetDialogItem(pMsg->hWin,ID_TEXT_0);
            TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
            TEXT_SetText(hItem, "This is Bag APP");
            TEXT_SetTextAlign(hItem,GUI_TA_HCENTER|GUI_TA_VCENTER);
            break;
        case WM_NOTIFY_PARENT:
            NCode=pMsg->Data.v;
            if(NCode==WM_NOTIFICATION_CHILD_DELETED) BagAPPWin=0;/* 子窗口删除了 */
            break;
        default:
            WM_DefaultProc(pMsg);
  }
}

/**
 * @brief       BookAPP的回调函数
 * @param       pMsg:消息结构体
 * @return      无
 */
static void _cbBookAPPDialog(WM_MESSAGE * pMsg) 
{
    WM_HWIN hItem;
    int     NCode;
    switch (pMsg->MsgId) 
    {
        case WM_PAINT:
            GUI_SetBkColor(GUI_RED);
            GUI_Clear();
            break;
        case WM_INIT_DIALOG:
            /* 初始化FRAMWIN */
            hItem=pMsg->hWin;
            FRAMEWIN_SetText(hItem, "Book APP");
            FRAMEWIN_SetFont(hItem,GUI_FONT_20_ASCII);
            FRAMEWIN_SetTextAlign(hItem, GUI_TA_HCENTER|GUI_TA_VCENTER);
            FRAMEWIN_AddCloseButton(hItem,FRAMEWIN_BUTTON_RIGHT,2); /* 添加关闭按钮 */
            FRAMEWIN_AddMaxButton(hItem,FRAMEWIN_BUTTON_RIGHT,2);   /* 添加最大化按钮 */
            FRAMEWIN_AddMinButton(hItem,FRAMEWIN_BUTTON_RIGHT,2);   /* 添加最小化按钮 */
        
            /* 初始化TEXT */
            hItem=WM_GetDialogItem(pMsg->hWin,ID_TEXT_1);
            TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
            TEXT_SetText(hItem, "This is Book APP");
            TEXT_SetTextAlign(hItem,GUI_TA_HCENTER|GUI_TA_VCENTER);
            break;
        case WM_NOTIFY_PARENT:
            NCode=pMsg->Data.v;
            if(NCode==WM_NOTIFICATION_CHILD_DELETED) BookAPPWin=0;/* 子窗口删除了 */
            break;
        default:
            WM_DefaultProc(pMsg);
  }
}

/**
 * @brief       BookshAPP的回调函数
 * @param       pMsg:消息结构体
 * @return      无
 */
static void _cbBookshAPPDialog(WM_MESSAGE * pMsg) 
{
    WM_HWIN hItem;
    int     NCode;
    switch (pMsg->MsgId) 
    {
        case WM_PAINT:
            GUI_SetBkColor(GUI_BLUE);
            GUI_Clear();
            break;   
        case WM_INIT_DIALOG:
            /* 初始化FRAMWIN */
            hItem=pMsg->hWin;
            FRAMEWIN_SetText(hItem, "Booksh APP");
            FRAMEWIN_SetFont(hItem,GUI_FONT_20_ASCII);
            FRAMEWIN_SetTextAlign(hItem, GUI_TA_HCENTER|GUI_TA_VCENTER);
            FRAMEWIN_AddCloseButton(hItem,FRAMEWIN_BUTTON_RIGHT,2); /* 添加关闭按钮 */
            FRAMEWIN_AddMaxButton(hItem,FRAMEWIN_BUTTON_RIGHT,2);   /* 添加最大化按钮 */
            FRAMEWIN_AddMinButton(hItem,FRAMEWIN_BUTTON_RIGHT,2);   /* 添加最小化按钮 */
        
            /* 初始化TEXT  */
            hItem=WM_GetDialogItem(pMsg->hWin,ID_TEXT_2);
            TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
            TEXT_SetText(hItem, "This is Booksh APP");
            TEXT_SetTextAlign(hItem,GUI_TA_HCENTER|GUI_TA_VCENTER);
            break;
        case WM_NOTIFY_PARENT:
            NCode=pMsg->Data.v;
            if(NCode==WM_NOTIFICATION_CHILD_DELETED) BookshAPPWin=0; /* 子窗口删除了 */
            break;
        default:
            WM_DefaultProc(pMsg);
  }
}

/**
 * @brief       BrowerAPP的回调函数
 * @param       pMsg:消息结构体
 * @return      无
 */
static void _cbBrowerAPPDialog(WM_MESSAGE * pMsg) 
{
    WM_HWIN hItem;
    int     NCode;
    switch (pMsg->MsgId) 
    {
        case WM_PAINT:
            GUI_SetBkColor(GUI_YELLOW);
            GUI_Clear();
            break;  
        case WM_INIT_DIALOG:
            /* 初始化FRAMWIN */
            hItem=pMsg->hWin;
            FRAMEWIN_SetText(hItem, "Brower APP");
            FRAMEWIN_SetFont(hItem,GUI_FONT_20_ASCII);
            FRAMEWIN_SetTextAlign(hItem, GUI_TA_HCENTER|GUI_TA_VCENTER);
            FRAMEWIN_AddCloseButton(hItem,FRAMEWIN_BUTTON_RIGHT,2); /* 添加关闭按钮 */
            FRAMEWIN_AddMaxButton(hItem,FRAMEWIN_BUTTON_RIGHT,2);   /* 添加最大化按钮 */
            FRAMEWIN_AddMinButton(hItem,FRAMEWIN_BUTTON_RIGHT,2);   /* 添加最小化按钮 */
        
            /* 初始化TEXT */
            hItem=WM_GetDialogItem(pMsg->hWin,ID_TEXT_3);
            TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
            TEXT_SetText(hItem, "This is Brower APP");
            TEXT_SetTextAlign(hItem,GUI_TA_HCENTER|GUI_TA_VCENTER);
            break;
        case WM_NOTIFY_PARENT:
            NCode=pMsg->Data.v;
            if(NCode==WM_NOTIFICATION_CHILD_DELETED) BrowerAPPWin=0;/* 子窗口删除了 */
            break;
        default:
            WM_DefaultProc(pMsg);
  }
}

/**
 * @brief       创建BagAPP
 * @param       无
 * @return      无
 */
void CreateBagAPP(void) 
{
    BagAPPWin = GUI_CreateDialogBox(BagAPPDialogCreate,GUI_COUNTOF(BagAPPDialogCreate),_cbBagAPPDialog,WM_HBKWIN,0,0);
}

/**
 * @brief       创建BookAPP
 * @param       无
 * @return      无
 */
void CreateBookAPP(void) 
{
    BookAPPWin = GUI_CreateDialogBox(BookAPPDialogCreate,GUI_COUNTOF(BookAPPDialogCreate),_cbBookAPPDialog,WM_HBKWIN,0,0);
}

/**
 * @brief       创建BookshAPP
 * @param       无
 * @return      无
 */
void CreateBookshAPP(void) 
{
    BookshAPPWin = GUI_CreateDialogBox(BookshAPPDialogCreate,GUI_COUNTOF(BookshAPPDialogCreate),_cbBookshAPPDialog,WM_HBKWIN,0,0);
}

/**
 * @brief       创建BrowerAPP
 * @param       无
 * @return      无
 */
void CreateBrowerAPP(void) 
{
    BrowerAPPWin = GUI_CreateDialogBox(BrowerAPPDialogCreate,GUI_COUNTOF(BrowerAPPDialogCreate),_cbBrowerAPPDialog,WM_HBKWIN,0,0);
}
