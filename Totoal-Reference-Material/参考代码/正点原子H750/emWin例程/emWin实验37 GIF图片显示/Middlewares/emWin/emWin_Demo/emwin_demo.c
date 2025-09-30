/**
 ****************************************************************************************************
 * @file        emwin_demo.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-01-04
 * @brief       emWin的GIF图片显示实验
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
#include "./FATFS/exfuns/exfuns.h"
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


static FIL GIFFile;
static FIL ScrSortFile; /* 屏幕截图文件 */
static char gifBuffer[GIFPERLINESIZE];


/**
 * @brief       用于将数据指针设置为至少具有有效数据位置
 * @param       p:             指向应用程序定义的数据
 * @param       NumBytesReq:   请求字节数
 * @param       ppData:        数据指针的指针。这个指针应该设置为有效位置.
 * @param       StartOfFile:   如果该标志为1，则数据指针应设置为数据流的开始
 * @retval      可用数据字节数.
 */
static int emwin_gif_get_data(void *p, const U8 **ppData, unsigned NumBytesReq, U32 Off)
{
    static int readaddress = 0;
    FIL *phFile;
    UINT NumBytesRead;

    phFile = (FIL *)p;

    if (NumBytesReq > sizeof(gifBuffer))
    {
        NumBytesReq = sizeof(gifBuffer);
    }

    /* 移动指针到应该读取的位置 */
    if (Off == 1) readaddress = 0;
    else readaddress = Off;

    f_lseek(phFile, readaddress);

    /* 读取数据到缓冲区中 */
    f_read(phFile, gifBuffer, NumBytesReq, &NumBytesRead);


    *ppData = (U8 *)gifBuffer;
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
    uint16_t index = 0;

    while (index < 0XFFFF)
    {
        sprintf((char *)pname, "0:/SCREENSHORT/屏幕截图%05d.bmp", index);
        res = f_open(&ScrSortFile, (const TCHAR *)pname, FA_READ); /* 尝试打开这个文件 */

        if (res == FR_NO_FILE)
        {
            break;                            /* 改文件名不存在=正是我们需要的 */
        }
        else if (res == FR_OK)
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
void emwin_create_gifpicture(uint8_t *filename, int x0, int y0, int Xsize, int Ysize)
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
int emwin_displaygif(char *GIFFileName, uint8_t mode, uint32_t x, uint32_t y, int member, int denom)
{
    int i;
    uint16_t bread;
    char *gifbuffer;
    char result;
    int XSize, YSize;
    GUI_GIF_INFO GifInfo;
    GUI_GIF_IMAGE_INFO ImageInfo;
    float Xflag, Yflag;


    result = f_open(&GIFFile, (const TCHAR *)GIFFileName, FA_READ); /* 打开文件 */

    /* 文件打开错误或者文件大于JPEGMEMORYSIZE */
    if ((result != FR_OK) || (GIFFile.obj.objsize > GIFMEMORYSIZE)) return 1;

    gifbuffer = mymalloc(SRAMIN, GIFFile.obj.objsize);

    if (gifbuffer == NULL) return 2;


    result = f_read(&GIFFile, gifbuffer, GIFFile.obj.objsize, (UINT *)&bread); /* 读取数据 */

    if (result != FR_OK) return 3;


    GUI_GIF_GetInfo(gifbuffer, GIFFile.obj.objsize, &GifInfo); /* 获取GIF图片信息 */

    XSize = GifInfo.xSize; /* 获取GIF图片的X轴大小 */
    YSize = GifInfo.ySize; /* 获取GIF图片的Y轴大小 */

    switch (mode)
    {
        case 0: /* 在指定位置显示图片 */
            if ((member == 1) && (denom == 1)) /* 无需缩放，直接绘制 */
            {
                /* 在指定位置显示JPEG图片 */
                for (i = 0; i < GifInfo.NumImages; i++)
                {
                    GUI_GIF_DrawSub(gifbuffer, GIFFile.obj.objsize, x, y, i);
                    GUI_GIF_GetImageInfo(gifbuffer, GIFFile.obj.objsize, &ImageInfo, i);
                    GUI_Delay(ImageInfo.Delay ? ImageInfo.Delay * 10 : 100); /*延时*/
                }
            }
            else  /* 否则图片需要缩放 */
            {
                for (i = 0; i < GifInfo.NumImages; i++)
                {
                    GUI_GIF_DrawSubScaled(gifbuffer, GIFFile.obj.objsize, x, y, i, member, denom);
                    GUI_GIF_GetImageInfo(gifbuffer, GIFFile.obj.objsize, &ImageInfo, i);
                    GUI_Delay(ImageInfo.Delay ? ImageInfo.Delay * 10 : 100); /* 延时 */
                }
            }

            break;

        case 1: /* 在LCD中间显示图片 */
            if ((member == 1) && (denom == 1)) /* 无需缩放，直接绘制 */
            {
                /* 在LCD中间显示图片 */
                for (i = 0; i < GifInfo.NumImages; i++)
                {
                    GUI_GIF_DrawSub(gifbuffer, GIFFile.obj.objsize, (lcddev.width - XSize) / 2 - 1, (lcddev.height - YSize) / 2 - 1, i);
                    GUI_GIF_GetImageInfo(gifbuffer, GIFFile.obj.objsize, &ImageInfo, i);
                    GUI_Delay(ImageInfo.Delay ? ImageInfo.Delay * 10 : 100); /* 延时 */
                }
            }
            else  /* 否则图片需要缩放 */
            {
                Xflag = (float)XSize * ((float)member / (float)denom);
                Yflag = (float)YSize * ((float)member / (float)denom);
                XSize = (lcddev.width - (int)Xflag) / 2 - 1;
                YSize = (lcddev.height - (int)Yflag) / 2 - 1;

                for (i = 0; i < GifInfo.NumImages; i++)
                {
                    GUI_GIF_DrawSubScaled(gifbuffer, GIFFile.obj.objsize, XSize, YSize, i, member, denom);
                    GUI_GIF_GetImageInfo(gifbuffer, GIFFile.obj.objsize, &ImageInfo, i);
                    GUI_Delay(ImageInfo.Delay ? ImageInfo.Delay * 10 : 100); /* 延时 */
                }
            }

            break;
    }

    f_close(&GIFFile);         /* 关闭JPEGFile文件 */
    myfree(SRAMIN, gifbuffer); /* 释放内存 */
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
int emwin_displaygifex(char *GIFFileName, uint8_t mode, uint32_t x, uint32_t y, int member, int denom)
{
    char result;
    int i;
    int XSize, YSize;
    float Xflag, Yflag;
    GUI_GIF_INFO GifInfo;
    GUI_GIF_IMAGE_INFO ImageInfo;

    result = f_open(&GIFFile, (const TCHAR *)GIFFileName, FA_READ); /* 打开文件 */

    /* 文件打开错误 */
    if (result != FR_OK)    return 1;

    GUI_GIF_GetInfoEx(emwin_gif_get_data, &GIFFile, &GifInfo);
    XSize = GifInfo.xSize; /* GIF图片X大小 */
    YSize = GifInfo.ySize; /* GIF图片Y大小 */

    switch (mode)
    {
        case 0: /* 在指定位置显示图片 */
            if ((member == 1) && (denom == 1)) /* 无需缩放，直接绘制 */
            {
                /* 在指定位置显示BMP图片 */
                for (i = 0; i < GifInfo.NumImages; i++)
                {
                    GUI_GIF_DrawSubEx(emwin_gif_get_data, &GIFFile, x, y, i);
                    GUI_GIF_GetImageInfoEx(emwin_gif_get_data, &GIFFile, &ImageInfo, i);
                    GUI_Delay(ImageInfo.Delay ? ImageInfo.Delay * 10 : 100); /*延时*/
                }
            }
            else  /* 否则图片需要缩放 */
            {
                for (i = 0; i < GifInfo.NumImages; i++)
                {
                    GUI_GIF_DrawSubScaledEx(emwin_gif_get_data, &GIFFile, x, y, i, member, denom);
                    GUI_GIF_GetImageInfoEx(emwin_gif_get_data, &GIFFile, &ImageInfo, i);
                    GUI_Delay(ImageInfo.Delay ? ImageInfo.Delay * 10 : 100); /* 延时 */
                }
            }

            break;

        case 1: /* 在LCD中间显示图片 */
            if ((member == 1) && (denom == 1)) /* 无需缩放，直接绘制 */
            {
                /* 在LCD中间显示图片 */
                for (i = 0; i < GifInfo.NumImages; i++)
                {
                    GUI_GIF_DrawSubEx(emwin_gif_get_data, &GIFFile, (lcddev.width - XSize) / 2 - 1, (lcddev.height - YSize) / 2 - 1, i);
                    GUI_GIF_GetImageInfoEx(emwin_gif_get_data, &GIFFile, &ImageInfo, i);
                    GUI_Delay(ImageInfo.Delay ? ImageInfo.Delay * 10 : 100); /*延时*/
                }
            }
            else  /* 否则图片需要缩放 */
            {
                Xflag = (float)XSize * ((float)member / (float)denom);
                Yflag = (float)YSize * ((float)member / (float)denom);
                XSize = (lcddev.width - (int)Xflag) / 2 - 1;
                YSize = (lcddev.height - (int)Yflag) / 2 - 1;

                for (i = 0; i < GifInfo.NumImages; i++)
                {
                    GUI_GIF_DrawSubScaledEx(emwin_gif_get_data, &GIFFile, XSize, YSize, i, member, denom);
                    GUI_GIF_GetImageInfoEx(emwin_gif_get_data, &GIFFile, &ImageInfo, i);
                    GUI_Delay(ImageInfo.Delay ? ImageInfo.Delay * 10 : 100); /* 延时 */
                }
            }

            break;
    }


    f_close(&GIFFile); /* 关闭GIFFile文件 */
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
    GUI_SetFont(&GUI_FontHZ16);
    GUI_SetColor(GUI_RED);
    GUI_Clear();

    while (1)
    {
        GUI_DispStringHCenterAt("在指定位置显示一张加载到RAM中的GIF图片", 400, 0);
        emwin_displaygif("0:/PICTURE/GIF/龙.gif", 0, 20, 20, 1, 1);
        GUI_Delay(1000);
        GUI_Clear();

        GUI_DispStringHCenterAt("在LCD中间显示一张缩小1/2的加载到RAM中的GIF图片", 400, 0);
        emwin_displaygif("0:/PICTURE/GIF/火柴人.gif", 0, 20, 20, 1, 1);
        GUI_Delay(1000);
        GUI_Clear();

        GUI_DispStringHCenterAt("在指定位置显示一张无需加载的GIF图片", 400, 0);
        emwin_displaygifex("0:/PICTURE/GIF/龙.gif", 0, 20, 20, 1, 1);
        GUI_Delay(1000);
        GUI_Clear();

        GUI_DispStringHCenterAt("在LCD中间显示一张缩小1/2的无需加载的GIF图片", 400, 0);
        emwin_displaygifex("0:/PICTURE/GIF/火柴人.gif", 1, 0, 0, 1, 2);
        GUI_Delay(1000);
        GUI_Clear();
    }
}
