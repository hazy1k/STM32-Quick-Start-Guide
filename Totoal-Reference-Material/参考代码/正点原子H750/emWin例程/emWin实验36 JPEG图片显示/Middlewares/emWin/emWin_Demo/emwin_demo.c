/**
 ****************************************************************************************************
 * @file        emwin_demo.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-01-04
 * @brief       emWin的JPEG图片显示实验
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
#include "EmWinHZFont.h"
#include "GUI.h"
#include "./MALLOC/malloc.h"
#include "./FATFS/source/ff.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/LCD/lcd.h"

/*****************************************************************************************************/
/*EMWIN*/
#include "GUI.h"
#include "WM.h"
#include "DIALOG.h"
#include "EmWinHZFont.h"
#include "WM.h"
/*****************************************************************************************************/
/*uC-OS3*/
#include "os.h"
#include "cpu.h"


static FIL JPEGFile;
static FIL ScrSortFile; /* 屏幕截图文件 */
static char jpegBuffer[JPEGPERLINESIZE];
extern GUI_FONT TTF12_Font;
extern GUI_FONT TTF18_Font;
extern GUI_FONT TTF24_Font;
extern GUI_FONT TTF36_Font;

/*******************************************************************
*
*       Static functions
*
********************************************************************
*/
/*********************************************************************
*
*       _GetData
*
* Function description
*   This routine is called by GUI_JPEG_DrawEx(). The routine is responsible
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
static int emwin_jpeg_get_data(void *p, const uint8_t **ppData, unsigned NumBytesReq, uint32_t Off)
{
    static int readaddress = 0;
    FIL *phFile;
    UINT NumBytesRead;

    phFile = (FIL *)p;

    if (NumBytesReq > sizeof(jpegBuffer))
    {
        NumBytesReq = sizeof(jpegBuffer);
    }

    /*移动指针到应该读取的位置 */
    if (Off == 1) 
    {
        readaddress = 0;
    }
    else 
    {
        readaddress = Off;
    }

    f_lseek(phFile, readaddress);

    /* 读取数据到缓冲区中 */
    f_read(phFile, jpegBuffer, NumBytesReq, &NumBytesRead);

    *ppData = (uint8_t *)jpegBuffer;
    return NumBytesRead;/* 返回读取到的字节数 */
}

/**
 * @brief       得到一个文件名
 * @param       pname:得到的文件名
 * @retval      无
 */
void emwin_emwinjpg_new_pathname(uint8_t *pname)
{
    uint8_t res;
    uint16_t index=0;

    while(index<0XFFFF)
    {
        sprintf((char*)pname,"0:/SCREENSHORT/屏幕截图%05d.bmp",index);
        res=f_open(&ScrSortFile,(const TCHAR*)pname,FA_READ);/* 尝试打开这个文件 */

        if (res==FR_NO_FILE)
        {
            break;                            /* 改文件名不存在=正是我们需要的 */
        }
        else if (res==FR_OK)
        {
            f_close(&ScrSortFile);           /* 如果打开成功，就关闭掉 */
        }

        index++;
    }
}

/**
 * @brief       此函数被GUI_BMP_Serialize()调用，用来向文件写入字节
 * @param       无
 * @retval      无
 */
static void _WriteByte2File(U8 Data, void * p) 
{
    UINT nWritten;
    f_write((FIL*)p,&Data,1,&nWritten);
}

/**
 * @brief       生成一个BMP图片，保存到SD卡中,实现屏幕截图功能！
 * @param       x0:创建BMP文件的X起始位置
 * @param       y0:创建BMP文件的Y起始位置
 * @param       Xsize:X的大小
 * @param       Ysize:Y的大小
 * @retval      无
 */
void emwin_create_jpgpicture(uint8_t *filename,int x0,int y0,int Xsize,int Ysize)
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
 * @brief       在指定位置显示加载到RAM中的JPEG图片
                JPEGFileName:图片在SD卡或者其他存储设备中的路径(需文件系统支持！)
 * @param       mode:显示模式
      @arg      0 在指定位置显示，有参数x,y确定显示位置
      @arg      1 在LCD中间显示图片，当选择此模式的时候参数x,y无效。
 * @param       X:图片左上角在LCD中的x轴位置(当参数mode为1时，此参数无效)
 * @param       y:图片左上角在LCD中的y轴位置(当参数mode为1时，此参数无效)
 * @param       member:  缩放比例的分子项
 * @param       denom:缩放比例的分母项
 * @retval      0 显示正常,其他 失败
 */
