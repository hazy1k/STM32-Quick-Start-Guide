/**
 ****************************************************************************************************
 * @file        emwin_demo.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-01-04
 * @brief       emWin 数值显示实验
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
/* uC-OS3 */
#include "os.h"
#include "cpu.h"


/**
 * @brief       emwin_main(emwin入口)
 * @param       x--x轴坐标
 * @param       y--y轴坐标
 * @retval      无
 */
void emwin_main(uint16_t x,uint16_t y)
{
    char* emwinversion;                                            /* emwin版本号 */
    GUI_SetBkColor(GUI_BLUE);
    GUI_Clear();
    GUI_SetTextMode(GUI_TM_TRANS);                                 /* 透明显示模式 */
    
    GUI_SetFont(&GUI_Font24_ASCII);
    GUI_SetColor(GUI_YELLOW);
    GUI_DispStringHCenterAt("ALIENTEK NUM DISPLAY DEMO",400,10);
    
    GUI_SetColor(GUI_RED);
    GUI_SetFont(&GUI_Font20_ASCII);
    
    GUI_DispStringAt("GUI_DispDecAt():",x,y);
    GUI_DispDecAt(200,x+150,y,3);                                  /* 显示数值200 */
    
    
    GUI_DispStringAt("GUI_DispFloat():",x,y+20);
    GUI_GotoXY(x+150,y+20);                                        /* 移动光标 */
    GUI_DispFloat(49.35,5);                                        /* 显示浮点数49.35 */
    GUI_DispString("   ");
    GUI_DispFloat(-0.345,6);                                       /* 显示浮点数-0.345 */
    
    GUI_DispStringAt("GUI_DispBinAt():",x,y+40);
    GUI_DispBinAt(8,x+150,y+40,4);                                 /* 显示十进制数8的二进制值 */
    
    
    GUI_DispStringAt("GUI_DispHex():",x,y+60); 
    GUI_GotoXY(x+150,y+60);                                        /* 移动光标 */
    GUI_DispHex(100,3);                                            /* 显示十进制数100的16进制值 */
    
    GUI_DispStringAt("GUI_DispHexAt():",x,y+80);
    GUI_DispHexAt(100,x+150,y+80,3);                               /* 在指定位置显示十进制数100的16进制值 */
    
    GUI_DispStringAt("GUI_GetVersionString():",x,y+100);
    emwinversion = (char*)GUI_GetVersionString();                  /* 获取当前emwin版本号 */
    GUI_DispStringAt(emwinversion,x+200,y+100);                    /* 显示emwin版本号 */
	
	while(1)
	{
		GUI_Delay(100);
	}
}
