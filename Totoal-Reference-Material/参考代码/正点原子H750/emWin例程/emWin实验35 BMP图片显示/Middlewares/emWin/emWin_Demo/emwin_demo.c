/**
 ****************************************************************************************************
 * @file        emwin_demo.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-01-04
 * @brief       emWin的BMP图片显示实验
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
#include "EmWinHZFont.h"
#include "GUI.h"
#include "./MALLOC/malloc.h"
#include "./FATFS/source/ff.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/LCD/lcd.h"
#if SYS_SUPPORT_OS
#include "os.h"
#include "cpu.h"
#endif


static FIL BMPFile;
static char bmpBuffer[BMPPERLINESIZE];
/*******************************************************************
*
*       Static functions
*
********************************************************************
 */
/*********************************************************************
*
*       BmpGetData
*
* Function description
*   This routine is called by GUI_BMP_DrawEx(). The routine is responsible
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
static int emiwn_bmp_get_data(void *p, const U8 **ppData, unsigned NumBytesReq, U32 Off)
{
    static int readaddress = 0;
    FIL *phFile;
    UINT NumBytesRead;

    phFile = (FIL *)p;
  
#if SYS_SUPPORT_OS
    CPU_SR_ALLOC();
#endif
        
    if (NumBytesReq > sizeof(bmpBuffer))
    {
        NumBytesReq = sizeof(bmpBuffer);
    }


    if (Off == 1) readaddress = 0; /* 移动指针到应该读取的位置 */
    else readaddress = Off;

#if SYS_SUPPORT_OS
    CPU_CRITICAL_ENTER();    /* 进入临界区 */
#endif
    f_lseek(phFile, readaddress);


    f_read(phFile, bmpBuffer, NumBytesReq, &NumBytesRead); /* 读取数据到缓冲区中 */
#if SYS_SUPPORT_OS
    CPU_CRITICAL_EXIT();           /* 退出临界区 */
#endif
    *ppData = (U8 *)bmpBuffer;
    return NumBytesRead;            /* 返回读取到的字节数 */
}

/**
 * @brief       在指定位置显示加载到RAM中的JPEG图片
                BMPFileName:图片在SD卡或者其他存储设备中的路径(需文件系统支持！)
 * @param       mode:显示模式
      @arg      0 在指定位置显示，有参数x,y确定显示位置
      @arg      1 在LCD中间显示图片，当选择此模式的时候参数x,y无效。
 * @param       X:图片左上角在LCD中的x轴位置(当参数mode为1时，此参数无效)
 * @param       y:图片左上角在LCD中的y轴位置(当参数mode为1时，此参数无效)
 * @param       member:  缩放比例的分子项
 * @param       denom:缩放比例的分母项
 * @retval      0 显示正常,其他 失败
  */
