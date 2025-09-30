/*********************************************************************
*                    SEGGER Microcontroller GmbH                     *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2020  SEGGER Microcontroller GmbH                *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V6.16 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The software has been licensed to  ARM LIMITED whose registered office
is situated at  110 Fulbourn Road,  Cambridge CB1 9NJ,  England solely
for  the  purposes  of  creating  libraries  for  ARM7, ARM9, Cortex-M
series,  and   Cortex-R4   processor-based  devices,  sublicensed  and
distributed as part of the  MDK-ARM  Professional  under the terms and
conditions  of  the   End  User  License  supplied  with  the  MDK-ARM
Professional. 
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
Licensing information
Licensor:                 SEGGER Software GmbH
Licensed to:              ARM Ltd, 110 Fulbourn Road, CB1 9NJ Cambridge, UK
Licensed SEGGER software: emWin
License number:           GUI-00181
License model:            LES-SLA-20007, Agreement, effective since October 1st 2011 
Licensed product:         MDK-ARM Professional
Licensed platform:        ARM7/9, Cortex-M/R4
Licensed number of seats: -
----------------------------------------------------------------------
File        : GUI_TOUCH_X.C
Purpose     : Config / System dependent externals for GUI
---------------------------END-OF-HEADER------------------------------
*/

#include "GUI.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/TOUCH/touch.h"


void GUI_TOUCH_X_ActivateX(void) {
        /* XPT2046_WriteCMD(0x90);*/
}

void GUI_TOUCH_X_ActivateY(void) {
        /*XPT2046_WriteCMD(0xd0);*/
}

int  GUI_TOUCH_X_MeasureX(void) {
    int32_t xvalue;
    uint16_t x,y; 
    
    /* 判断电阻屏还是电容屏 */
    if ((tp_dev.touchtype & 0x80) == 0)
    {
        tp_dev.scan(0);
        
        if (tp_dev.sta & TP_PRES_DOWN)                              /* 触摸屏被按下 */
        {
            /* 电阻屏出现触摸没有那么顺畅的问题，请把touch.c文件下的TP_ERR_RANGE宏定义设置大一点 */
            tp_read_xy2(&x,&y); 
            return (4095 - x); 
//            return tp_read_xoy(0X90);                             /* CMD_RDX=0XD0  */
        }
    }
    else        /* 电容屏 */
    {
        tp_dev.scan(0);
        xvalue = tp_dev.x[0];
        return xvalue;
    }
    
    return 0;
}

int  GUI_TOUCH_X_MeasureY(void)
{
    int32_t yvalue;
    uint16_t x,y;
    
    /* 判断电阻屏还是电容屏 */
    if ((tp_dev.touchtype & 0x80) == 0)
    {
        /* 解决快速按下的鼠标乱屏问题 */
        tp_dev.scan(0);
        
        if (tp_dev.sta & TP_PRES_DOWN)                              /* 触摸屏被按下 */
        {
            /* 电阻屏出现触摸没有那么顺畅的问题，请把touch.c文件下的TP_ERR_RANGE宏定义设置大一点 */
            tp_read_xy2(&x,&y); 
            return y; 
//            return tp_read_xoy(0XD0);                             /*CMD_RDX=0XD0  */
        }
    }
    else        /* 电容屏 */
    {
        yvalue = tp_dev.y[0];
        return yvalue;
    }
    
    return 0;
}


