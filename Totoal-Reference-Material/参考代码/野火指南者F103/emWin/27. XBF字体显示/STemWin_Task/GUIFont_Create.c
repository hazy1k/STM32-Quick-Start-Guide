/**
  ******************************************************************************
  * @file    GUIFont_Create.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   XBF格式字体emwin函数接口
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F103 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
	
#include "./flash/bsp_spi_flash.h"
#include "GUIFont_Create.h"
#include "ff.h"
/* FreeRTOS头文件 */
#include "FreeRTOS.h"
#include "task.h"


#include <string.h>
#include <stdlib.h>



/* 存储器初始化标志 */
static uint8_t storage_init_flag = 0;

/* 字库结构体 */
GUI_XBF_DATA 	XBF_XINSONGTI_18_Data;
GUI_FONT     	FONT_XINSONGTI_18;

GUI_XBF_DATA 	XBF_SIYUANHEITI_36_Data;
GUI_FONT     	FONT_SIYUANHEITI_36;

/* 字库存储路径 */
#if (XBF_FONT_SOURCE == USE_FLASH_FONT)

	/* 资源烧录到的FLASH基地址（目录地址，需与烧录程序一致） */
	#define RESOURCE_BASE_ADDR	(16*1024*1024)
	/* 存储在FLASH中的资源目录大小 */
	#define CATALOG_SIZE				(8*1024)

	/* 字库目录信息类型 */
	typedef struct 
	{
		char 	      name[40];  /* 资源的名字 */
		uint32_t  	size;      /* 资源的大小 */ 
		uint32_t 	  offset;    /* 资源相对于基地址的偏移 */
	}CatalogTypeDef;
	/* 资源文件名称 */
	static const char FONT_XINSONGTI_18_ADDR[] = "xinsongti18.xbf";
	static const char FONT_SIYUANHEITI_36_ADDR[] = "siyuanheiti36_2bpp.xbf";

#elif (XBF_FONT_SOURCE == USE_SDCARD_FONT)

	static const char FONT_STORAGE_ROOT_DIR[]  =   "0:";
	static const char FONT_XINSONGTI_18_ADDR[] = 	 "0:/Font/新宋体18.xbf";
	static const char FONT_SIYUANHEITI_36_ADDR[] = "0:/Font/思源黑体36_2bpp.xbf";

#endif

/* 字库存储在文件系统时需要使用的变量 */
#if (XBF_FONT_SOURCE == USE_SDCARD_FONT)
	static FIL fnew;									/* file objects */
	static FATFS fs;									/* Work area (file system object) for logical drives */
	static FRESULT res; 
	static UINT br;            				/* File R/W count */
#endif

#if (XBF_FONT_SOURCE == USE_FLASH_FONT)
/**
  * @brief  从FLASH中的目录查找相应的资源位置
  * @param  res_base 目录在FLASH中的基地址
  * @param  res_name[in] 要查找的资源名字
  * @retval -1表示找不到，其余值表示资源在FLASH中的基地址
  */
int GetResOffset(const char *res_name)
{
	int i,len;
	CatalogTypeDef Catalog;
	
	len =strlen(res_name);
	for(i=0;i<CATALOG_SIZE;i+=sizeof(CatalogTypeDef))
	{
		taskENTER_CRITICAL();//进入临界区
		SPI_FLASH_BufferRead((uint8_t*)&Catalog,RESOURCE_BASE_ADDR+i,sizeof(CatalogTypeDef));
    taskEXIT_CRITICAL();//退出临界区
		
		if(strncasecmp(Catalog.name,res_name,len)==0)
		{
			return Catalog.offset;
		}
	}
	return -1;
}
#endif

/**
  * @brief  获取字体数据的回调函数
  * @param  Offset：要读取的内容在XBF文件中的偏移位置
  * @param  NumBytes：要读取的字节数
	* @param  pVoid：自定义数据的指针
  * @param  pBuffer：存储读取内容的指针
  * @retval 0 成功, 1 失败
  */
static int _cb_FONT_XBF_GetData(U32 Offset, U16 NumBytes, void * pVoid, void * pBuffer)
{
#if (XBF_FONT_SOURCE == USE_FLASH_FONT)
	int32_t FontOffset;
	uint32_t FONT_BASE_ADDR;
	
	if (storage_init_flag == 0)
	{
		/* 初始化SPI FLASH */
		SPI_FLASH_Init();
		storage_init_flag = 1;
	}
	
	/* 从pVoid中获取字库在FLASH中的偏移量(pvoid的值在GUI_XBF_CreateFont中传入) */
	FontOffset = GetResOffset(pVoid);
	if(FontOffset == -1)
	{
		printf("无法在FLASH中找到字库文件\r\n");
		while(1);
	}
	
	/* 字库的实际地址=资源目录地址+字库相对于目录的偏移量 */
	FONT_BASE_ADDR = RESOURCE_BASE_ADDR + FontOffset;
	
	/* 读取内容 */
	SPI_FLASH_BufferRead(pBuffer,FONT_BASE_ADDR+Offset,NumBytes);
	
	return 0;
	
#elif (XBF_FONT_SOURCE == USE_SDCARD_FONT)
	
	if (storage_init_flag == 0)
	{
		/* 挂载sd卡文件系统 */
		res = f_mount(&fs,FONT_STORAGE_ROOT_DIR,1);
		storage_init_flag = 1;
	}
	
	/* 从pVoid中获取字库的存储地址(pvoid的值在GUI_XBF_CreateFont中传入) */
	res = f_open(&fnew , (char *)pVoid, FA_OPEN_EXISTING | FA_READ);

	if (res == FR_OK) 
	{
		f_lseek (&fnew, Offset);/* 指针偏移 */
		/* 读取内容 */
		res = f_read( &fnew, pBuffer, NumBytes, &br );		 
		f_close(&fnew);
		return 0;  
	}    
	else
	  return 1; 
#endif
}

/**
  * @brief  创建XBF字体
  * @param  无
  * @retval 无
  */
void Create_XBF_Font(void) 
{
	/* 新宋体18 */
	GUI_XBF_CreateFont(&FONT_XINSONGTI_18,              /* GUI_FONT 字体结构体指针 */
					           &XBF_XINSONGTI_18_Data,          /* GUI_XBF_DATA 结构体指针 */
					           GUI_XBF_TYPE_PROP_EXT,           /* 字体类型 */
					           _cb_FONT_XBF_GetData,            /* 获取字体数据的回调函数 */
					           (void *)&FONT_XINSONGTI_18_ADDR);/* 要传输给回调函数的自定义数据指针，此处传输字库的地址 */
	/* 思源黑体36 */
	GUI_XBF_CreateFont(&FONT_SIYUANHEITI_36,              /* GUI_FONT 字体结构体指针 */
					           &XBF_SIYUANHEITI_36_Data,          /* GUI_XBF_DATA 结构体指针 */
					           GUI_XBF_TYPE_PROP_AA2_EXT,           /* 字体类型 */
					           _cb_FONT_XBF_GetData,            /* 获取字体数据的回调函数 */
					           (void *)&FONT_SIYUANHEITI_36_ADDR);/* 要传输给回调函数的自定义数据指针，此处传输字库的地址 */
}

/*********************************************END OF FILE**********************/