int emwin_dispbmp(uint8_t *BMPFileName, uint8_t mode, uint32_t x, uint32_t y, int member, int denom)
{
    uint16_t bread;
    char *bmpbuffer;
    char result;
    volatile int XSize, YSize;
    volatile float Xflag, Yflag;
    GUI_COLOR forecolor, bkcolor;
  
#if SYS_SUPPORT_OS
    CPU_SR_ALLOC();
#endif

    forecolor = GUI_GetColor();  /* 获取当前前景色 */
    bkcolor = GUI_GetBkColor();  /* 获取当前背景色 */
    result = f_open(&BMPFile, (const TCHAR *)BMPFileName, FA_READ); /* 打开文件 */

    /* 文件打开错误或者文件大于BMPMEMORYSIZE */
    if ((result != FR_OK) || (BMPFile.obj.objsize > BMPMEMORYSIZE)) return 1;

    bmpbuffer = mymalloc(SRAMIN, BMPFile.obj.objsize); /* 申请内存 */

    if (bmpbuffer == NULL) 
    {
        printf("申请内存失败\r\n");
        return 2;
    }

#if SYS_SUPPORT_OS
    CPU_CRITICAL_ENTER();    /* 进入临界区 */
#endif

    result = f_read(&BMPFile, bmpbuffer, BMPFile.obj.objsize, (UINT *)&bread); /* 读取数据 */

    if (result != FR_OK) 
    {
        printf("读取失败\r\n");
        return 3;
    }

#if SYS_SUPPORT_OS
    CPU_CRITICAL_EXIT();           /* 退出临界区 */
#endif

    switch (mode)
    {
        case 0: /* 在指定位置显示图片 */
            if ((member == 1) && (denom == 1)) /* 无需缩放，直接绘制 */
            {
                GUI_BMP_Draw(bmpbuffer, x, y); /* 在指定位置显示BMP图片 */
            }
            else  /* 否则图片需要缩放 */
            {
                GUI_BMP_DrawScaled(bmpbuffer, x, y, member, denom);
            }

            break;

        case 1: /* 在LCD中间显示图片 */
            XSize = GUI_BMP_GetXSize(bmpbuffer); /* 获取图片的X轴大小 */
            YSize = GUI_BMP_GetYSize(bmpbuffer); /* 获取图片的Y轴大小 */

            if ((member == 1) && (denom == 1)) /* 无需缩放，直接绘制 */
            {
                /* 在LCD中间显示图片 */
                GUI_BMP_Draw(bmpbuffer, (lcddev.width - XSize) / 2 - 1, (lcddev.height - YSize) / 2 - 1);
            }
            else  /* 否则图片需要缩放 */
            {
                Xflag = (float)XSize * ((float)member / (float)denom);
                Yflag = (float)YSize * ((float)member / (float)denom);
                XSize = (lcddev.width - (int)Xflag) / 2 - 1;
                YSize = (lcddev.height - (int)Yflag) / 2 - 1;
                GUI_BMP_DrawScaled(bmpbuffer, XSize, YSize, member, denom);
            }

            break;
    }

    f_close(&BMPFile);          /* 关闭BMPFile文件 */
    myfree(SRAMIN, bmpbuffer);  /* 释放内存 */
    GUI_SetColor(forecolor);    /* 恢复前景色 */
    GUI_SetBkColor(bkcolor);    /* 恢复背景色 */
    return 0;
}

/**
 * @brief       在指定位置显示无需加载到RAM中的BMP图片(需文件系统支持！对于小RAM，推荐使用此方法！)
                BMPFileName:图片在SD卡或者其他存储设备中的路径
 * @param       mode:显示模式
      @arg      0 在指定位置显示，有参数x,y确定显示位置
      @arg      1 在LCD中间显示图片，当选择此模式的时候参数x,y无效。
 * @param       X:图片左上角在LCD中的x轴位置(当参数mode为1时，此参数无效)
 * @param       y:图片左上角在LCD中的y轴位置(当参数mode为1时，此参数无效)
 * @param       member:  缩放比例的分子项
 * @param       denom:缩放比例的分母项
 * @retval      0 显示正常,其他 失败
  */
