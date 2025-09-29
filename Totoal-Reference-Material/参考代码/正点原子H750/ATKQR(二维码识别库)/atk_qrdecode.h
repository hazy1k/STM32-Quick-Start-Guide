#ifndef __ATK_QRDECODE_H
#define __ATK_QRDECODE_H
#include <stdint.h>
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//
//本二维码&条码识别LIB由ALIENTEK提供,方便大家使用.
//该二维码&条码识别LIB功能:
//1,支持QR二维码识别.
//2,支持CODE128,CODE39,I25,EAN13等四种编码方式的条码识别.
//3,支持UTF8-OEM转换输出(需要客户自己提供转换码表).

//当需要支持UTF82OEM转换输出的时候:
//1,将ATK_QR_UTF82OEM_SUPPORT宏的值改为1(默认是1)
//2,实现atk_qr_convert函数.该函数将unicode编码转换为OEM编码.
//这样,输出结果,就是OEM编码的字符串(一般就是GBK编码),方便在单片机系统里面显示.
//如果不需要支持OEM转换,则:将ATK_QR_UTF82OEM_SUPPORT宏的值改为0即可,这样输出结果就是原始编码
//的字符串(UTF8/GBK).如果有中文,在单片机系统里面,可能是无法直接显示的,如果是英文,则可以直接显示.

//当需要支持GBK编码的二维码时:将ATK_QR_GBK_SUPPORT宏的值改为1(默认是1)即可.

//本LIB移植步骤:	
//1,实现atk_qrdecode.c里面的所有函数. 
//2,堆栈(Stack_Size)设置为0X1000或以上.

//本LIB使用步骤:					   
//1,调用atk_qr_init函数,初始化识别程序,返回值为ATK_QR_OK,则初始化成功.
//2,调用atk_qr_decode函数,给定参数,对图像进行识别.
//3,如果需要不停的识别,则重复第2个步骤即可.
//4,调用atk_qr_destroy函数,结束识别,释放所有内存.结束识别.

//u8 atk_qr_decode(u16 bmp_width,u16 bmp_heigh,u8 *bmp,u8 btype,u8* result)函数参数说明:
//img_width,img_heigh:输入图像的宽度和高度
//imgbuf:图像缓存区(8位灰度图像,不是RGB565!!!!)
//btype:0,识别二维码
//      1,识别CODE128条码
//      2,识别CODE39条码
//      3,识别I25条码
//      4,识别EAN13条码
//result:识别结果缓冲区.如果result[0]==0,则说明未识别到任何数据,否则就是识别到的数据(字符串)
//返回值:ATK_QR_OK,识别完成
//            其他,错误代码
//如果需要对所有支持的编码进行识别,则轮流设置btype为0~4即可实现.
 	  				      
//版本:V1.4
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//公司网址:www.alientek.com
//Copyright(C) 广州市星翼电子科技有限公司 2016-2026
//All rights reserved	    
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//ATK_QR_UTF82OEM_SUPPORT:定义是否支持将UTF8编码转换为OEM编码(中文一般是指GBK编码),定义为:
//1,则输出结果为GBK编码方式的字符串;定义为0,则输出的是原始字符串(未做编码转换,可能是UTF8,也
//可能是GBK,取决于二维码的编码方式).
#define ATK_QR_UTF82OEM_SUPPORT		1	

//ATK_QR_GBK_SUPPORT:定义在ATK_QR_UTF82OEM_SUPPORT==1的时候,是否支持识别GBK编码的二维码,如
//果定义为1,则程序会先判断是UTF8还是GBK,根据识别结果,再做转换.如果定义为0,则只识别UTF8编码方
//式的二维码.不识别GBK编码的二维码.
//注意:
//1,当ATK_QR_UTF82OEM_SUPPORT==0的时候,该宏定义不起作用.
//2,当发现有识别错误(UTF8识别成GBK了)的时候,可设置ATK_QR_GBK_SUPPORT==0,以更好的支持UTF8
//3,这里的GBK并不是所有GBK都支持,仅支持GB2312编码的识别.
#define ATK_QR_GBK_SUPPORT			1	


//返回值定义
#define ATK_QR_OK					0					//正常
#define ATK_QR_MEM_ERR				1					//内存错误
#define ATK_QR_OTHER_ERR			2					//其他错误
#define ATK_QR_RECO_ERR				3					//无法识别


u8 atk_qr_init(void);									//初始化识别库	
u8 atk_qr_decode(u16 bmp_width,u16 bmp_heigh,u8 *bmp,u8 btype,u8* result);//QR解码函数
void atk_qr_destroy(void);								//结束识别	
void atk_qr_memset(void *p,u8 c,u32 len);				//内存设置函数 
void *atk_qr_malloc(u32 size);							//动态申请内存
void *atk_qr_realloc(void *ptr,u32 size);				//内存重申请函数  
void atk_qr_free(void *ptr);							//动态释放内存
void atk_qr_memcpy(void *des,void *src,u32 n);			//内存复制函数 
u16 atk_qr_convert(u16 unicode);						//unicode转oem
#endif



