/**
 ****************************************************************************************************
 * @file        emwin_demo.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-01-04
 * @brief       emWin颜色条绘制实验
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


#define X_START 60  /* 颜色条开始X坐标 */
#define Y_START 40  /* 颜色条开始Y坐标 */

typedef struct {
    int NumBars;     /* 颜色的数量 */
    GUI_COLOR Color; /* 颜色 */
    const char * s;  /* 颜色的名称 */
} BAR_DATA;  


static const BAR_DATA _aBarData[] = {
    { 2, GUI_RED    , "Red" },
    { 2, GUI_GREEN  , "Green" },
    { 2, GUI_BLUE   , "Blue" },
    { 1, GUI_WHITE  , "Grey" },
    { 2, GUI_YELLOW , "Yellow" },
    { 2, GUI_CYAN   , "Cyan" },
    { 2, GUI_MAGENTA, "Magenta" },
};

static const GUI_COLOR _aColorStart[] = { GUI_BLACK, GUI_WHITE };  /* 定义两种颜色分别为黑色与白色 */

/**
 * @brief       显示颜色条
 * @param       无
 * @return      无
 */
static void emwin_show_colorbar(void) 
{
    GUI_RECT Rect;
    int      yStep;
    int      i;
    int      j;
    int      xSize;
    int      ySize;
    int      NumBars;    /* 要显示的颜色条数量2+2+2+1+2+2+2=13 */
    int      NumColors;  /* 要显示的颜色数量，这里有7个 */

    /* 获取显示屏尺寸 */
    xSize = LCD_GetXSize();
    ySize = LCD_GetYSize();

    /* 获取颜色条的数量 ，NumColors等于7 */
    NumColors = GUI_COUNTOF(_aBarData);
  
    for (i = NumBars = 0, NumBars = 0; i < NumColors; i++) 
    {
        NumBars += _aBarData[i].NumBars;  /* 获取_aBarData的颜色的数量 */
    }

    yStep = (ySize - Y_START) / NumBars;/* 获得显示颜色的平均值 */
   
    /* 显示文本 */
    Rect.x0 = 0;
    Rect.x1 = X_START - 1;
    Rect.y0 = Y_START;
    GUI_SetFont(&GUI_Font8x16);

    for (i = 0; i < NumColors; i++) 
    {
        Rect.y1 = Rect.y0 + yStep * _aBarData[i].NumBars - 1;
        GUI_DispStringInRect(_aBarData[i].s, &Rect, GUI_TA_LEFT | GUI_TA_VCENTER);  /* 显示文本 */
        Rect.y0 = Rect.y1 + 1;
    }
    
    /* 显示颜色 */
    Rect.x0 = X_START;
    Rect.x1 = xSize - 1;
    Rect.y0 = Y_START;

    for (i = 0; i < NumColors; i++) 
    {
        for (j = 0; j < _aBarData[i].NumBars; j++) 
        {
            Rect.y1 = Rect.y0 + yStep - 1;
            GUI_DrawGradientH(Rect.x0, Rect.y0, Rect.x1, Rect.y1, _aColorStart[j], _aBarData[i].Color); /* 显示颜色 */
            Rect.y0 = Rect.y1 + 1;
        }
    }
}

/**
 * @brief       emwin_main(emwin入口)
 * @param       无
 * @return      无
 */
void emwin_main(void)
{
    GUI_SetBkColor(GUI_BLUE);
    GUI_SetColor(GUI_YELLOW);
    GUI_Clear();
    
    GUI_SetFont(&GUI_Font24_ASCII);
    GUI_SetTextMode(GUI_TM_TRANS);  /*透明显示*/
    GUI_DispStringHCenterAt("COLOR_BAR TEST!",400,0);
    
    emwin_show_colorbar();            /*显示颜色条*/
}

