#ifndef __FATFS_TEST_H__
#define __FATFS_TEST_H__

/*
*************************************************************************
*                             包含的头文件
*************************************************************************
*/


/* 开发板硬件bsp头文件 */
#include "stm32f10x.h"
#include "led/bsp_led.h" 
#include "./flash/bsp_spi_flash.h"
#include "ff.h"



/*
*************************************************************************
*                             变量声明
*************************************************************************
*/

extern FATFS fs;													/* FatFs文件系统对象 */
//FIL fnew;													/* 文件对象 */
extern FRESULT res_flash;                /* 文件操作结果 */
//UINT fnum;            					  /* 文件成功读写数量 */
//BYTE ReadBuffer[1024];        /* 读缓冲区 */
//extern BYTE WriteBuffer; 

//FATFS fs;													/* FatFs文件系统对象 */
//FIL fnew;													/* 文件对象 */
//FRESULT res_flash;                /* 文件操作结果 */
//UINT fnum;            					  /* 文件成功读写数量 */
//BYTE ReadBuffer[1024]={0};        /* 读缓冲区 */
//BYTE WriteBuffer[] =              /* 写缓冲区*/
//"欢迎使用秉火STM32开发板 今天是个好日子，新建文件系统测试文件\r\n"; 

/*
*************************************************************************
*                               函数声明
*************************************************************************
*/

void FileSystem_Init(void);

void FileSystem_Test(void);




#endif /* __FATFS_TEST_H__ */





