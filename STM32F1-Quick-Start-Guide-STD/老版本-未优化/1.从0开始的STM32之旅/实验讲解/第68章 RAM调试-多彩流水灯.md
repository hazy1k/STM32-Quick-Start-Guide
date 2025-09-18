# 第六十八章 RAM调试-多彩流水灯

## 1. 硬件设计

本小节中使用到的流水灯硬件不再介绍，主要讲解与SRAM调试相关的硬件配置。在SRAM上调试程序， 需要修改STM32芯片的启动方式

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/SRAM005.jpg)

在我们的实验板左侧有引出STM32芯片的BOOT0和BOOT1引脚，可使用跳线帽设置它们的电平从而控制芯片的启动方式， 它支持从内部FLASH启动、系统存储器启动以及内部SRAM启动方式。

本实验在SRAM中调试代码，因此把BOOT0和BOOT1引脚都使用跳线帽连接到3.3V，使芯片从SRAM中启动。

## 2. 软件设计

### 2.1 编程大纲

本实验的工程从普通的多彩流水灯工程改写而来，主要修改了分散加载文件及一些程序的下载选项。

(1) 在原工程的基础上创建一个调试版本；

(2) 修改分散加载文件，使链接器把代码分配到内部SRAM空间；

(3) 添加宏修改STM32的向量表地址；

(4) 修改仿真器和下载器的配置，使程序能通过下载器存储到内部SRAM；

(5) 根据使用情况选择是否需要使用仿真器命令脚本文件*.ini；

(6) 尝试给SRAM下载程序或仿真调试。

### 2.2 代码分析

```c
#include "stm32f10x.h"
#include "bsp_led.h"

#define SOFT_DELAY Delay(0x0FFFFF);

void Delay(__IO u32 nCount); 

int main(void)
{	
	/* LED ¶Ë¿Ú³õÊ¼»¯ */
	LED_GPIO_Config();	 

	while (1)
	{
		LED1_ON;			  // ÁÁ
		SOFT_DELAY;
		LED1_OFF;		   // Ãð

		LED2_ON;			 // ÁÁ
		SOFT_DELAY;
		LED2_OFF;		   // Ãð

		LED3_ON;			 // ÁÁ
		SOFT_DELAY;
		LED3_OFF;		   // Ãð	 

		/*ÂÖÁ÷ÏÔÊ¾ ºìÂÌÀ¶»Æ×ÏÇà°× ÑÕÉ«*/
		LED_RED;
		SOFT_DELAY;
		
		LED_GREEN;
		SOFT_DELAY;
		
		LED_BLUE;
		SOFT_DELAY;
		
		LED_YELLOW;
		SOFT_DELAY;
		
		LED_PURPLE;
		SOFT_DELAY;
				
		LED_CYAN;
		SOFT_DELAY;
		
		LED_WHITE;
		SOFT_DELAY;
		
		LED_RGBOFF;
		SOFT_DELAY;		
	}
}

void Delay(__IO uint32_t nCount)	 //¼òµ¥µÄÑÓÊ±º¯Êý
{
	for(; nCount != 0; nCount--);
}

```

用USB线连接开发板“USB TO UART”接口跟电脑，把BOOT0及BOOT1引脚使用跳帽连接到低电平，在电脑端打开串口调试助手， 点击下载按钮，把编译好的程序下载到芯片的内部SRAM中，复位运行，观察流水灯是否正常闪烁；给开发板断电再重新上电，观察程序是否还能正常运行。
