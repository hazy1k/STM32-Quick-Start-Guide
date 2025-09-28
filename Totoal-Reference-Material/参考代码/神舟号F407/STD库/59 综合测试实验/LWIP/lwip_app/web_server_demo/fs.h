#ifndef __FS_H__
#define __FS_H__
#include "lwip/opt.h"
#include "ff.h"
#include "malloc.h"
/////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板 
//lwip fs驱动代码(基于lwip的fs.c修改,以支持从SD卡读取网页源码和各种资源)	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/3/16
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 


/** Set this to 1 to include an application state argument per file
 * that is opened. This allows to keep a state per connection/file.
 */
#ifndef LWIP_HTTPD_FILE_STATE
#define LWIP_HTTPD_FILE_STATE         0
#endif

/** HTTPD_PRECALCULATED_CHECKSUM==1: include precompiled checksums for
 * predefined (MSS-sized) chunks of the files to prevent having to calculate
 * the checksums at runtime. */
#ifndef HTTPD_PRECALCULATED_CHECKSUM
#define HTTPD_PRECALCULATED_CHECKSUM  0
#endif

#if HTTPD_PRECALCULATED_CHECKSUM
struct fsdata_chksum 
{
	u32_t offset;
	u16_t chksum;
	u16_t len;
};
#endif /* HTTPD_PRECALCULATED_CHECKSUM */

//fs_file结构体
struct fs_file 
{
	FIL *flwip;				//文件指针
	char *data;   			//数据缓存区
	int len;  				//剩余数据长度  
	u32 dataleft;			//数据缓冲区剩余字节数
	u32 dataptr;			//数据缓冲区读指针
	u32 fleft;				//文件剩余字节数
	void *pextension;
#if HTTPD_PRECALCULATED_CHECKSUM			//HTTPD_PRECALCULATED_CHECKSUM是否定义?
	const struct fsdata_chksum *chksum;
	u16_t chksum_count;
#endif  
	u8_t http_header_included;				//是否包含http头,这里我们设置:LWIP_HTTPD_DYNAMIC_HEADERS为1,让lwip自动添加http头
#if LWIP_HTTPD_CUSTOM_FILES					//LWIP_HTTPD_CUSTOM_FILES是否定义?
	u8_t is_custom_file;
#endif  
#if LWIP_HTTPD_FILE_STATE					//LWIP_HTTPD_FILE_STATE是否定义?
	void *state;
#endif  
};

struct fs_file *fs_open(const char *name);
void fs_close(struct fs_file *file);
int fs_read(struct fs_file *file, char *buffer, int count);
int fs_bytes_left(struct fs_file *file);

#if LWIP_HTTPD_FILE_STATE
/** This user-defined function is called when a file is opened. */
void *fs_state_init(struct fs_file *file, const char *name);
/** This user-defined function is called when a file is closed. */
void fs_state_free(struct fs_file *file, void *state);
#endif /* #if LWIP_HTTPD_FILE_STATE */

#endif /* __FS_H__ */
