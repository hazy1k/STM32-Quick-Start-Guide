/**
 ****************************************************************************************************
 * @file        emwin_demo.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-01-04
 * @brief       emWin的PNG图片显示实验
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
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"

/*****************************************************************************************************/
/*EMWIN*/
#include "GUI.h"
#include "WM.h"
#include "DIALOG.h"
#include "WM.h"
#include "EmWinHZFont.h"
#include "GUI.h"
#include "./MALLOC/malloc.h"
#include "./FATFS/source/ff.h"
#include "./BSP/LCD/lcd.h"
/*****************************************************************************************************/
/*uC-OS3*/
#include "os.h"
#include "cpu.h"


static FIL PNGFile;

/*******************************************************************
*
*       Static functions
*
********************************************************************
*/
/*********************************************************************
*
*       PngGetData
*
* Function description
*   This routine is called by GUI_PNG_DrawEx(). The routine is responsible
*   for setting the data pointer to a valid data location with at least
*   one valid byte.
*
* Parameters:
*   p           - Pointer to application defined data.
*   NumBytesReq - Number of bytes requested.
*   ppData      - Pointer to data pointer. This pointer should be set to
*                 a valid location.
*   StartOfFile - If this flag is 1, the data pointer should be set to the
*                 beginning of the data stream.
*
* Return value:
*   Number of data bytes available.
*/
static int emwin_png_get_data(void *p, const U8 **ppData, unsigned NumBytesReq, U32 Off)
{
    CPU_SR_ALLOC();
    static int readaddress = 0;
    FIL *phFile;
    U8 *pData;
    UINT NumBytesRead;

    pData = (U8 *)*ppData;
    phFile = (FIL *)p;

    /* 移动指针到应该读取的位置 */
    if (Off == 1) readaddress = 0;
    else readaddress = Off;

#if SYS_SUPPORT_OS
    CPU_CRITICAL_ENTER(); /* 临界区 */
#endif
    f_lseek(phFile, readaddress);

    /* 读取数据到缓冲区中 */
    f_read(phFile, pData, NumBytesReq, &NumBytesRead);


#if SYS_SUPPORT_OS
    CPU_CRITICAL_EXIT(); /* 退出临界区 */
#endif
    return NumBytesRead; /* 返回读取到的字节数 */
}

/**
 * @brief       得到一个文件名
 * @param       pname:得到的文件名
 * @retval      无
 */
void emwin_emwinjpg_new_pathname(uint8_t *pname)
{
    static FIL ScrSortFile; /* 屏幕截图文件 */
    uint8_t res;
    uint16_t index = 0;

    while (index < 0XFFFF)
    {
        sprintf((char *)pname, "0:/SCREENSHORT/屏幕截图%05d.bmp", index);
        res = f_open(&ScrSortFile, (const TCHAR *)pname, FA_READ); /* 尝试打开这个文件 */

        if (res == FR_NO_FILE)
        {
            break;
        }
        else if (res == FR_OK)
        {
            f_close(&ScrSortFile);  /* 如果打开成功，就关闭掉 */
        }

        index++;
    }
}

/**
 * @brief       此函数被GUI_BMP_Serialize()调用，用来向文件写入字节
 * @param       无
 * @retval      无
 */
static void _WriteByte2File(U8 Data, void *p)
{
    UINT nWritten;
    f_write((FIL *)p, &Data, 1, &nWritten);
}

/**
 * @brief       生成一个BMP图片，保存到SD卡中,实现屏幕截图功能！
 * @param       x0:创建BMP文件的X起始位置
 * @param       y0:创建BMP文件的Y起始位置
 * @param       Xsize:X的大小
 * @param       Ysize:Y的大小
 * @retval      无
 */
void emwin_create_pngpicture(uint8_t *filename, int x0, int y0, int Xsize, int Ysize)
{
    static FIL hFile;
    GUI_COLOR forecolor, bkcolor;
    forecolor = GUI_GetColor();   /* 获取当前前景色 */
    bkcolor = GUI_GetBkColor();   /* 获取当前背景色 */
    /* 创建一个文件，路径为filename,如果文件原本就存在的话会被新建的文件覆盖掉！*/
    f_open(&hFile,(const TCHAR*)filename,FA_READ | FA_WRITE | FA_CREATE_ALWAYS);
    GUI_BMP_SerializeEx(_WriteByte2File,x0,y0,Xsize,Ysize,&hFile);
    f_close(&hFile);  /* 关闭文件 */
    GUI_SetColor(forecolor);   /* 恢复前景色 */
    GUI_SetBkColor(bkcolor);   /* 恢复背景色 */
}

/**
 * @brief       在指定位置显示加载到RAM中的GIF图片
                GIFFileName:图片在SD卡或者其他存储设备中的路径(需文件系统支持！)
 * @param       mode:显示模式
      @arg      0 在指定位置显示，有参数x,y确定显示位置
      @arg      1 在LCD中间显示图片，当选择此模式的时候参数x,y无效
 * @param       x:图片左上角在LCD中的x轴位置(当参数mode为1时，此参数无效)
 * @param       y:图片左上角在LCD中的y轴位置(当参数mode为1时，此参数无效)
 * @param       member:  缩放比例的分子项
 * @param       denom:缩放比例的分母项
 * @retval      0 显示正常,其他 失败
 */
