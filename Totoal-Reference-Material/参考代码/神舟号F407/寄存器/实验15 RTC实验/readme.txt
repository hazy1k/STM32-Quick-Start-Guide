实验器材:
	探索者STM32F4开发板
	
实验目的:
	学习STM32自带RTC的使用
	
硬件资源:
	1,DS0(连接在PF9)
	2,串口1(波特率:115200,PA9/PA10连接在板载USB转串口芯片CH340上面)
	3,ALIENTEK 2.8/3.5/4.3/7寸TFTLCD模块(通过FSMC驱动,FSMC_NE4接LCD片选/A6接RS) 
	4,RTC(实时时钟)
	
实验现象:
	本实验通过TFTLCD显示RTC时间，并可以通过usmart设置RTC时间/闹钟,从而调节时间,或设置RTC闹钟,
	以便大家验证RTC功能。 
	
注意事项: 
	1,4.3寸和7寸屏需要比较大电流,USB供电可能不足,请用外部电源适配器(推荐外接12V 1A电源).
	2,本例程在LCD_Init函数里面(在ILI93xx.c),用到了printf,如果不初始化串口1,将导致液晶无法显示!!
	3,串口波特率为115200
	4,请用USB线连接在USB_232,找到USB转串口后测试本例程.
	5,P6的PA9/PA10必须通过跳线帽连接在RXD/TXD上.  
 

					正点原子@ALIENTEK
					2014-10-25
					广州市星翼电子科技有限公司
					电话：020-38271790
					传真：020-36773971
					购买：http://shop62103354.taobao.com
					http://shop62057469.taobao.com
					公司网站：www.alientek.com
					技术论坛：www.openedv.com
					
					
					
					
					
					
					
					
					
					
					
					
					