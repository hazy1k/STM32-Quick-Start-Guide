实验器材:
	探索者STM32F4开发板
	
实验目的:
	学习STM32定时器通过PWM输出电压,可用于SPWM实现(按正弦规律输出电压,就是SPWM了)
	
硬件资源:
	1,DS0(连接在PF9)
	2,串口1(波特率:115200,PA9/PA10连接在板载USB转串口芯片CH340上面)
	3,ALIENTEK 2.8/3.5/4.3/7寸TFTLCD模块(通过FSMC驱动,FSMC_NE4接LCD片选/A6接RS) 
	4,按键KEY1(PE3)/KEY_UP(PA0,也称之为WK_UP)
	5,PWM DAC(定时器9通道2输出PWM,通过PA3输出,经过二阶RC滤波后输出电压)
	6,ADC(STM32内部ADC1_CH5,连接在PA5).

实验现象:
	本实验利用按键（KEY1/KEY_UP两个按键,或USMART）控制STM32F4的TIM9_CH2的PWM输出，经过二阶RC滤波
	后转换为DAC输出，通过ADC1的通道5采集PWM DAC的输出电压，在LCD模块上面显示ADC获取到的电压值以及
	PWM DAC的设定输出电压值等信息。同时可以通过usmart调用PWM_DAC_Set函数，来直接设置PWM DAC的输出
	电压。
	  
注意事项: 
	1,4.3寸和7寸屏需要比较大电流,USB供电可能不足,请用外部电源适配器(推荐外接12V 1A电源).
	2,本例程在LCD_Init函数里面(在ILI93xx.c),用到了printf,如果不初始化串口1,将导致液晶无法显示!! 
	3,ADC和DAC的参考电压默认通过P7连接在VDDA上面(3.3V),所以默认参考电压是3.3V  
	4,通过跳线帽短接多功能接口(P12)的ADC和PDC,即可实现自测试(AD读取PWM DAC值,显示在LCD上) 
	5,务必要拔了P9，PA3(RX)的跳线帽（右侧跳线帽），否则影响转换结果。 


					正点原子@ALIENTEK
					2014-10-25
					广州市星翼电子科技有限公司
					电话：020-38271790
					传真：020-36773971
					购买：http://shop62103354.taobao.com
					http://shop62057469.taobao.com
					公司网站：www.alientek.com
					技术论坛：www.openedv.com
					
					
					
					
					
					
					
					
					
					
					
					
					
					
					
					
					