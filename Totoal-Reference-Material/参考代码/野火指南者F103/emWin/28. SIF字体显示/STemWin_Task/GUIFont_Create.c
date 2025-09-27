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


/* 字库结构体 */
GUI_FONT     	FONT_SIYUANHEITI_20_4BPP;
GUI_FONT     	FONT_XINSONGTI_16_4BPP;

/* 字库缓冲区 */
uint8_t *SIFbuffer20;
uint8_t *SIFbuffer16;

/* 字库存储路径 */
#if (SIF_FONT_SOURCE == USE_FLASH_FONT)

	/* 资源烧录到的FLASH基地址（目录地址） */
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
	
	CatalogTypeDef Catalog;
	
	static const char  FONT_XINSONGTI_18_ADDR[]	 = "xinsongti18_4bpp.sif";
	static const char  FONT_SIYUANHEITI_36_ADDR[]	 = "siyuanheit36_4bpp.sif";

#elif (SIF_FONT_SOURCE == USE_SDCARD_FONT)

	static const char FONT_STORAGE_ROOT_DIR[]  =   "0:";
	static const char FONT_XINSONGTI_16_ADDR[] = 	 "0:/Font/16_4bpp.sif";
	static const char FONT_SIYUANHEITI_20_ADDR[] = 	 "0:/Font/20_4bpp.sif";

#endif

/* 存储器初始化标志 */
static uint8_t storage_init_flag = 0;

/* 字库存储在文件系统时需要使用的变量 */
#if (SIF_FONT_SOURCE == USE_SDCARD_FONT)
	static FIL fnew;									  /* file objects */
	static FATFS fs;									  /* Work area (file system object) for logical drives */
	static FRESULT res;
	static UINT br;            			    /* File R/W count */
#endif

#if (SIF_FONT_SOURCE == USE_FLASH_FONT)
/**
  * @brief  从FLASH中的目录查找相应的资源位置
  * @param  res_base 目录在FLASH中的基地址
  * @param  res_name[in] 要查找的资源名字
  * @retval -1表示找不到，其余值表示资源在FLASH中的基地址
  */
int GetResOffset(const char *res_name)
{
	int i,len;

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
  * @brief  加载字体数据到SDRAM
  * @note 无
  * @param  res_name：要加载的字库文件名
  * @retval Fontbuffer：已加载好的字库数据
  */
void *FONT_SIF_GetData(const char *res_name)
{
	uint8_t *Fontbuffer;
	GUI_HMEM hFontMem;
	
#if (SIF_FONT_SOURCE == USE_FLASH_FONT)
	
	int32_t offset;
	
	if (storage_init_flag == 0)
	{
		/* 初始化SPI FLASH */
		SPI_FLASH_Init();
		storage_init_flag = 1;
	}
	
	/* 获取字库文件在FLASH中的偏移量 */
	offset = GetResOffset(res_name);
	if(offset == -1)
	{
		printf("无法在FLASH中找到字库文件\r\n");
		while(1);
	}
	
	/* 申请一块动态内存空间 */
	hFontMem = GUI_ALLOC_AllocZero(Catalog.size);
	/* 转换动态内存的句柄为指针 */
	Fontbuffer = GUI_ALLOC_h2p(hFontMem);
	
	/* 读取内容 */
	SPI_FLASH_BufferRead(Fontbuffer, RESOURCE_BASE_ADDR+offset, Catalog.size);
	
	return Fontbuffer; 
	
#elif (SIF_FONT_SOURCE == USE_SDCARD_FONT)
	
	if (storage_init_flag == 0)
	{
		/* 挂载sd卡文件系统 */
		res = f_mount(&fs,FONT_STORAGE_ROOT_DIR,1);
		storage_init_flag = 1;
	}
	
	/* 打开字库 */
	res = f_open(&fnew , res_name, FA_OPEN_EXISTING | FA_READ);
	if(res != FR_OK)
	{
		printf("Open font failed! res = %d\r\n", res);
		while(1);
	}
	
	/* 申请一块动态内存空间 */
	hFontMem = GUI_ALLOC_AllocZero(fnew.fsize);
	/* 转换动态内存的句柄为指针 */
	Fontbuffer = GUI_ALLOC_h2p(hFontMem);

	/* 读取内容 */
	res = f_read(&fnew, Fontbuffer, fnew.fsize, &br);
	if(res != FR_OK)
	{
		printf("Read font failed! res = %d\r\n", res);
		while(1);
	}
	f_close(&fnew);
	
	return Fontbuffer;  
#endif
}

/**
  * @brief  创建SIF字体
  * @param  无
  * @retval 无
  */
void Create_SIF_Font(void) 
{
	/* 获取字体数据 */
	SIFbuffer16 = FONT_SIF_GetData(FONT_XINSONGTI_16_ADDR);
	SIFbuffer20 = FONT_SIF_GetData(FONT_SIYUANHEITI_20_ADDR);
	
	/* 新宋体18 */
	GUI_SIF_CreateFont(SIFbuffer16,               /* 已加载到内存中的字体数据 */
	                   &FONT_XINSONGTI_16_4BPP,   /* GUI_FONT 字体结构体指针 */
										 GUI_SIF_TYPE_PROP_AA4_EXT);/* 字体类型 */
	/* 思源黑体36 */
	GUI_SIF_CreateFont(SIFbuffer20,               /* 已加载到内存中的字体数据 */
	                   &FONT_SIYUANHEITI_20_4BPP, /* GUI_FONT 字体结构体指针 */
										 GUI_SIF_TYPE_PROP_AA4_EXT);/* 字体类型 */

}

/*********************************************END OF FILE**********************/