int emwin_displaypng(char *PNGFileName, uint8_t mode, uint32_t x, uint32_t y)
{
    CPU_SR_ALLOC();
    uint16_t bread;
    char *pngbuffer;
    char result;
    int XSize, YSize;
    int flag;

    result = f_open(&PNGFile, (const TCHAR *)PNGFileName, FA_READ); /* 打开文件 */

    /* 文件打开错误或者文件大于JPEGMEMORYSIZE */
    if ((result != FR_OK) || (PNGFile.obj.objsize > PNGMEMORYSIZE))
    {
        printf("文件打开错误\r\n");
        return 1;
    }

    pngbuffer = mymalloc(SRAMIN, PNGFile.obj.objsize);

    if (pngbuffer == NULL)
    {
        printf("申请内存失败\r\n");
        return 2;
    }

#if SYS_SUPPORT_OS
    CPU_CRITICAL_ENTER(); /* 临界区 */
#endif

    result = f_read(&PNGFile, pngbuffer, PNGFile.obj.objsize, (UINT *)&bread); /* 读取数据 */

    if (result != FR_OK)
    {
        printf("打开文件失败\r\n");
        return 3;
    }

#if SYS_SUPPORT_OS
    CPU_CRITICAL_EXIT(); /* 退出临界区 */
#endif

    XSize = GUI_PNG_GetXSize(pngbuffer, PNGFile.obj.objsize); /* 获取PNG图片的X轴大小 */
    YSize = GUI_PNG_GetYSize(pngbuffer, PNGFile.obj.objsize); /* 获取PNG图片的Y轴大小 */

    switch (mode)
    {
        case 0: /* 在指定位置显示图片 */
            flag = GUI_PNG_Draw(pngbuffer, PNGFile.obj.objsize, x, y); /* 显示PNG图片 */
            if (flag != 0)
            {
                printf("显示PNG图片失败\r\n");
            }
            break;

        case 1: /* 在LCD中间显示图片 */
            flag = GUI_PNG_Draw(pngbuffer, PNGFile.obj.objsize, (lcddev.width - XSize) / 2 - 1, (lcddev.height - YSize) / 2 - 1);
            if (flag != 0)
            {
                printf("显示PNG图片失败\r\n");
            }
            break;
    }

    f_close(&PNGFile); /* 关闭PNGFile文件 */
    myfree(SRAMIN, pngbuffer);
    return 0;
}

/**
 * @brief       指定位置显示无需加载到RAM中的GIF图片(需文件系统支持！对于小RAM，推荐使用此方法！)
                GIFFileName:图片在SD卡或者其他存储设备中的路径
 * @param       mode:显示模式
      @arg      0 在指定位置显示，有参数x,y确定显示位置
      @arg      1 在LCD中间显示图片，当选择此模式的时候参数x,y无效
 * @param       x:图片左上角在LCD中的x轴位置(当参数mode为1时，此参数无效)
 * @param       y:图片左上角在LCD中的y轴位置(当参数mode为1时，此参数无效)
 * @param       member:  缩放比例的分子项
 * @param       denom:缩放比例的分母项
 * @retval      0 显示正常,其他 失败
 */
int emwin_displaypngex(char *PNGFileName, uint8_t mode, uint32_t x, uint32_t y)
{
    char result;
    int XSize, YSize;

    result = f_open(&PNGFile, (const TCHAR *)PNGFileName, FA_READ); /* 打开文件 */

    /* 文件打开错误 */
    if (result != FR_OK) return 1;

    XSize = GUI_PNG_GetXSizeEx(emwin_png_get_data, &PNGFile); /* PNG图片X大小 */
    YSize = GUI_PNG_GetYSizeEx(emwin_png_get_data, &PNGFile); /* PNG图片Y大小 */

    switch (mode)
    {
        case 0: /* 在指定位置显示图片 */
            GUI_PNG_DrawEx(emwin_png_get_data, &PNGFile, x, y);
            break;

        case 1: /* 在LCD中间显示图片 */
            GUI_PNG_DrawEx(emwin_png_get_data, &PNGFile, (lcddev.width - XSize) / 2 - 1, (lcddev.height - YSize) / 2 - 1);
            break;
    }

    f_close(&PNGFile); /* 关闭PNGFile文件 */
    return 0;
}

/**
 * @brief       atk_emwin_main(emwin入口)
 * @param       无
 * @return      无
 */
void emwin_main(void) 
{
    GUI_SetBkColor(GUI_BLUE);
    GUI_SetFont(&GUI_FontHZ16);
    GUI_SetColor(GUI_RED);
    GUI_Clear();

    while (1)
    {
        GUI_DispStringHCenterAt("在指定位置显示一张加载到RAM中的PNG图片", 400, 0);
        emwin_displaypng("0:/PICTURE/PNG/皇冠.png", 0, 200, 100);
        GUI_Delay(1000);
        GUI_Clear();

        emwin_displaypng("0:/PICTURE/PNG/香蕉.png", 1, 0, 0);
        GUI_Delay(1000);
        GUI_Clear();

        GUI_DispStringHCenterAt("在指定位置显示一张无需加载的PNG图片", 400, 0);

        emwin_displaypngex("0:/PICTURE/PNG/文件夹.png", 1, 0, 0);
        GUI_Delay(1000);
        GUI_Clear();

    }
}
