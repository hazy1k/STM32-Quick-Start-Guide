 /**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   用于从SD卡拷贝数据文件到FLASH的工程
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 F103-指南者 STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
#include "stm32f10x.h"
#include "./usart/bsp_usart.h"
#include "./led/bsp_led.h"
#include "./flash/bsp_spi_flash.h"
#include "./key/bsp_key.h"  
#include "./FATFS/ff.h"
#include "./res_mgr/res_mgr.h"


/**
  ******************************************************************************
  *                              定义变量
  ******************************************************************************
  */
extern FATFS sd_fs;													/* Work area (file system object) for logical drives */
extern FATFS flash_fs;

//要复制的文件路径，到aux_data.c修改
extern char src_dir[];

/*
 * 函数名：main
 * 描述  ：主函数
 * 输入  ：无
 * 输出  ：无
 */
int main(void)
{ 	
  FRESULT res = FR_OK;
  
	/* 初始化LED */
  LED_GPIO_Config();
  
  Key_GPIO_Config();

  /* 初始化调试串口，一般为串口1 */
  USART_Config();
  
  SPI_FLASH_Init();
  
  res = FR_DISK_ERR;
       
  //挂载SD卡
  res = f_mount(&sd_fs,SD_ROOT,1);

  //如果文件系统挂载失败就退出
  if(res != FR_OK)
  {
    BURN_ERROR("f_mount ERROR!请给开发板插入SD卡然后重新复位开发板!");
    LED_RED;
    while(1);
  }    
    
  printf("\r\n 按一次KEY1开始烧写字库并复制文件到FLASH。 \r\n"); 
  printf("\r\n 注意该操作会把FLASH的原内容会被删除！！ \r\n"); 
  
  while(Key_Scan(GPIOA,GPIO_Pin_0)==0);
  
  printf("\r\n 正在进行整片擦除，时间很长，请耐心等候...\r\n");
  SPI_FLASH_BulkErase();    
  
  /* 生成烧录目录信息文件 */
  Make_Catalog(src_dir,0);
  
  /* 烧录 目录信息至FLASH*/
  Burn_Catalog();  
  /* 根据 目录 烧录内容至FLASH*/
  Burn_Content();
  /* 校验烧录的内容 */
  Check_Resource();
  
//  printf("读取资源位置测试\r\n");
//  printf("xinsongti24.xbf offset = %d\r\n", 
//                  GetResOffset("xinsongti24.xbf"));

  while(1);
}

/*********************************************END OF FILE**********************/
