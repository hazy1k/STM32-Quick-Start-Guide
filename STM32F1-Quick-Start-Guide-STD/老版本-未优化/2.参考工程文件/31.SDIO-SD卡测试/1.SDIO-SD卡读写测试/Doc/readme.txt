【*】程序简介
-工程名称：SDIO—SD卡
-实验平台: 野火STM32 指南者 开发板 
-MDK版本：5.16
-ST固件库版本：3.5
【 ！】功能简介：
使用SDIO接口对SD卡进行基本读写(不含文件系统)
学习目的：学习STM32的SDIO驱动SD卡。
【 ！】实验操作：
在开始进行SD卡基本测试前，请给开发板插入32G以内的SD卡。		
电脑端使用串口调试助手，选择电脑与STM32相连的COM口，设置为115200-N-8-1，
复位开发板，即可接收STM32串口发送给电脑的调试信息。	
若已确认，请按开发板的KEY1按键，开始SD卡测试实验。
【*】 引脚分配
SDIO(microSD卡座，支持32G以内的SD卡)：
microtSD卡座的SDIO接口与STM32的SDIO相连。
		SDIO_D0	<--->PC8
		SDIO_D1	<--->PC9
		SDIO_D2	<--->PC10
		SDIO_D3	<--->PC11
		SDIO_CK	<--->PC12
		SDIO_CMD<--->PD2
串口（TTL-USB TO UART）：
CH340的收发引脚与STM32的发收引脚相连。
	CH340  RXD  <----->  USART1  TX  (PA9)
	CH340  TXD  <----->  USART1  RX  (PA10)
按键：
	KEY1<--->PA0
				开始时，按键未按下时处于低电平状态
	KEY2<--->PC13
LED：
	LED_R<-----> PB5
	LED_G<-----> PB0
	LED_B<-----> PB1  对应引脚低电平则LED灯亮