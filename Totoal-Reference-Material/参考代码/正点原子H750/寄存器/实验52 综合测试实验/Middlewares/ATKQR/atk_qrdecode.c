/**
 ****************************************************************************************************
 * @file        atk_qrdecode.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.1
 * @date        2022-06-02
 * @brief       二维码识别库 代码
 *              本二维码&条码识别LIB由ALIENTEK提供,方便大家使用.
 *              该二维码&条码识别LIB功能:
 *              1,支持QR二维码识别.
 *              2,支持CODE128,CODE39,I25,EAN13等四种编码方式的条码识别.
 *              3,支持UTF8-OEM转换输出(需要客户自己提供转换码表).
 *
 *              当需要支持UTF82OEM转换输出的时候:
 *              1,将ATK_QR_UTF82OEM_SUPPORT宏的值改为1(默认是1)
 *              2,实现atk_qr_convert函数.该函数将unicode编码转换为OEM编码.
 *              这样,输出结果,就是OEM编码的字符串(一般就是GBK编码),方便在单片机系统里面显示.
 *              如果不需要支持OEM转换,则:将ATK_QR_UTF82OEM_SUPPORT宏的值改为0即可,这样输出结果就是原始编码
 *              的字符串(UTF8/GBK).如果有中文,在单片机系统里面,可能是无法直接显示的,如果是英文,则可以直接显示.
 *
 *              当需要支持GBK编码的二维码时:将ATK_QR_GBK_SUPPORT宏的值改为1(默认是1)即可.
 *
 *              本LIB移植步骤:
 *              1,实现atk_qrdecode.c里面的所有函数.
 *              2,堆栈(Stack_Size)设置为0X1000或以上.
 *
 *              本LIB使用步骤:
 *              1,调用atk_qr_init函数,初始化识别程序,返回值为ATK_QR_OK,则初始化成功.
 *              2,调用atk_qr_decode函数,给定参数,对图像进行识别.
 *              3,如果需要不停的识别,则重复第2个步骤即可.
 *              4,调用atk_qr_destroy函数,结束识别,释放所有内存.结束识别.
 *
 *              uint8_t atk_qr_decode(uint16_t bmp_width,uint16_t bmp_heigh,uint8_t *bmp,uint8_t btype,uint8_t* result)函数参数说明:
 *              img_width,img_heigh:输入图像的宽度和高度
 *              imgbuf:图像缓存区(8位灰度图像,不是RGB565!!!!)
 *              btype:0,识别二维码
 *                    1,识别CODE128条码
 *                    2,识别CODE39条码
 *                    3,识别I25条码
 *                    4,识别EAN13条码
 *              result:识别结果缓冲区.如果result[0]==0,则说明未识别到任何数据,否则就是识别到的数据(字符串)
 *              返回值:ATK_QR_OK,识别完成
 *                          其他,错误代码
 *              如果需要对所有支持的编码进行识别,则轮流设置btype为0~4即可实现.
 *
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.1 20220602
 * 1, 修改注释方式
 * 2, 修改u8/u16/u32为uint8_t/uint16_t/uint32_t
 ****************************************************************************************************
 */

#include "atk_qrdecode.h"
#include "./MALLOC/malloc.h"


/**
 * @brief       内存设置函数
 * @param       p               : 内存首地址
 * @param       c               : 要设置的值
 * @param       len             : 需要设置的内存大小(字节为单位)
 * @retval      无
 */
void atk_qr_memset(void *p, uint8_t c, uint32_t len)
{
    my_mem_set(p, c, len);
}

/**
 * @brief       内存申请函数
 * @param       size            : 申请的内存大小
 * @retval      0,失败; 其他, 内存首地址;
 */
void *atk_qr_malloc(uint32_t size)
{
    return mymalloc(SRAM12, size);
}

/**
 * @brief       内存重申请函数
 * @param       ptr             : 旧内存首地址
 * @param       size            : 需要设置的内存大小(字节为单位)
 * @retval      无
 */
void *atk_qr_realloc(void *ptr, uint32_t size)
{
    return myrealloc(SRAM12, ptr, size);
}

/**
 * @brief       内存释放函数
 * @param       ptr             : 内存首地址
 * @retval      无
 */
void atk_qr_free(void *ptr)
{
    myfree(SRAM12, ptr);
}

/**
 * @brief       内存复制函数
 * @param       des             : 目的地址
 * @param       src             : 源地址
 * @param       n               : 需要复制的内存长度(字节为单位)
 * @retval      无
 */
void atk_qr_memcpy(void *des, void *src, uint32_t n)
{
    my_mem_copy((uint8_t *)des, (uint8_t *)src, n);
}


/* 如果使能了UTF2OEM支持,则需要实现将UTF8转出的UNICODE转码成OEM */
#if ATK_QR_UTF82OEM_SUPPORT

#include "./FATFS/source/ff.h"


/**
 * @brief       将UNICODE编码转换成OEM编码
 * @param       unicode         : UNICODE编码的字符内码
 * @retval      OEM编码方式的字符内码
 */
uint16_t atk_qr_convert(uint16_t unicode)
{
    return ff_uni2oem(unicode, 0);
}

#endif















