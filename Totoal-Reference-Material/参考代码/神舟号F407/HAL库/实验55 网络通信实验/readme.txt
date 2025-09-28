实验器材:
	探索者STM32F4开发板
	
实验目的:
	学习内存管理知识,方便大工程应用.
	
硬件资源:
	1,DS0(连接在PF9)
	2,串口1(波特率:115200,PA9/PA10连接在板载USB转串口芯片CH340上面)
	3,ALIENTEK 2.8/3.5/4.3/7寸TFTLCD模块(通过FSMC驱动,FSMC_NE4接LCD片选/A6接RS)
	4,按键KEY0(PE4)/KEY1(PE3)/KEY2(PE2)/KEY_UP(PA0,也称之为WK_UP)
	5,外部SRAM(IS62WV51216,通过FSMC驱动,FSMC_NE3接SRAM) 

实验现象:
	本实验开机后，显示提示信息，等待外部输入。KEY0用于申请内存，每次申请2K字节内存。KEY1用于写数据
	到申请到的内存里面。KEY2用于释放内存。KEY_UP用于切换操作内存区（内部SRAM内存/外部SRAM内存/CCM
	内存）。	DS0用于指示程序运行状态。本章我们还可以通过USMART调试，测试内存管理函数。 
   
注意事项: 
	1,4.3寸和7寸屏需要比较大电流,USB供电可能不足,请用外部电源适配器(推荐外接12V 1A电源).
	2,本例程在LCD_Init函数里面(在ILI93xx.c),用到了printf,如果不初始化串口1,将导致液晶无法显示!!  
	
	 
					正点原子@ALIENTEK
					2014-10-25
					广州市星翼电子科技有限公司
					电话：020-38271790
					传真：020-36773971
					购买：http://shop62103354.taobao.com
					http://shop62057469.taobao.com
					公司网站：www.alientek.com
					技术论坛：www.openedv.com
					
					
					