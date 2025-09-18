# 第六十四章 USB-外部flash&SD卡模拟U盘

## 1. 硬件设计

### 1.1 选定硬件平台

- **MCU**：STM32F103（具备 USB 全速（Full-speed）功能的微控制器）
- **外部存储设备**：
  - **USB Flash**：连接到 STM32F103 的 USB 接口。
  - **SD 卡**：连接到 STM32F103 的 SDIO 接口或 SPI 接口。
- **电源**：为 STM32F103 和外部设备提供足够的电源，通常是 5V 和 3.3V 电源。

### 1.2 硬件接口

- **USB接口**：连接 STM32F103 的 USB 引脚（D+ 和 D-）与外部 USB 端口，作为 U 盘接口。
- **SD 卡接口**：SD 卡通过 SDIO 接口连接到 STM32F103，或者可以使用 SPI 模式连接。
- **外部 Flash**：可以通过 SPI 或 parallel 接口与 STM32F103 连接。

## 2. 软件设计

为了模拟一个 U 盘设备，STM32F103 需要作为 USB 设备（USB Mass Storage）运行，并能够与 SD 卡和外部 Flash 进行数据交换。

### 2.1 配置 USB 外设

使用 STM32F103 的 USB 外设，通过 USB 协议与主机（PC）通信。USB 设备模式下，使用 STM32 的 USB 控制器来模拟 U 盘设备。

#### 2.1.1 USB 驱动框架

- 使用 STM32 官方提供的 **USB Host/Device Library**。这个库包含了 USB 协议栈和各类 USB 设备的驱动程序。STM32CubeMX 也提供了相关的配置工具，可以自动生成 USB 设备（Mass Storage）所需的驱动代码。

### 2.2 USB 协议模拟

- **Mass Storage Class（MSC）协议**：这是 U 盘的标准协议。你需要使用 MSC 类设备驱动来模拟一个标准的 U 盘接口。
- **初始化 USB 设备**：配置 STM32F103 为 USB Mass Storage 设备，处理 USB 数据传输。
- **数据交换**：当主机（PC）通过 USB 进行读写操作时，STM32 会根据读取的地址从 SD 卡或外部 Flash 中读取数据，或将数据写入这些存储设备。

### 2.3 SD 卡接口

- **SDIO 或 SPI接口**：SD 卡可以通过 SDIO 或 SPI 接口与 STM32F103 连接。
  - 如果使用 **SDIO**，需要配置 SDIO 外设以支持高速度的数据传输。
  - 如果使用 **SPI**，则需要配置 SPI 外设与 SD 卡通信。
- **文件系统**：使用 FATFS 文件系统，FATFS 是一个开放的文件系统库，可以在嵌入式设备中实现对 SD 卡和 Flash 的读写操作。

### 2.4 外部 Flash

- 使用 SPI 接口连接外部 Flash（例如 SPI NAND Flash 或 SPI NOR Flash）。
- **驱动**：你需要为外部 Flash 编写 SPI 通信驱动，以及提供文件读写接口，和 SD 卡接口一样，使用 FATFS 文件系统。

### 2.5 任务管理

在 STM32 中，可能需要使用 FreeRTOS（或其他实时操作系统）来管理 USB、SD 卡和外部 Flash 的读写操作，特别是在需要高效多任务调度时。

## 3. 主函数

```c

#include "stm32f10x.h"
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
	Set_System();
	Set_USBClock();
	USB_Interrupts_Config();
 	USB_Init();
 	while (bDeviceState != CONFIGURED);	 //µÈ´ýÅäÖÃÍê³É
  	while (1)
  	{
    	LED1_ON();
    	USB_Delay(0x0FFFFF);
  }
}

```


