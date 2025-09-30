/**
 ****************************************************************************************************
 * @file        emwin_demo.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-01-04
 * @brief       emWin的XBF格式汉字显示实验
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
#include "xbffontcreate.h"
/*****************************************************************************************************/
/*uC-OS3*/
#include "os.h"
#include "cpu.h"


#define ID_FRAMEWIN_0     (GUI_ID_USER + 0)
#define ID_MULTIEDIT_0    (GUI_ID_USER + 1)
#define ID_BUTTON_0       (GUI_ID_USER + 2)
#define ID_BUTTON_1       (GUI_ID_USER + 3)
#define ID_CHECKBOX_0     (GUI_ID_USER + 4)
#define ID_CHECKBOX_1     (GUI_ID_USER + 5)
#define ID_CHECKBOX_2     (GUI_ID_USER + 6)
#define ID_CHECKBOX_3     (GUI_ID_USER + 7)
#define ID_RADIO_0        (GUI_ID_USER + 8)
#define ID_LISTVIEW_0     (GUI_ID_USER + 9)
#define ID_DROPDOWN_0     (GUI_ID_USER + 10)
#define ID_TEXT_0         (GUI_ID_USER + 11)
#define ID_TEXT_1         (GUI_ID_USER + 12)
#define ID_TEXT_2         (GUI_ID_USER + 13)
#define ID_TEXT_3         (GUI_ID_USER + 14)
#define ID_SCROLLBAR_0    (GUI_ID_USER + 15)
#define ID_SLIDER_0       (GUI_ID_USER + 16)

WM_HWIN hDialog;

const char *_ListViewTable[][4]={
    {"吴总",    "20141226001","97","A"},
    {"李四",    "20141226002","84","B"},
    {"刘翔",    "20141226003","73","C"},
    {"赵子龙",    "20141226004","85","B"},
    {"刘备",    "20141226005","45","E"},
    {"欧阳夏雨","20141226006","98","A"},
    {"王子奇",    "20141226007","84","B"},
    {"孙菲菲",    "20141226008","65","D"},
};

char *MultiEdit_String    =  
"STM32战舰开发板\r\n        \
CPU:STM32F103\r\n        \
FLASH:512K\r\n            \
RAM:64K\r\n            \
板载:10M/100M以太网PHY芯片\r\n    \
     WM8978音频DAC    ";
    
/* 对话框资源列表 */
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
    { FRAMEWIN_CreateIndirect,     "Framewin",     ID_FRAMEWIN_0,     0,       0,       800,     480,     0, FRAMEWIN_CF_MOVEABLE, 0 },
    { MULTIEDIT_CreateIndirect,    "Multiedit",    ID_MULTIEDIT_0,    20,      20,      300,     160,     0, 0x0,    0 },
    { BUTTON_CreateIndirect,       "BUTTON",       ID_BUTTON_0,       398,     370,     140,     50,      0, 0x0,    0 },
    { BUTTON_CreateIndirect,       "BUTTON",       ID_BUTTON_1,       599,     370,     140,     50,      0, 0x0,    0 },
    { CHECKBOX_CreateIndirect,     "Checkbox",     ID_CHECKBOX_0,     530,     90,      120,     30,      0, 0x0,    0 },
    { CHECKBOX_CreateIndirect,     "Checkbox",     ID_CHECKBOX_1,     530,     130,     120,     30,      0, 0x0,    0 },
    { CHECKBOX_CreateIndirect,     "Checkbox",     ID_CHECKBOX_2,     530,     170,     120,     30,      0, 0x0,    0 },
    { CHECKBOX_CreateIndirect,     "Checkbox",     ID_CHECKBOX_3,     530,     210,     120,     30,      0, 0x0,    0 },
    { RADIO_CreateIndirect,        "Radio",        ID_RADIO_0,        650,     119,     80,      87,      0, 0x1404, 0 },
    { LISTVIEW_CreateIndirect,     "Listview",     ID_LISTVIEW_0,     20,      200,     300,     194,     0, 0x0,    0 },
    { DROPDOWN_CreateIndirect,     "Dropdown",     ID_DROPDOWN_0,     324,     22,      150,     100,     0, 0x0,    0 },
    { TEXT_CreateIndirect,         "Text0",        ID_TEXT_0,         327,     180,     150,     20,      0, 0x64,   0 },
    { TEXT_CreateIndirect,         "Text1",        ID_TEXT_1,         327,     200,     150,     20,      0, 0x64,   0 },
    { TEXT_CreateIndirect,         "Text2",        ID_TEXT_2,         327,     220,     200,     30,      0, 0x64,   0 },
    { TEXT_CreateIndirect,         "Text3",        ID_TEXT_3,         327,     250,     300,     50,      0, 0x64,   0 },
    { SCROLLBAR_CreateIndirect,    "Scrollbar",    ID_SCROLLBAR_0,    482,     310,     260,     40,      0, 0x0,    0 },
    { SLIDER_CreateIndirect,       "Slider",       ID_SLIDER_0,       513,     18,      259,     38,      0, 0x0,    0 },
};


