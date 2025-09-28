#include "lwip/opt.h"
#include "lwip/def.h"
#include "fs.h"
#include "spb.h"
#include <string.h> 
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

//定义Web Server资源所在路径,根据自己的实际情况修改
#define HTTP_SRC_PATH  "1:SYSTEM/LWIP/WebServer" 

//单个文件,最大尺寸设置
//当单个文件超过该尺寸后,改用多次读取的方式,继续读取剩余内容.
//如果单个文件不超过该尺寸,则可以一次读完.
#define HTTP_MAX_FILE_SIZE		100*1024	
 
//定义最多能打开的文件数目 
#define LWIP_MAX_OPEN_FILES     10 

////////////////////////////////////////////////////////////////////////////////// 
//定义文件系统内存分配结构体
struct fs_table 
{
	struct fs_file file;  	
	u8_t inuse;   			//0表示未使用,1表示使用
}; 
//定义一个fs_tabble数组,其中包括LWIP_MAX_OPEN_FILES个元素
struct fs_table fs_memory[LWIP_MAX_OPEN_FILES];

//文件内存申请函数 给LWIP_MAX_OPEN_FILES个文件同时分配内存
static struct fs_file * fs_malloc(void)
{
	int i;
	for(i=0;i<LWIP_MAX_OPEN_FILES;i++) 
	{
		if(fs_memory[i].inuse==0) 
		{
			fs_memory[i].inuse = 1;
			
			fs_memory[i].file.flwip=mymalloc(SRAMIN,sizeof(FIL));//为文件指针申请内存
			if(fs_memory[i].file.flwip)
			{
				return &fs_memory[i].file;
			}else return NULL;//申请失败
		}
	}
	return NULL;
}

//释放内存,一次释放掉LWIP_MAX_OPEN_FILES个变量的内存
static void fs_free(struct fs_file *file)
{
	int i;
	for(i=0;i<LWIP_MAX_OPEN_FILES;i++) 
	{
		if(&fs_memory[i].file==file) 
		{
			fs_memory[i].inuse=0;
			myfree(SRAMIN,file->flwip);	//释放内存
			myfree(SRAMEX,file->data);	//释放内存
			break;
		}
	}
	return;
}

//打开一个文件
//name:要打开的文件名
struct fs_file *fs_open(const char *name)
{
	struct fs_file *file; 
	u8 *pname=0; 
	u8 res;
	u32 br; 
	u32 lenth=0;
	//printf("fopen:%s\r\n",name);
	file = fs_malloc();										//申请内存
	if(file==NULL)return NULL; 
	pname=mymalloc(SRAMIN,100);	
	if(pname)												//内存申请成功
	{ 
		strcpy((char*)pname,HTTP_SRC_PATH);					//拷贝根WebServer目录路径
		strcat((char*)pname,name);							//添加子目录 
		//printf("open:%s\r\n",pname);
		res=f_open(file->flwip,(const TCHAR*)pname,FA_READ);//打开文件 
		if(res==FR_OK)
		{  
			if(file->flwip->fsize<HTTP_MAX_FILE_SIZE)lenth=file->flwip->fsize;//lenth为文件大小
			else lenth=HTTP_MAX_FILE_SIZE;					//lenth为HTTP_MAX_FILE_SIZE大小
			file->data=mymalloc(SRAMEX,lenth);				//申请lenth大小内存
			if(!file->data)
			{
				spb_delete();								//释放SPB占用的内存
				file->data=mymalloc(SRAMEX,lenth);			//重新申请lenth大小内存			
			}
			if(file->data)									//申请OK
			{  
				res=f_read(file->flwip,file->data,lenth,&br);
				if(res==FR_OK)
				{ 
					file->len=br;							//读取到的字节 
					file->fleft=file->flwip->fsize-file->flwip->fptr;//文件剩余字节数
					file->dataleft=0;						//必须设置为0
					file->dataptr=0;						//缓冲区读指针设置为0
					file->pextension=NULL;
					file->http_header_included=1; 			//不包含http头
					myfree(SRAMIN,pname); 
					return file;
				}
				
			}
		}
	} 
	if(file->flwip)f_close(file->flwip);	//关闭文件
	myfree(SRAMIN,pname);  
	fs_free(file);							//释放file内存
	return NULL;
}

//关闭文件
//参数:要关闭的文件
void fs_close(struct fs_file *file)
{ 
 	fs_free(file); 					 	//释放file的内存
}

//读取文件
//参数:file 要读取的文件
//参数:buffer 读取到后存放的缓冲区
//参数:count  要读取的个数
int fs_read(struct fs_file *file, char *buffer, int count)
{
 	u8 res;
	u32 br;  
	if(file->fleft&&(file->dataleft==0))	//还有数据,且需要读出
	{
		res=f_read(file->flwip,file->data,HTTP_MAX_FILE_SIZE,&br);//读取20K字节
		if(res==FR_OK)			//读取成功
		{   
			file->dataptr=0;	 
			file->dataleft=br;	//数据缓冲区剩余字节数
 		}else return -1;		//读取错误
	}
	if(file->dataleft)			//缓冲区还有剩余数据
	{
		if(file->dataleft<=count)
		{
			count=file->dataleft; 
		}
		memcpy(buffer,file->data+file->dataptr,count);
		file->dataptr+=count;
		file->dataleft-=count; 
		file->fleft-=count;
		return count; 
	}
	return -1;
} 
//当前文件剩余字节数
int fs_bytes_left(struct fs_file *file)
{ 
	return file->fleft;
}
























