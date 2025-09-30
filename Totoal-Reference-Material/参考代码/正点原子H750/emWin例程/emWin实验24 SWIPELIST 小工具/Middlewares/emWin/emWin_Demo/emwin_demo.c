/**
 ****************************************************************************************************
 * @file        emwin_demo.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-01-04
 * @brief       emWin的SWIPELIST部件使用实验
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
#include "./SYSTEM/usart/usart.h"
#include "music.h"
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


GUI_BITMAP image_tab[2];
#define COLOR_GREEN     GUI_MAKE_COLOR(0x14FF39)  /* 定义一个颜色 */
#define NUM_SEPARATORS  3                         /* 可用分隔器的数目 */
#define NUM_ITEMS       5                         /* 可用项目数量 */


/* 此数组包含关于每个项的信息 */
static const char *paItemText[3][5] =
{
    {
        "Brightness",
        "Mobile Data",
        "Bluetooth",
        "Hot Spot",
        "Mobil",
    },
    {
        "Wifi",
        "music",
        "Idle",
        "Night Mode",
        "Font Size",
    },
    {
        "Account",
        "Sound",
        "Memory",
        "Alert",
        "Clock",
    },
};

/* 关于标题的信息 */
static const char *paSeperatorText[] =
{
    "Display",
    "Network",
    "Settings"
};

/**
 * @brief       emwin_main(emwin入口)
 * @param       无
 * @return      无
 */
void emwin_main(void) 
{
    WM_HWIN hSwipelist;
    WM_HWIN hItem;
    int i;
    int j;
    int Xsize;
    int Ysize;
    
    Xsize = LCD_GetXSize();  /* 获取LCD的X大小 */
    Ysize = LCD_GetYSize();  /* 获取LCD的Y大小 */

    image_tab[0] = bmwifi;   /* 设置位图 */
    image_tab[1] = bmmusic;
  
    WM_MULTIBUF_Enable(1);  /* 自动使用多缓冲 */

    WM_MOTION_Enable(1);              /* 使能移动 */
    WM_SetBkWindowColor(GUI_BLUE);    /* 设置一个默认的背景颜色 */

    SWIPELIST_SetDefaultBkColor(SWIPELIST_CI_BK_ITEM_SEL, COLOR_GREEN);  /* 设置选定项目的颜色 */
    SWIPELIST_SetDefaultBkColor(SWIPELIST_CI_BK_SEP_ITEM, GUI_DARKGRAY); /* 设置分隔符的颜色 */
    

    /* 创建一个swipelist */
    hSwipelist = SWIPELIST_CreateEx(10, 10, Xsize/2, Ysize/2, WM_HBKWIN, WM_CF_SHOW, 0, 0);
    WM_MOTION_SetMoveable(hSwipelist, WM_CF_MOTION_Y, 1);
    
    for (j = 0; j < NUM_SEPARATORS; j++) /* 在每个分隔符下添加5个项目 */
    {
        SWIPELIST_AddSepItem(hSwipelist, paSeperatorText[j], 35);
        
      
        for (i = 0; i < NUM_ITEMS; i++)
        {
            SWIPELIST_AddItem(hSwipelist, paItemText[j][i], 35);  /* 添加一个新的项 */
            SWIPELIST_SetSepColor(hSwipelist,i,GUI_RED);          /* 设置项目之间的颜色线 */
        } 
       
    }

    for (i = 0; i < 5; i++) /* 将复选框附加到前五个项目 */
    {
        hItem = CHECKBOX_Create(0, 0, 15, 15, WM_HBKWIN, 0, WM_CF_SHOW);
        SWIPELIST_ItemAttachWindow(hSwipelist, 1 + i, hItem, Xsize/2-20, 10);
    }
    
    for (i = 0; i < 2; i++) /* 设置位图 */
    {
        SWIPELIST_SetBitmap(hSwipelist,i+7,SWIPELIST_BA_RIGHT,&image_tab[i]);
    }
    
    while(1)
    {
        GUI_Delay(200);
    }
}
