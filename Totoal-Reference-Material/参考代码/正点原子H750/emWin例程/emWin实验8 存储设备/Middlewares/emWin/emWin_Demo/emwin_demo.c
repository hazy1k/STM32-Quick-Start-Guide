/**
 ****************************************************************************************************
 * @file        emwin_demo.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-01-04
 * @brief       emWin存储设备实验
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


/* 定义一个区域 */
static GUI_RECT Rect = {200, 150, 300, 210}; 

/**
 * @brief       未使用存储设备绘制
 * @param       Delay : 延时
 * @return      无
 */
static void emwin_draw(int Delay) 
{
    GUI_SetPenSize(5);
    GUI_SetColor(GUI_RED);
    GUI_DrawLine(Rect.x0 + 3, Rect.y0 + 3, Rect.x1 - 3, Rect.y1 - 3);
    GUI_Delay(Delay);
    GUI_SetColor(GUI_GREEN);
    GUI_DrawLine(Rect.x0 + 3, Rect.y1 - 3, Rect.x1 - 3, Rect.y0 + 3);
    GUI_Delay(Delay);
    GUI_SetColor(GUI_WHITE);
    GUI_SetFont(&GUI_FontComic24B_ASCII);
    GUI_SetTextMode(GUI_TM_TRANS);
    GUI_DispStringInRect("Closed", &Rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
    GUI_Delay(Delay);
}

/**
 * @brief       emwin_main(emwin入口)
 * @param       无
 * @return      无
 */
void emwin_main(void) 
{
    GUI_MEMDEV_Handle hMem;    /* 存储设备的句柄 */
    int i;

    GUI_SetBkColor(GUI_BLUE);
    GUI_Clear();
    GUI_SetColor(GUI_YELLOW);
    GUI_SetFont(&GUI_Font24_ASCII);
    GUI_DispStringHCenterAt("MEMDEV_MemDev - Sample", 400, 0);
    GUI_SetFont(&GUI_Font20_ASCII);
    GUI_DispStringHCenterAt("Shows the advantage of using a\nmemorydevice", 400,50);
    GUI_SetFont(&GUI_Font16_ASCII);
    GUI_DispStringHCenterAt("Draws the picture\nwithout a\nmemory device", 250, 100);
    GUI_DispStringHCenterAt("Draws the picture\nusing a\nmemory device", 550, 100);
    

    
    hMem = GUI_MEMDEV_Create(Rect.x0, Rect.y0, Rect.x1 - Rect.x0, Rect.y1 - Rect.y0); /* 创建存储设备 */
    GUI_MEMDEV_Select(hMem);  /* 选择hMem作为绘制操作目标的存储设备 */
    emwin_draw(0);            /* 执行要显示的内容复制到存储设备中 */
    GUI_MEMDEV_Select(0);     /* 选中LCD */

    while (1) 
    {
        for (i = 0; i < 3; i++) 
        {
            GUI_Delay(250);
            GUI_ClearRect(LCD_GetXSize() - Rect.x1, Rect.y0, LCD_GetXSize(), Rect.y1);  /* 清除选中的屏幕 */
            GUI_Delay(250);
            GUI_MEMDEV_CopyToLCDAt(hMem, LCD_GetXSize() - Rect.x1, Rect.y0); /* 将存储设备的内容从内存复制到LCD的指定位置 */
        }

        GUI_Delay(500);  
      
        /*  未使用存储设备 */
        emwin_draw(400);
        GUI_Delay(400);
        GUI_ClearRect(0, 150, 799, 210);
    }
}
