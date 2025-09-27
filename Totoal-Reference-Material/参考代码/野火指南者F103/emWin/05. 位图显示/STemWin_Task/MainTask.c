/**
  *********************************************************************
  * @file    MainTask.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   FreeRTOS v9.0.0 + STM32 工程模版
  *********************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F103 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
/*******************************************************************************
 * 包含的头文件
 ******************************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <string.h>
/* FreeRTOS头文件 */
#include "FreeRTOS.h"
#include "task.h"
/* STemWIN头文件 */
#include "ScreenShot.h"
#include "MainTask.h"
#include "DIALOG.h"

/*******************************************************************************
 * 全局变量
 ******************************************************************************/
extern GUI_CONST_STORAGE GUI_BITMAP bmngc7293;

UINT    f_num;
extern FATFS   fs;								/* FatFs文件系统对象 */
extern FIL     file;							/* file objects */
extern FRESULT result; 
extern DIR     dir;
/*******************************************************************************
 * 函数
 ******************************************************************************/
/**
  * @brief 从内部存储器中读取并绘制BMP图片数据
  * @note 无
  * @param 无
  * @retval 无
  */
static void ShowBitmap(void)
{
  for(int y = 0; y < 240; y += bmngc7293.YSize)
  {
    for(int x = 0; x < 320; x += bmngc7293.XSize)
    {
       GUI_DrawBitmap(&bmngc7293, x, y);
    }
  }
}

/**
  * @brief 从外部存储器中读取并绘制BMP图片数据
  * @note 无
  * @param sFilename：要读取的文件名
  *        x：要显示的x轴坐标
  *        y：要显示的y轴坐标
  * @retval 无
  */
static void ShowStreamedBitmap(const char *sFilename, int x, int y)
{
 	WM_HMEM hMem;
  char *_acbuffer = NULL;

	/* 进入临界段 */
	taskENTER_CRITICAL();
	/* 打开图片 */
	result = f_open(&file, sFilename, FA_READ);
	if ((result != FR_OK))
	{
		printf("文件打开失败！\r\n");
		_acbuffer[0]='\0';
	}
	
	/* 申请一块动态内存空间 */
	hMem = GUI_ALLOC_AllocZero(file.fsize);
	/* 转换动态内存的句柄为指针 */
	_acbuffer = GUI_ALLOC_h2p(hMem);

	/* 读取图片数据到动态内存中 */
	result = f_read(&file, _acbuffer, file.fsize, &f_num);
	if(result != FR_OK)
	{
		printf("文件读取失败！\r\n");
	}
	/* 读取完毕关闭文件 */
	f_close(&file);
	/* 退出临界段 */
	taskEXIT_CRITICAL();
	/* 绘制流位图 */
	GUI_DrawStreamedBitmapAuto(_acbuffer, x, y);
  
	/* 释放内存 */
	GUI_ALLOC_Free(hMem);
}

/**
  * @brief GUI主任务
  * @note 无
  * @param 无
  * @retval 无
  */
void MainTask(void)
{	
  ShowBitmap();
  
  GUI_Delay(2000);
//  GUI_Clear();
  /* 由于F103指南者的内存较小，无法显示较大分辨率的流位图，
	低分辨力图片显示效果极其不佳，故不进行流位图演示*/
//  ShowStreamedBitmap("0:/image/illustration.dta",
//                     0,
//                     0);
  
	while(1)
	{
    GUI_Delay(2000);
	}
}
