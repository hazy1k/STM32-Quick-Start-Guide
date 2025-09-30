/**
 ****************************************************************************************************
 * @file        emwin_demo.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-01-04
 * @brief       emWin绘画位图实验
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


extern GUI_BITMAP bmALIENTEK;  /* ALIENTEK图标 */

/**
 * @brief       显示C文件格式的位图
 * @param       无
 * @return      无
 */
void emwin_draw_bitmap(void)
{
    GUI_SetBkColor(GUI_BLUE);
    GUI_SetColor(GUI_YELLOW);
    GUI_Clear();
    
    GUI_SetFont(&GUI_Font24_ASCII);
    GUI_SetTextMode(GUI_TM_TRANS);        /* 透明显示 */
    GUI_DispStringHCenterAt("ALIENTEK BITMAP DISPLAY",400,0 );
    
    GUI_DrawBitmap(&bmALIENTEK,295,194);  /* 绘制ALIENTEK图标 */
}

/**
 * @brief       在显示器中缩放位图
 * @param       Xmag:X方向的比例因子，单位为千分之
 * @param       Ymag:Y方向的比例因子，单位为千分之
 * @return      无
 */
void emwin_zoom_bitmap(int Xmag,int Ymag)
{
    GUI_SetBkColor(GUI_BLUE);
    GUI_Clear();
    GUI_DrawBitmapEx(&bmALIENTEK,400,240,105,46,Xmag,Ymag); /* 按照比例绘制位图 */
}

/**
 * @brief       emwin_main(emwin入口)
 * @param       无
 * @retval      无
 */
void emwin_main(void)
{
    int Xmag = 0 ,Ymag = 0;     /* 缩放因子,单位 千分之 */
    emwin_draw_bitmap();
    GUI_Delay(2000);

    while(1)
    {
        Xmag += 100;
        Ymag += 100;

        if(Xmag >= 2000)
        {
            Xmag = 100;
        }

        if(Ymag >= 2000)
        {
            Ymag = 100;
        }

        emwin_zoom_bitmap(Xmag,Ymag); /* 绘制缩放的位图 */
        GUI_Delay(500);
    }
}
