/**
 ****************************************************************************************************
 * @file        emwin_demo.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-01-04
 * @brief       emWin 2D绘画显示实验
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


/*(0.20).----------------.(40.20)
         \             /
          \           /
           \         /
            \       /
             \     /
              \   /
               \./(20.0)        */
const GUI_POINT aPoints[] =
{
    {40, 20},
    {0, 20},
    {20, 0}
};

const GUI_POINT aPoints1[] =
{
    {0, 20},
    {40, 20},
    {20, 0}
};

const GUI_POINT aPointArrow[] =
{
    { 0, -5},
    {-40, -35},
    {-10, -25},
    {-10, -85},
    { 10, -85},
    { 10, -25},
    { 40, -35},
};

GUI_POINT aEnlargedPoints[GUI_COUNTOF(aPoints)];   /* 多边形放大后坐标 */
GUI_POINT aMagnifiedPoints[GUI_COUNTOF(aPoints1)];


/**
 * @brief       绘制弧线 绘制简易仪表盘
 * @param       无
 * @return      无
 */
void emwin_drawarcscale(void)
{
    int x0 = 630;
    int y0 = 380;
    int i;
    char ac[4];
    GUI_SetPenSize( 5 );
    GUI_SetTextMode(GUI_TM_TRANS);
    GUI_SetFont(&GUI_FontComic18B_ASCII);
    GUI_SetColor(GUI_BLACK);
    GUI_DrawArc( x0, y0, 150, 150, -30, 210 );

    for (i = 0; i <= 23; i++)
    {
        float a = (-30 + i * 10) * 3.1415926 / 180;
        int x = -141 * cos(a) + x0;
        int y = -141 * sin(a) + y0;

        if (i % 2 == 0)
            GUI_SetPenSize( 5 );
        else
            GUI_SetPenSize( 4 );

        GUI_DrawPoint(x, y);

        if (i % 2 == 0)
        {
            x = -123 * cos(a) + x0;
            y = -130 * sin(a) + y0;
            sprintf(ac, "%d", 10 * i);
            GUI_SetTextAlign(GUI_TA_VCENTER);
            GUI_DispStringHCenterAt(ac, x, y);
        }
    }
}

/**
 * @brief       绘制线图
 * @param       无
 * @return      无
 */
void emwin_draw_graph(void)
{
    I16 aY[400];
    int i;

    for (i = 0; i < GUI_COUNTOF(aY); i++) aY[i] = rand() % 100;

    GUI_ClearRect(0, 300, 400, 400); /* 每次显示前先清除显示区域 */
    GUI_DrawGraph(aY, GUI_COUNTOF(aY), 0, 300);
}

/**
 * @brief       2D图形绘制
 * @param       无
 * @return      无
 */
void emwin_display_2d(void)
{
    int i;
    GUI_SetBkColor(GUI_BLUE);  /* 设置背景颜色 */
    GUI_Clear();

    GUI_SetColor(GUI_YELLOW);
    GUI_SetFont(&GUI_Font24_ASCII);
    GUI_DispStringHCenterAt("ALIENTEK 2D DISPALY", 400, 10);

    GUI_SetColor(GUI_RED);
    GUI_SetFont(&GUI_Font16_ASCII);

    GUI_SetBkColor(GUI_GREEN);
    GUI_ClearRect(10, 50, 100, 150); /* 在一个矩形区域填充绿色背景 */
    GUI_SetBkColor(GUI_BLUE);        /* 背景改回蓝色 */

    GUI_DrawGradientH(110, 50, 210, 150, 0X4117BB, 0XC6B6F5);  /* 绘制用水平颜色梯度填充的矩形 */
    GUI_DrawGradientV(220, 50, 320, 150, 0X4117BB, 0XC6B6F5);  /* 绘制用垂直颜色梯度填充的矩形 */
    GUI_DrawGradientRoundedH(330, 50, 430, 150, 25, 0X4117BB, 0XC6B6F5);  /* 绘制水平颜色梯度填充的圆角矩形 */
    GUI_DrawGradientRoundedV(440, 50, 540, 150, 25, 0X4117BB, 0XC6B6F5);  /* 绘制垂直颜色梯度填充的圆角矩形 */

    GUI_DrawRect(550, 50, 650, 150);  /* 在当前窗口中指定的位置绘制矩形 */
    GUI_FillRect(660, 50, 760, 150);  /* 在当前窗口中指定位置绘制填充的矩形区域 */

    GUI_SetPenSize(5);                /* 设置画笔颜色,单位像素点 */
    GUI_DrawLine(10, 160, 110, 260);  /* 绘制线条 */

    for (i = 0; i < 50; i += 3) GUI_DrawCircle(200, 220, i); /* 绘制圆，无法绘制半径大于180的圆 */

    GUI_FillCircle(320, 220, 50);    /* 在指定位置绘制指定尺寸的填充的圆,无法绘制半径大于180的圆 */

    GUI_SetPenSize(2);
    GUI_SetColor(GUI_CYAN);
    GUI_DrawEllipse(450, 220, 70, 50); /* 在指定位置绘制指定尺寸的椭圆的轮廓 无法处理大于180的rx/ry参数  */
    GUI_SetColor(GUI_MAGENTA);
    GUI_FillEllipse(450, 220, 65, 45); /* 在指定位置绘制指定尺寸的填充的椭圆 */

    emwin_drawarcscale();
    emwin_draw_graph();
}

/**
 * @brief       Alpha混合显示
 * @param       无
 * @return      无
 */
