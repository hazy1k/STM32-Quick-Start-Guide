# 第六十三章 USB-SD卡模拟U盘

## 1. **硬件连接**

- STM32F103 USB 端口连接到主机端（计算机）模拟 U 盘。
- 使用 SD 卡模块连接 STM32F103，连接方式可以使用 SPI 或 SDIO。

## 2. **基本步骤**

- 配置 USB 外设，初始化 USB 设备控制器，使用标准库的 USB 驱动程序。
- 配置 SD 卡接口，使用标准库的 SPI 或 SDIO 驱动来操作 SD 卡。
- 使用 FatFs 文件系统访问 SD 卡。
- 实现 USB MSC（Mass Storage Class）协议来模拟 U 盘。

## 3. 主函数

```c
#include "stm32f10x.h"
#include "sdio_sdcard.h"
#include "usart.h"
#include "led.h"  
#include "hw_config.h" 
#include "usb_lib.h"
#include "usb_pwr.h"

void USB_Delay(__IO uint32_t nCount)
{
	for(; nCount != 0; nCount--);
}

int main(void)
{
	USART_Config();
	LED_Init();
	Set_System(); // Set system clock to 72MHz
	Set_USBClock(); // Set USB clock to 48MHz
	USB_Interrupts_Config(); // Configure USB interrupts
 	USB_Init(); // Initialize USB
 	while(bDeviceState != CONFIGURED); // Wait for USB device to be configured	
	printf("\r\n STM32 USB TEST\r\n");
	while (1)
  	{
    	LED1_ON();
    	USB_Delay(0x0FFFFF);
  }
}

```


