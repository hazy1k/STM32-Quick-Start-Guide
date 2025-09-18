# 第六十五章 USB-外部flash模拟U盘

## 1. 硬件设计

### 1.1 选择硬件平台

- **MCU**：STM32F103（具有 USB 2.0 全速设备接口）
- **外部 Flash 存储**：外部 Flash 可以使用 SPI 接口连接，常见的外部 Flash 包括 **SPI NOR Flash** 或 **SPI NAND Flash**。
- **电源**：为 STM32F103 和外部 Flash 提供稳定的电源，通常是 3.3V 和 5V 电源。
- **连接**：
  - **USB**：STM32F103 的 USB 引脚（D+ 和 D-）连接到 PC 或主机，模拟一个 U 盘设备。
  - **外部 Flash**：通过 **SPI** 接口连接外部 Flash 存储。

### 1.2 硬件接口

- **USB接口**：STM32F103 的 USB 2.0 引脚（D+ 和 D-）连接至 PC，模拟 USB Mass Storage 设备。
- **SPI接口**：连接外部 Flash 存储，通常是 NOR Flash。
- **外部 Flash**：使用 SPI 协议与 STM32F103 进行通信，控制外部 Flash 读写。

## 2. 软件设计

### 2.1 USB 协议实现

为了使 STM32F103 作为一个 USB 外设模拟 U 盘，我们需要实现 **USB Mass Storage** 协议。此协议是 U 盘与主机之间的标准数据交换协议。

1. **初始化 USB 设备**：STM32F103 需要初始化为 **USB Mass Storage** 设备，模拟 U 盘的接口。
2. **处理 USB 命令**：接收来自主机的 **CBW**（Command Block Wrapper）和 **CSW**（Command Status Wrapper），根据这些命令从外部 Flash 读取或写入数据。
3. **数据传输**：根据主机的读写请求，STM32F103 将数据从外部 Flash 中读取或写入，并通过 USB 传输给主机。

### 2.2 外部 Flash 存储管理

外部 Flash 存储通过 **SPI** 接口与 STM32F103 进行连接。需要编写 SPI 驱动来实现对外部 Flash 的读写操作。

1. **SPI 初始化**：配置 STM32F103 的 SPI 外设进行数据传输。
2. **读取和写入操作**：通过 SPI 协议实现对外部 Flash 存储器的控制。
3. **文件系统**：使用 **FATFS** 文件系统，将外部 Flash 显示为一个磁盘，并允许读写操作。

### 2.3 文件系统管理

使用 **FATFS** 文件系统来管理外部 Flash 中的数据，处理文件的打开、读取、写入等操作。STM32F103 通过 FATFS 库管理外部 Flash 的数据。

## 3. 主函数

```c
#include "stm32f10x.h"
#include "fatfs_flash_spi.h"
#include "usart.h"	
#include "led.h"  
#include "hw_config.h" 
#include "usb_lib.h"
#include "usb_pwr.h"

static void USB_Delay(__IO uint32_t nCount)
{
	for(; nCount != 0; nCount--);
}

int main(void)
{
	USART_Config();	
  LED_Init();
	Set_System();
	Set_USBClock();
	USB_Interrupts_Config();
 	USB_Init();
 	while (bDeviceState != CONFIGURED);	
  while (1)
  {
    LED2_ON();
    USB_Delay(0x0FFFFF);
  }
}

```