void emwin_alpha_display(void)
{
    GUI_EnableAlpha(1);           /* 使能Alpha功能 */
    GUI_SetBkColor(GUI_WHITE);    /* 设置背景颜色 */
    GUI_Clear();

    GUI_SetTextMode(GUI_TM_TRANS);/* 文本透明显示 */
    GUI_SetColor(GUI_BLACK);
    GUI_SetFont(&GUI_Font32_1);
    GUI_DispStringHCenterAt("ALIENTEK ALPHA TEST", 400, 220);

    GUI_SetAlpha(0x40);                   /* 设置alpha值0x00-0XFF  */
    GUI_SetColor(GUI_RED);
    GUI_FillRect(100, 50, 300, 250);      /* 绘制红色矩形 */

    GUI_SetAlpha(0x80);
    GUI_SetColor(GUI_GREEN);
    GUI_FillRect(200, 150, 400, 350);     /* 绘制绿色矩形 */

    GUI_SetAlpha(0xC0);
    GUI_SetColor(GUI_BLUE);
    GUI_FillRect(300, 250, 500, 450);     /* 绘制蓝色矩形 */

    GUI_SetAlpha(0x80);
    GUI_SetColor(GUI_YELLOW);
    GUI_FillRect(400, 150, 600, 350);     /* 绘制黄色矩形 */

    GUI_SetAlpha(0x40);
    GUI_SetColor(GUI_BROWN);
    GUI_FillRect(500, 50, 700, 250);      /* 绘制棕色矩形 */
    GUI_SetAlpha(0);                      /* 恢复alpha通道 */
}

/**
 * @brief       绘制多边形
 * @param       无
 * @return      无
 */
void emwin_draw_polygon(void)
{
    int i, Mag, magy = 50, Count = 4;
    GUI_SetBkColor(GUI_BLUE);
    GUI_SetColor(GUI_YELLOW);
    GUI_Clear();

    GUI_SetTextMode(GUI_TM_TRANS);   /* 透明显示 */
    GUI_SetFont(&GUI_Font24_ASCII);  /* 设置字体 */
    GUI_DispStringHCenterAt("ALIENTEK PLAYGON DISPLAY", 400, 10);

    GUI_SetColor(GUI_WHITE);
    GUI_SetDrawMode(GUI_DM_XOR);    /* 设置绘制模式 */
    GUI_FillPolygon(aPoints, GUI_COUNTOF(aPoints), 140, 110);

    for (i = 1; i < 10; i++)
    {
        GUI_EnlargePolygon(aEnlargedPoints, aPoints, GUI_COUNTOF(aPoints), i * 5);  /* 放大坐标 */
        GUI_FillPolygon(aEnlargedPoints, GUI_COUNTOF(aPoints), 140, 110);           /* 绘制放大坐标后的多边形 */
    }

    GUI_SetDrawMode(GUI_DM_NORMAL);  /* 恢复默认的绘制模式 */

    GUI_SetColor(GUI_GREEN);

    for (Mag = 1; Mag <= 4; Mag *= 2, Count /= 2)
    {
        int magx = 600;
        GUI_MagnifyPolygon(aMagnifiedPoints, aPoints, GUI_COUNTOF(aPoints), Mag); /* 按指定系数放大多边形 */

        for (i = Count; i > 0; i--, magx += 40 * Mag)
        {
            GUI_FillPolygon(aMagnifiedPoints, GUI_COUNTOF(aPoints), magx, magy);  /* 绘制多边形 */
        }

        magy += 20 * Mag;
    }

    GUI_SetFont(&GUI_Font8x16);
    GUI_DispStringAt("Polygons of arbitrary shape ", 100, 300);
    GUI_DispStringAt("in any color", 220, 320);
    GUI_SetColor(GUI_DARKRED);
    GUI_FillPolygon (&aPointArrow[0], 7, 200, 400);  /* 绘制多边形 */
}


/**
 * @brief       绘制扇形
 * @param       无
 * @return      无
 */
void emwin_draw_Pie(void)
{
    int i, a0, a1;
    int xSize;
    int ySize;
  
    GUI_SetBkColor(GUI_BLUE);
    GUI_SetColor(GUI_YELLOW);
    GUI_Clear();
  
    GUI_SetTextMode(GUI_TM_TRANS);   /* 透明显示 */
    GUI_SetFont(&GUI_Font24_ASCII);  /* 设置字体 */
    GUI_DispStringHCenterAt("ALIENTEK Pie DISPLAY", 400, 10);
  
    xSize = LCD_GetXSize();
    ySize = LCD_GetYSize();
    const unsigned aValues[] = { 100, 135, 190, 240, 340, 360};
    const GUI_COLOR aColors[] = { GUI_BLUE, GUI_GREEN, GUI_RED,
                                  GUI_CYAN, GUI_MAGENTA, GUI_YELLOW };
    
    for (i = 0; i < GUI_COUNTOF(aValues); i++) 
    {
        a0 = (i == 0) ? 0 : aValues[i - 1];
        a1 = aValues[i];
        GUI_SetColor(aColors[i]);
        GUI_DrawPie(xSize/2, ySize/2, 100, a0, a1, 0);
    }
}


void emwin_main(void)
{
    while(1)
    {
        emwin_display_2d();
        GUI_Delay(2000);
        emwin_alpha_display();
        GUI_Delay(2000);
        emwin_draw_polygon();
        GUI_Delay(2000);
        emwin_draw_Pie();
        GUI_Delay(4000);        
    }
}
