实验器材:
	探索者STM32F4开发板
	
实验目的:
	学习ATK-HC05蓝牙串口模块的使用，实现蓝牙串口通信
	
硬件资源:
	1,DS0(连接在PF9),DS1(连接在PF10)
	2,串口1(波特率:115200,PA9/PA10连接在板载USB转串口芯片CH340上面)
	3,ALIENTEK 2.8/3.5/4.3/7寸TFTLCD模块(通过FSMC驱动,FSMC_NE4接LCD片选/A6接RS) 
	4,按键KEY0(PE4),KEY_UP(PA0,也称之为WK_UP)
	5,串口3(波特率:9600,PB10/PB11连接ATK-HC05蓝牙模块)
	6,ATK MODULE接口&ATK-HC05蓝牙模块(模块直接插ATK MODULE接口即可),连接关系如下: 
	STM32开发板-->ATK-HC05蓝牙模块
		   PB10-->RXD
		   PB11-->TXD
			PF6-->KEY
			PC0-->LED
			GND-->GND
		5V/3.3V-->VCC 

实验现象:
	1,通过STM32F4的USART3连接ATK-HC05蓝牙模块,检测并显示蓝牙模块的状态.
	2,通过KEY0按键可以开启/关闭定时向ATK-HC05蓝牙模块发送数据(ALIENTEK HC05 xx)测试蓝牙模块的数据发送.
	3,可以通过KEY_UP按键设置ATK-HC05蓝牙模块的主从工作模式.
	4,可以通过LCD显示ATK-HC05蓝牙模块接收到的数据.
	5,可以通过USMART对ATK-HC05蓝牙模块进行AT指令查询和设置.
	6,结合手机端蓝牙软件(蓝牙串口助手 v1.97.apk),可以实现手机无线控制开发板(点亮和关闭LED1). 

注意事项:
	1,4.3寸和7寸屏需要比较大电流,USB供电可能不足,请用外部电源适配器(推荐外接12V 1A电源).
	2,本例程在LCD_Init函数里面(在ILI93xx.c),用到了printf,如果不初始化串口1,将导致液晶无法显示!!  
	3,本例程不支持ALIENTEK CPLD方案的7寸电容触摸屏（V1版本）的测试!!
	4,请确保ATK-HC05蓝牙模块的通信波特率为9600.
	5,最好有2个ATK-HC05蓝牙串口模块,一主一从,方便测试.否则就得通过其他设备(必须有蓝牙)同ATK-HC05蓝牙模块进行对接测试.
	6,务必短接探索者STM32F4开发板P10的USART3_RX和GBC_TX以及USART3_TX和GBC_RX
	7,本例程详细说明，见光盘：9，增值资料\1，ALIENTEK 产品资料\1，ATK-HC05蓝牙串口模块\ATK-HC05蓝牙串口模块使用说明(F4版)_AN1408A.pdf


		正点原子@ALIENTEK
		2014-10-25
		广州市星翼电子科技有限公司
		电话：020-38271790
		传真：020-36773971
		购买：http://shop62103354.taobao.com
		http://shop62057469.taobao.com
		公司网站：www.alientek.com
		技术论坛：www.openedv.com