#include "atk_qrdecode.h"
#include "malloc.h"
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


//内存设置函数
void atk_qr_memset(void *p,u8 c,u32 len) 
{
	mymemset(p,c,len);
}	 					  
//内存申请函数
void *atk_qr_malloc(u32 size) 
{
	return mymalloc(SRAMIN,size);
}
//内存重申请函数
void *atk_qr_realloc(void *ptr,u32 size) 
{
	return myrealloc(SRAMIN,ptr,size);
}
//内存释放函数
void atk_qr_free(void *ptr) 
{
	myfree(SRAMIN,ptr);
}	
//内存复制函数
void atk_qr_memcpy(void *des,void *src,u32 n) 
{
	mymemcpy((u8*)des,(u8*)src,n);
}
//如果使能了UTF2OEM支持,则需要实现将UTF8转出的UNICODE转码成OEM
#if ATK_QR_UTF82OEM_SUPPORT
#include "ff.h"
//将UNICODE编码转换成OEM编码
//unicode:UNICODE编码的字符内码
//返回值:OEM编码方式的字符内码
u16 atk_qr_convert(u16 unicode)
{
	return ff_convert(unicode,0);	  
}
#endif

