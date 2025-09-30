/**
 ****************************************************************************************************
 * @file        emwin_demo.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-01-04
 * @brief       emWin的TTF格式汉字显示实验
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

#include "./FATFS/source/ff.h"
#include "emwin_demo.h"
#include "./MALLOC/malloc.h"
#include "./SYSTEM/usart/usart.h"
#include "GUI.h"
#include "os.h"
#include "cpu.h"


static GUI_TTF_CS Cs0, Cs1, Cs2, Cs3;
FIL TTFFontFile;
FSIZE_t  TTFFontFileSize;
GUI_FONT TTF12_Font;
GUI_FONT TTF18_Font;
GUI_FONT TTF24_Font;
GUI_FONT TTF36_Font;
GUI_TTF_DATA TTFData;

/**
 * @brief       创建TTF字体，共EMWIN使用
 * @param       fxpath:存放TTF字库的路径
 * @retval      0,成功；其他值，失败
 */
int emwin_create_ttf_font(uint8_t *fxpath)
{
    CPU_SR_ALLOC();
    int result;
    uint16_t bread;
    char *TtfBuffer;

    result = f_open(&TTFFontFile, (const TCHAR *)fxpath, FA_READ);  /* 打开字库文件 */ 
  
    if (result != FR_OK)
    {
        return 1;
    }
  

    TtfBuffer = mymalloc(SRAMIN, TTFFontFile.obj.objsize); /* 申请内存 */
    if (TtfBuffer == NULL) 
    {
        printf("内存申请失败\r\n");
        return 3; /* 内存申请失败 */
    }

    /* 读取字体数据 */
#if SYS_SUPPORT_OS
    CPU_CRITICAL_ENTER();       /* 进入临界区 */
#endif

    result = f_read(&TTFFontFile, TtfBuffer, TTFFontFile.obj.objsize, (UINT *)&bread); /* 读取数据 */

    if (result != FR_OK) 
    {
        return 4; /* 文件打开失败，跳出 */
    }

    f_close(&TTFFontFile); /* 关闭TTFFointFile文件*/
#if SYS_SUPPORT_OS
    CPU_CRITICAL_EXIT();   /* 退出临界区 */
#endif

    TTFData.pData = TtfBuffer; /* 指向文件地址 */
    TTFData.NumBytes = TTFFontFile.obj.objsize; /* 文件大小*/

    Cs0.pTTF        = &TTFData;
    Cs0.PixelHeight = 12;
    Cs0.FaceIndex   = 0;

    Cs1.pTTF        = &TTFData;
    Cs1.PixelHeight = 18;
    Cs1.FaceIndex   = 0;

    Cs2.pTTF        = &TTFData;
    Cs2.PixelHeight = 24;
    Cs2.FaceIndex   = 0;

    Cs3.pTTF        = &TTFData;
    Cs3.PixelHeight = 36;
    Cs3.FaceIndex   = 0;

    result = GUI_TTF_CreateFont(&TTF12_Font, &Cs0); /* 创建字体 */

    if (result) return 5; /* 字体创建失败*/

    result = GUI_TTF_CreateFont(&TTF18_Font, &Cs1); /* 创建字体 */

    if (result) return 5; /* 字体创建失败*/

    result = GUI_TTF_CreateFont(&TTF24_Font, &Cs2); /* 创建字体*/

    if (result) return 5; /* 字体创建失败 */

    result = GUI_TTF_CreateFont(&TTF36_Font, &Cs3); /* 创建字体*/

    if (result) return 5; /* 字体创建失败 */

    return 0;
}

/**
 * @brief       emwin_main(emwin入口)
 * @param       无
 * @return      无
 */
void emwin_main(void) 
{
    int result;
    result = emwin_create_ttf_font((uint8_t *)"0:/SYSTEM/EMWINFONT/TTF/comesinhandy.ttf");

    if(result) 
    {
        printf("TTF字体创建失败,%d\r\n",result);
    }
    
    GUI_CURSOR_Show();
    GUI_SetBkColor(GUI_CYAN);
    GUI_SetColor(GUI_RED);
    GUI_UC_SetEncodeUTF8();
    GUI_Clear();
    GUI_SetFont(&TTF12_Font);
    GUI_DispStringAt("ALIENTEK STemWin TTF Font Test 0123456789!",5,10);
    GUI_SetFont(&TTF18_Font);
    GUI_DispStringAt("ALIENTEK STemWin TTF Font Test 0123456789!",5,25);
    GUI_SetFont(&TTF24_Font);
    GUI_DispStringAt("ALIENTEK STemWin TTF Font Test 0123456789!",5,45);
    GUI_SetFont(&TTF36_Font);
    GUI_DispStringAt("ALIENTEK STemWin TTF Font Test 0123456789!",5,70);
}