int emwin_displyjpeg(uint8_t *JPEGFileName, uint8_t mode, uint32_t x, uint32_t y, int member, int denom)
{
    uint16_t bread;
    int flag;
    char *jpegbuffer;
    char result;
    int XSize, YSize;
    GUI_JPEG_INFO JpegInfo;
    float Xflag, Yflag;


    result = f_open(&JPEGFile, (const TCHAR *)JPEGFileName, FA_READ); /* 打开文件 */

    /*文件打开错误或者文件大于JPEGMEMORYSIZE*/
    if ((result != FR_OK) || (JPEGFile.obj.objsize > JPEGMEMORYSIZE))
    {
        printf("文件打开错误\r\n");
        return 1;
    }

    jpegbuffer = mymalloc(SRAMIN, JPEGFile.obj.objsize); /* 申请内存 */

    if (jpegbuffer == NULL) 
    {
        printf("申请内存失败\r\n");
        return 2;
    }


    result = f_read(&JPEGFile, jpegbuffer, JPEGFile.obj.objsize, (UINT *)&bread); /* 读取数据 */

    if (result != FR_OK)
    {
        printf("读取失败\r\n");
        return 3;
    }

    GUI_JPEG_GetInfo(jpegbuffer, JPEGFile.obj.objsize, &JpegInfo); /* 获取JEGP图片信息 */
    XSize = JpegInfo.XSize;   /* 获取JPEG图片的X轴大小 */
    YSize = JpegInfo.YSize;   /* 获取JPEG图片的Y轴大小 */

    switch (mode)
    {
        case 0: /*在指定位置显示图片*/
            if ((member == 1) && (denom == 1)) /*无需缩放，直接绘制*/
            {
                flag = GUI_JPEG_Draw(jpegbuffer, JPEGFile.obj.objsize, x, y); /*在指定位置显示JPEG图片*/
                if (flag != 0)
                {
                    printf("错误0\r\n");
                }
            }
            else  /*否则图片需要缩放*/
            {
                GUI_JPEG_DrawScaled(jpegbuffer, JPEGFile.obj.objsize, x, y, member, denom);
            }

            break;

        case 1: /* 在LCD中间显示图片 */
            if ((member == 1) && (denom == 1)) /* 无需缩放，直接绘制 */
            {
                /* 在LCD中间显示图片 */
                GUI_JPEG_Draw(jpegbuffer, JPEGFile.obj.objsize, (lcddev.width - XSize) / 2 - 1, (lcddev.height - YSize) / 2 - 1);
            }
            else  /* 否则图片需要缩放 */
            {
                Xflag = (float)XSize * ((float)member / (float)denom);
                Yflag = (float)YSize * ((float)member / (float)denom);
                XSize = (lcddev.width - (int)Xflag) / 2 - 1;
                YSize = (lcddev.height - (int)Yflag) / 2 - 1;
                GUI_JPEG_DrawScaled(jpegbuffer, JPEGFile.obj.objsize, XSize, YSize, member, denom);
            }

            break;
    }

    f_close(&JPEGFile);         /* 关闭JPEGFile文件 */
    myfree(SRAMIN, jpegbuffer); /* 释放内存 */
    return 0;
}

/**
 * @brief       在指定位置显示无需加载到RAM中的BMP图片(需文件系统支持！对于小RAM，推荐使用此方法！)
                JPEGFileName:图片在SD卡或者其他存储设备中的路径
 * @param       mode:显示模式
      @arg      0 在指定位置显示，有参数x,y确定显示位置
      @arg      1 在LCD中间显示图片，当选择此模式的时候参数x,y无效。
 * @param       X:图片左上角在LCD中的x轴位置(当参数mode为1时，此参数无效)
 * @param       y:图片左上角在LCD中的y轴位置(当参数mode为1时，此参数无效)
 * @param       member:  缩放比例的分子项
 * @param       denom:缩放比例的分母项
 * @retval      0 显示正常,其他 失败
 */
