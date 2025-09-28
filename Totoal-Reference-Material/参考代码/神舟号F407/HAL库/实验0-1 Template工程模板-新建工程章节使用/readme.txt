实验器材:
	阿波罗STM32F429开发板 
	
实验目的:
	本实验为新建工程实验，仅供大家新建工程时参考。
	新建工程详细步骤，请看《STM32F429开发指南-库函数版本》第3.3节。
	
硬件资源:
	1,串口1(波特率:115200,PA9/PA10连接在板载USB转串口芯片CH340上面)
	
实验现象:
	本实验下载后，DS0和DS1闪烁。 

工程文件来源：

①startup_stm32f429xx.s                                                     ->   \STM32Cube_FW_F4_V1.10.0\Drivers\CMSIS\Device\ST\STM32F4xx\Source\Templates\arm
②cmsis_armcc.h，core_cm4.h，core_cmFunc.h，core_cmInstr.h ，core_cmSimd.h  ->   \STM32Cube_FW_F4_V1.10.0\Drivers\CMSIS\Include

③stm32f4xx.h，stem_stm32f4xx.h和stm32f429xx.                               ->   \STM32Cube_FW_F4_V1.10.0\Drivers\CMSIS\Device\ST\STM32F4xx\Include

④stm32f4xx_it.h，stm32f4xx_hal_conf.h 和main.h                             ->   \STM32Cube_FW_F4_V1.10.0\Projects\STM32F429I-Discovery\Templates\Inc
⑤system_stm32f4xx.c，stm32f4xx_it.c, stm32f4xx_hal_msp.c 和main.c          ->   \STM32Cube_FW_F4_V1.10.0\Projects\STM32F429I-Discovery\Templates\Src

SYSTEM文件夹，任何一个HAL库实验工程中有
	
注意事项:
	无.


					正点原子@ALIENTEK
					2016-6-24
					广州市星翼电子科技有限公司
					电话：020-38271790
					传真：020-36773971
					购买：http://shop62103354.taobao.com
					http://shop62057469.taobao.com
					公司网站：www.alientek.com
					技术论坛：www.openedv.com