int emwin_dispbmpex(uint8_t *BMPFileName, uint8_t mode, uint32_t x, uint32_t y, int member, int denom)
{
    char result;
    volatile int XSize, YSize;
    volatile float Xflag, Yflag;
    GUI_COLOR forecolor, bkcolor;
    forecolor = GUI_GetColor();  /* 获取当前前景色 */
    bkcolor = GUI_GetBkColor();  /* 获取当前背景色 */

    result = f_open(&BMPFile, (const TCHAR *)BMPFileName, FA_READ);	/* 打开文件 */

    /* 文件打开错误 */
    if (result != FR_OK)    return 1;

    switch (mode)
    {
        case 0: /* 在指定位置显示图片 */
            if ((member == 1) && (denom == 1)) /* 无需缩放，直接绘制 */
            {
                GUI_BMP_DrawEx(emiwn_bmp_get_data, &BMPFile, x, y); /* 在指定位置显示BMP图片 */
            }
            else  /* 否则图片需要缩放 */
            {
                GUI_BMP_DrawScaledEx(emiwn_bmp_get_data, &BMPFile, x, y, member, denom);
            }

            break;

        case 1:  /* 在LCD中间显示图片 */
            XSize = GUI_BMP_GetXSizeEx(emiwn_bmp_get_data, &BMPFile);   /* 获取图片的X轴大小 */
            YSize = GUI_BMP_GetYSizeEx(emiwn_bmp_get_data, &BMPFile);   /* 获取图片的Y轴大小 */

            if ((member == 1) && (denom == 1)) /* 无需缩放，直接绘制 */
            {
                /* 在LCD中间显示图片 */
                GUI_BMP_DrawEx(emiwn_bmp_get_data, &BMPFile, (lcddev.width - XSize) / 2 - 1, (lcddev.height - YSize) / 2 - 1);
            }
            else  /* 否则图片需要缩放 */
            {
                Xflag = (float)XSize * ((float)member / (float)denom);
                Yflag = (float)YSize * ((float)member / (float)denom);
                XSize = (lcddev.width - (int)Xflag) / 2 - 1;
                YSize = (lcddev.height - (int)Yflag) / 2 - 1;
                GUI_BMP_DrawScaledEx(emiwn_bmp_get_data, &BMPFile, XSize, YSize, member, denom);
            }

            break;
    }

    f_close(&BMPFile);        /* 关闭BMPFile文件 */
    GUI_SetColor(forecolor);  /* 恢复前景色 */
    GUI_SetBkColor(bkcolor);  /* 恢复背景色 */
    return 0;
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
void emwin_create_bmppicture(uint8_t *filename, int x0, int y0, int Xsize, int Ysize)
{
    static FIL hFile;
   
    GUI_COLOR forecolor, bkcolor;
    forecolor = GUI_GetColor();   /* 获取当前前景色 */
    bkcolor = GUI_GetBkColor();   /* 获取当前背景色 */
    /* 创建一个文件，路径为filename,如果文件原本就存在的话会被新建的文件覆盖掉！ */
    f_open(&hFile, (const TCHAR *)filename, FA_READ | FA_WRITE | FA_CREATE_ALWAYS);
    GUI_BMP_SerializeEx(_WriteByte2File, x0, y0, Xsize, Ysize, &hFile);
    f_close(&hFile);           /* 关闭文件 */
    GUI_SetColor(forecolor);   /* 恢复前景色 */
    GUI_SetBkColor(bkcolor);   /* 恢复背景色 */
}

/**
 * @brief       emwin_main(emwin入口)
 * @param       无
 * @return      无
 */
void emwin_main(void) 
{
    GUI_SetBkColor(GUI_BLUE);
    GUI_SetColor(GUI_RED);
    GUI_SetFont(&GUI_FontHZ16);
    GUI_Clear();

    while (1)
    {
        GUI_DispStringHCenterAt("在指定位置显示一张加载到RAM中的BMP图片", 400, 5);
        emwin_dispbmp((uint8_t *)"0:/PICTURE/BMP/ALIENTEKLOGO.bmp", 0, 10, 78, 1, 1);
        GUI_Delay(1000);
        GUI_Clear();

        GUI_DispStringHCenterAt("在LCD中间显示一张缩小1/2的加载到RAM中的BMP图片", 400, 5);
        emwin_dispbmp((uint8_t *)"0:/PICTURE/BMP/ALIENTEKLOGO.bmp", 1, 0, 0, 1, 2);
        GUI_Delay(1000);
        GUI_Clear();

        GUI_DispStringHCenterAt("在LCD中间显示一张扩大4/3倍的加载到RAM中的BMP图片", 400, 5);
        emwin_dispbmp((uint8_t *)"0:/PICTURE/BMP/ALIENTEKLOGO.bmp", 1, 0, 0, 4, 3);
        GUI_Delay(1000);
        GUI_Clear();

        GUI_DispStringHCenterAt("在指定位置显示一张无需加载的BMP图片", 400, 5);
        emwin_dispbmpex((uint8_t *)"0:/PICTURE/BMP/ALIENTEKLOGO.bmp", 0, 10, 78, 1, 1);
        GUI_Delay(1000);
        GUI_Clear();


        GUI_DispStringHCenterAt("在LCD中间显示一张缩小1/2的无需加载的BMP图片", 400, 5);
        emwin_dispbmpex((uint8_t *)"0:/PICTURE/BMP/ALIENTEKLOGO.bmp", 1, 0, 0, 1, 2);
        GUI_Delay(1000);
        GUI_Clear();

        GUI_DispStringHCenterAt("在LCD中间显示一张扩大4/3倍的无需加载的BMP图片", 400, 5);
        emwin_dispbmpex((uint8_t *)"0:/PICTURE/BMP/ALIENTEKLOGO.bmp", 1, 0, 0, 4, 3);
        GUI_Delay(1000);
        GUI_Clear();
    }
}
