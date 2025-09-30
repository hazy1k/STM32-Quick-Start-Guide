/**
 ****************************************************************************************************
 * @file        emwin_demo.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-01-04
 * @brief       emWin 文本显示实验
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
/*****************************************************************************************************/
/*EMWIN*/
#include "GUI.h"
#include "WM.h"
/*****************************************************************************************************/
/*uC-OS3*/
#include "os.h"
#include "cpu.h"


/**
 * @brief       emwin_main(emwin入口)
 * @param       无
 * @retval      无
 */
void emwin_main(void)
{
    int i;
    char acText[] = "This example demostrates text wrapping";
    GUI_RECT Rect = {200, 240, 259, 299};   /* 定义矩形显示区域 */
    GUI_WRAPMODE aWm[] = {GUI_WRAPMODE_NONE,
                          GUI_WRAPMODE_CHAR,
                          GUI_WRAPMODE_WORD};

    GUI_SetBkColor(GUI_BLUE);              /* 设置背景颜色 */
    GUI_Clear();                           /* 清屏 */
    GUI_SetFont(&GUI_Font24_ASCII);        /* 设置字体 */
    GUI_SetColor(GUI_YELLOW);              /* 设置前景色(如文本，画线等颜色) */
    GUI_DispString("HELLO WORD!");

    GUI_SetFont(&GUI_Font8x16);            /* 设置字体 */
    GUI_SetPenSize(10);                    /* 设置笔大小 */
    GUI_SetColor(GUI_RED);                 /* 红色字体 */
    GUI_DrawLine(300, 50, 500, 130);       /* 绘线 */
    GUI_DrawLine(300, 130, 500, 50);       /* 绘线 */
    GUI_SetBkColor(GUI_BLACK);             /* 设置黑色背景 */
    GUI_SetColor(GUI_WHITE);               /* 设置字体颜色为白色 */
    GUI_SetTextMode(GUI_TM_NORMAL);        /* 正常模式 */
    GUI_DispStringHCenterAt("GUI_TM_NORMAL", 400, 50);
    GUI_SetTextMode(GUI_TM_REV);           /* 反转文本 */
    GUI_DispStringHCenterAt("GUI_TM_REV", 400, 66);
    GUI_SetTextMode(GUI_TM_TRANS);         /* 透明文本 */
    GUI_DispStringHCenterAt("GUI_TM_TRANS", 400, 82);
    GUI_SetTextMode(GUI_TM_XOR);           /* 异或文本 */
    GUI_DispStringHCenterAt("GUI_TM_XOR", 400, 98);
    GUI_SetTextMode(GUI_TM_TRANS | GUI_TM_REV);  /* 透明反转文本 */
    GUI_DispStringHCenterAt("GUI_EM_TRANS|GUI_TM_REV", 400, 114);

    GUI_SetTextMode(GUI_TM_TRANS);        /* 透明文本 */

    for (i = 0; i < 3; i++)
    {
        GUI_SetColor(GUI_WHITE);
        GUI_FillRectEx(&Rect);
        GUI_SetColor(GUI_BLACK);
        GUI_DispStringInRectWrap(acText, &Rect, GUI_TA_LEFT, aWm[i]); /* 在当前窗口指定的矩形区域内显示字符串(并可自动换行) */
        Rect.x0 += 70;
        Rect.x1 += 70;
    }
    
    while(1)
    {
        GUI_Delay(100);
    }
}