int emwin_displayjpegex(uint8_t *JPEGFileName, uint8_t mode, uint32_t x, uint32_t y, int member, int denom)
{
    volatile char result;
    volatile int flag;
    volatile int XSize, YSize;
    volatile float Xflag, Yflag;
    GUI_JPEG_INFO JpegInfo;

    result = f_open(&JPEGFile, (const TCHAR *)JPEGFileName, FA_READ); /* 打开文件 */

    /* 文件打开错误 */
    if (result != FR_OK)
    {
        printf("文件打开错误\r\n");
        return 1;
    }

    GUI_JPEG_GetInfoEx((GUI_GET_DATA_FUNC *)emwin_jpeg_get_data, &JPEGFile, &JpegInfo);
    XSize = JpegInfo.XSize; /* JPEG图片X大小 */
    YSize = JpegInfo.YSize; /* JPEG图片Y大小 */

    switch (mode)
    {
        case 0:   /*在指定位置显示图片*/
            if ((member == 1) && (denom == 1))   /*无需缩放，直接绘制*/
            {
                flag = GUI_JPEG_DrawEx((GUI_GET_DATA_FUNC *)emwin_jpeg_get_data, &JPEGFile, x, y); /*在指定位置显示BMP图片*/
                if (flag != 0)
                {
                    printf("错误1\r\n");
                }
            }
            else   /*否则图片需要缩放*/
            {
                GUI_JPEG_DrawScaledEx((GUI_GET_DATA_FUNC *)emwin_jpeg_get_data, &JPEGFile, x, y, member, denom);
            }

            break;

        case 1: /* 在LCD中间显示图片 */
            if ((member == 1) && (denom == 1)) /* 无需缩放，直接绘制 */
            {
                /* 在LCD中间显示图片 */
                GUI_JPEG_DrawEx((GUI_GET_DATA_FUNC *)emwin_jpeg_get_data, &JPEGFile, (lcddev.width - XSize) / 2 - 1, (lcddev.height - YSize) / 2 - 1);
            }
            else  /* 否则图片需要缩放 */
            {
                Xflag = (float)XSize * ((float)member / (float)denom);
                Yflag = (float)YSize * ((float)member / (float)denom);
                XSize = (lcddev.width - (int)Xflag) / 2 - 1;
                YSize = (lcddev.height - (int)Yflag) / 2 - 1;
                GUI_JPEG_DrawScaledEx((GUI_GET_DATA_FUNC *)emwin_jpeg_get_data, &JPEGFile, XSize, YSize, member, denom);
            }

            break;
    }

    f_close(&JPEGFile);  /* 关闭BMPFile文件 */
    return 0;
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
        GUI_DispStringHCenterAt("在指定位置显示一张加载到RAM中的JPEG图片", 240, 0);
        emwin_displyjpeg((uint8_t *)"0:/PICTURE/JPEG/实例图片.jpg", 0, 0, 0, 1, 1);
        GUI_Delay(1000);
        GUI_Clear();

        GUI_DispStringHCenterAt("在LCD中间显示一张扩大2倍的加载到RAM中的JPEG图片", 240, 0);
        emwin_displyjpeg((uint8_t *)"0:/PICTURE/JPEG/jpeg0.jpg", 1, 0, 0, 2, 1);
        GUI_Delay(1000);
        GUI_Clear();

        GUI_DispStringHCenterAt("在LCD中间显示一张缩小1/2的加载到RAM中的JPEG图片", 240, 0);
        emwin_displyjpeg((uint8_t *)"0:/PICTURE/JPEG/SIM900A.jpg", 1, 0, 0, 1, 2);
        GUI_Delay(1000);
        GUI_Clear();

        GUI_DispStringHCenterAt("在指定位置显示一张无需加载的JPEG图片", 240, 0);
        emwin_displayjpegex((uint8_t *)"0:/PICTURE/JPEG/实例图片.jpg", 0, 0, 0, 1, 1);
        GUI_Delay(1000);
        GUI_Clear();

        GUI_DispStringHCenterAt("在LCD中间显示一张扩大2倍的无需加载的JPEG图片", 240, 0);
        emwin_displayjpegex((uint8_t *)"0:/PICTURE/JPEG/jpeg0.jpg", 1, 0, 0, 2, 1);
        GUI_Delay(1000);
        GUI_Clear();

        GUI_DispStringHCenterAt("在LCD中间显示一张缩小1/2的无需加载的JPEG图片", 240, 0);
        emwin_displayjpegex((uint8_t *)"0:/PICTURE/JPEG/SIM900A.jpg", 1, 0, 0, 1, 2);
        GUI_Delay(1000);
        GUI_Clear();
    }
}