/**
 * @brief       对话框窗口回调函数
 * @param       pMsg:消息
 * @retval      无
 */
static void _cbDialog(WM_MESSAGE * pMsg) 
{
    WM_HWIN hItem;
    WM_HWIN hHeader;
    int     NCode;
    int     Id;
    int     i;
    switch (pMsg->MsgId) 
    {
        case WM_INIT_DIALOG:
            
            /* 初始化FRAMEWIN */
            hItem = pMsg->hWin;
            FRAMEWIN_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
            FRAMEWIN_SetTitleHeight(hItem, 30);
            FRAMEWIN_SetFont(hItem,&XBF16_Font);
            FRAMEWIN_SetTextColor(hItem, GUI_RED);
            FRAMEWIN_SetText(hItem, "ALIENTEK XBF 汉字显示");
        
            /* 初始化MULTIEDIT */
            hItem = WM_GetDialogItem(pMsg->hWin, ID_MULTIEDIT_0);
            MULTIEDIT_SetTextColor(hItem,MULTIEDIT_CI_EDIT,GUI_BLACK);
            MULTIEDIT_SetBkColor(hItem,MULTIEDIT_CI_EDIT,GUI_LIGHTCYAN);
            MULTIEDIT_SetFont(hItem,&XBF16_Font);
            MULTIEDIT_SetText(hItem,MultiEdit_String);
            MULTIEDIT_EnableBlink(hItem,500,1);
        
            /* 初始化BUTTON */
            hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
            BUTTON_SetFont(hItem,&XBF16_Font);
            BUTTON_SetText(hItem,"确定");
            hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
            BUTTON_SetFont(hItem,&XBF16_Font);
            BUTTON_SetText(hItem,"取消");
        
            /* 初始化CHECKBOX */
            hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_0);
            CHECKBOX_SetFont(hItem,&XBF12_Font);
            CHECKBOX_SetText(hItem, "复选框 1");
            hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_1);
            CHECKBOX_SetFont(hItem,&XBF12_Font);
            CHECKBOX_SetText(hItem, "复选框 2");
            hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_2);
            CHECKBOX_SetFont(hItem,&XBF12_Font);
            CHECKBOX_SetText(hItem, "复选框 3");
            hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_3);
            CHECKBOX_SetFont(hItem,&XBF12_Font);
            CHECKBOX_SetText(hItem, "复选框 4");
        
            /* 初始化RADIO */
            hItem = WM_GetDialogItem(pMsg->hWin, ID_RADIO_0);
            RADIO_SetFont(hItem,&XBF12_Font);
            RADIO_SetText(hItem, "选项 1", 0);
            RADIO_SetText(hItem, "选项 2", 1);
            RADIO_SetText(hItem, "选项 3", 2);
            RADIO_SetText(hItem, "选项 4", 3);
  
            /* 初始化LISTVIEW */
            hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0);
            hHeader = LISTVIEW_GetHeader(hItem);
            LISTVIEW_SetHeaderHeight(hItem, 30);
            HEADER_SetFont(hHeader,&XBF12_Font);
            LISTVIEW_AddColumn(hItem, 40, "名字", GUI_TA_HCENTER | GUI_TA_VCENTER);
            LISTVIEW_AddColumn(hItem, 40, "学号", GUI_TA_HCENTER | GUI_TA_VCENTER);
            LISTVIEW_AddColumn(hItem, 40, "成绩", GUI_TA_HCENTER | GUI_TA_VCENTER);
            LISTVIEW_AddColumn(hItem, 40, "等级", GUI_TA_HCENTER | GUI_TA_VCENTER);
            LISTVIEW_SetGridVis(hItem, 1);
            LISTVIEW_SetAutoScrollH(hItem, 1);
            LISTVIEW_SetAutoScrollV(hItem, 1);
            LISTVIEW_SetRowHeight(hItem, 20);
            LISTVIEW_SetFont(hItem,&XBF16_Font);
            LISTVIEW_SetBkColor(hItem,LISTVIEW_CI_UNSEL,GUI_ORANGE);
            for(i=0;i<GUI_COUNTOF(_ListViewTable);i++)
            {
                LISTVIEW_AddRow(hItem,_ListViewTable[i]);
            }
        

            /* 初始化DROPDOWN */
            hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_0);
            DROPDOWN_SetAutoScroll(hItem,1);/* 开启自动滚动条 */
            DROPDOWN_SetFont(hItem,&XBF16_Font);
            DROPDOWN_AddString(hItem, "汉语");
            DROPDOWN_AddString(hItem, "英语");
            DROPDOWN_AddString(hItem, "韩语");
            DROPDOWN_AddString(hItem, "马来西亚语");
            DROPDOWN_AddString(hItem, "西班牙语");
            DROPDOWN_AddString(hItem, "其他语言......");
 
            /* 初始化TEXT */
            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
            TEXT_SetFont(hItem,&XBF12_Font);
            TEXT_SetTextColor(hItem,GUI_YELLOW);
            TEXT_SetText(hItem, "广州星翼电子");
            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
            TEXT_SetFont(hItem,&XBF16_Font);
            TEXT_SetTextColor(hItem,GUI_MAGENTA);
            TEXT_SetText(hItem, "广州星翼电子");
            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_2);
            TEXT_SetFont(hItem,&XBF24_Font);
            TEXT_SetTextColor(hItem,GUI_TRANSPARENT);
            TEXT_SetText(hItem, "广州星翼电子");
            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_3);
            TEXT_SetFont(hItem,&XBF36_Font);
            TEXT_SetTextColor(hItem,GUI_BLUE);
            TEXT_SetText(hItem, "广州星翼电子");
            break;
        case WM_NOTIFY_PARENT:
            Id = WM_GetId(pMsg->hWinSrc);
            NCode = pMsg->Data.v;
            switch(Id) 
            {
                case ID_MULTIEDIT_0: 
                    switch(NCode) 
                    {
                        case WM_NOTIFICATION_CLICKED:
                        
                            break;
                        case WM_NOTIFICATION_RELEASED:
                            
                            break;
                        case WM_NOTIFICATION_VALUE_CHANGED:
                            
                        break;
                    }
                    break;
                case ID_BUTTON_0: 
                    switch(NCode) 
                    {
                        case WM_NOTIFICATION_RELEASED:
                        
                            break;
                    }
                    break;
                case ID_BUTTON_1:
                    switch(NCode) 
                    {
                        case WM_NOTIFICATION_RELEASED:
                            
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
 * @brief       窗口回掉函数
 * @param       pMsg:消息
 * @retval      无
 */
static void _cbBkWindow(WM_MESSAGE *pMsg)
{
    switch(pMsg->MsgId)
    {
        case WM_PAINT:
            GUI_SetBkColor(GUI_BLUE);
            GUI_Clear();
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
    Create_XBF12((uint8_t *)"0:/SYSTEM/EMWINFONT/XBF/XBF12.xbf");
    Create_XBF16((uint8_t *)"0:/SYSTEM/EMWINFONT/XBF/XBF16.xbf");
    Create_XBF24((uint8_t *)"0:/SYSTEM/EMWINFONT/XBF/XBF24.xbf");
    Create_XBF36((uint8_t *)"0:/SYSTEM/EMWINFONT/XBF/XBF36.xbf");
    GUI_UC_SetEncodeUTF8();
    
    /* 更换皮肤 */
    BUTTON_SetDefaultSkin(BUTTON_SKIN_FLEX); 
    CHECKBOX_SetDefaultSkin(CHECKBOX_SKIN_FLEX);
    DROPDOWN_SetDefaultSkin(DROPDOWN_SKIN_FLEX);
    FRAMEWIN_SetDefaultSkin(FRAMEWIN_SKIN_FLEX);
    HEADER_SetDefaultSkin(HEADER_SKIN_FLEX);
    MENU_SetDefaultSkin(MENU_SKIN_FLEX);
    MULTIPAGE_SetDefaultSkin(MULTIPAGE_SKIN_FLEX);
    PROGBAR_SetDefaultSkin(PROGBAR_SKIN_FLEX);
    RADIO_SetDefaultSkin(RADIO_SKIN_FLEX);
    SCROLLBAR_SetDefaultSkin(SCROLLBAR_SKIN_FLEX);
    SLIDER_SetDefaultSkin(SLIDER_SKIN_FLEX);
    SPINBOX_SetDefaultSkin(SPINBOX_SKIN_FLEX);
    
    WM_SetCallback(WM_HBKWIN,_cbBkWindow);      /* 设置背景窗口回调函数 */
    GUI_CURSOR_Show();
    #if GUI_SUPPORT_MEMDEV
        WM_SetCreateFlags(WM_CF_MEMDEV);
    #endif
    
    hDialog = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
    GUI_Exec();
    
    while(1)
    {
        GUI_Delay(100);
    }
}
