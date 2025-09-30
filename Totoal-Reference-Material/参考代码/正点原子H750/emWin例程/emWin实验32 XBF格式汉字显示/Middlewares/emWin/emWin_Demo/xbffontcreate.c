/**
 ****************************************************************************************************
 * @file        xbffontcreate.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-01-04
 * @brief       emWin的XBF格式汉字显示实验
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
 
#include "xbffontcreate.h"
#include "./FATFS/source/ff.h"


/* 定义字体 */
GUI_FONT XBF12_Font;
GUI_FONT XBF16_Font;
GUI_FONT XBF24_Font;
GUI_FONT XBF36_Font;
GUI_XBF_DATA  XBF12_Data;
GUI_XBF_DATA  XBF16_Data;
GUI_XBF_DATA  XBF24_Data;
GUI_XBF_DATA  XBF36_Data;
FIL XBF16FontFile;
FIL XBF12FontFile;
FIL XBF24FontFile;
FIL XBF36FontFile;

/**
 * @brief       创建XBF12字体，共EMWIN使用
 * @param       Off:      在XBF中偏移(位置)
 * @param       NumBytes: 要读取的字节数
 * @param       pVoid:    要读取的文件
 * @param       pBuff:    读取到的数据的缓冲区
 * @retval      返回值:0，成功；1，失败
 */
static int _cbGetData(uint32_t Off, uint16_t NumBytes, void *pVoid, void *pBuffer)
{
    volatile int result;
    volatile uint16_t bread;
    FIL *hFile;

    hFile = (FIL *)pVoid;

    /* 设置在文件中的偏移(位置) */
    result = f_lseek(hFile, Off);

    if (result != FR_OK)  return 1; /* 返回错误 */

    /* 读取字体数据 */

    result = f_read(hFile, pBuffer, NumBytes, (UINT *)&bread); /* 读取数据 */


    if (result != FR_OK) return 1; /* 返回错误 */

    return 0;
}

/**
 * @brief       创建XBF12字体，共EMWIN使用
 * @param       fxpath:XBF字体文件路径
 * @retval      返回值:0，成功；1，失败
 */
uint8_t Create_XBF12(uint8_t *fxpath)
{
    volatile int result;
    result = f_open(&XBF12FontFile, (const TCHAR *)fxpath, FA_READ);  /* 打开字库文件 */

    if (result != FR_OK) return 1;

    /* 创建XBF12字体 */
    GUI_XBF_CreateFont( &XBF12_Font,               /* 指向GUI_FONT结构 */
                        &XBF12_Data,               /* 指向GUI_XBF_DATA结构 */
                        GUI_XBF_TYPE_PROP_AA2_EXT, /* 要创建的字体类型 */
                        (GUI_XBF_GET_DATA_FUNC *)_cbGetData,        /* 回调函数 */
                        &XBF12FontFile);           /* 窗体给回调函数_cbGetData的参数 */
    return 0;
}

/**
 * @brief       创建XBF16字体，共EMWIN使用
 * @param       fxpath:XBF字体文件路径
 * @retval      返回值:0，成功；1，失败
 */
uint8_t Create_XBF16(uint8_t *fxpath)
{
    volatile int result;
    result = f_open(&XBF16FontFile, (const TCHAR *)fxpath, FA_READ);	/*打开字库文件 */

    if (result != FR_OK) return 1;

    /*创建XBF16字体 */
    GUI_XBF_CreateFont(	&XBF16_Font,              /* 指向GUI_FONT结构 */
                        &XBF16_Data,              /* 指向GUI_XBF_DATA结构 */
                        GUI_XBF_TYPE_PROP_AA2_EXT,/* 要创建的字体类型  */
                        (GUI_XBF_GET_DATA_FUNC *)_cbGetData,       /* 回调函数 */
                        &XBF16FontFile);          /* 窗体给回调函数_cbGetData的参数 */
    return 0;
}

/**
 * @brief       创建XBF24字体，共EMWIN使用
 * @param       fxpath:XBF字体文件路径
 * @retval      返回值:0，成功；1，失败
 */
uint8_t Create_XBF24(uint8_t *fxpath)
{
    volatile int result;
    result = f_open(&XBF24FontFile, (const TCHAR *)fxpath, FA_READ); /* 打开字库文件 */

    if (result != FR_OK) return 1;

    /*创建XBF16字体 */
    GUI_XBF_CreateFont( &XBF24_Font,              /* 指向GUI_FONT结构 */
                        &XBF24_Data,              /* 指向GUI_XBF_DATA结构  */
                        GUI_XBF_TYPE_PROP_AA2_EXT,/* 要创建的字体类型 */
                       (GUI_XBF_GET_DATA_FUNC *) _cbGetData,       /* 回调函数 */
                        &XBF24FontFile);          /* 窗体给回调函数_cbGetData的参数 */
    return 0;
}

/**
 * @brief       创建XBF36字体，共EMWIN使用
 * @param       fxpath:XBF字体文件路径
 * @retval      返回值:0，成功；1，失败
 */
uint8_t Create_XBF36(uint8_t *fxpath)
{
    volatile int result;
    result = f_open(&XBF36FontFile, (const TCHAR *)fxpath, FA_READ); /*打开字库文件 */

    if (result != FR_OK) return 1;

    /* 创建XBF16字体 */
    GUI_XBF_CreateFont( &XBF36_Font,              /* 指向GUI_FONT结构 */
                        &XBF36_Data,              /* 指向GUI_XBF_DATA结构 */
                        GUI_XBF_TYPE_PROP_AA2_EXT,/* 要创建的字体类型 */
                        (GUI_XBF_GET_DATA_FUNC *)_cbGetData,       /* 回调函数 */
                        &XBF36FontFile);          /* 窗体给回调函数_cbGetData的参数 */
    return 0;
}
