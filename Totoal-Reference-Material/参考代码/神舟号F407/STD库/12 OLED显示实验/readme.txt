实验器材:
	探索者STM32F4开发板
	
实验目的:
	学习单色OLED显示屏的使用
	
硬件资源:
	1,DS0(连接在PF9) 
	2,ALIENTEK 0.96寸OLED模块(连接IO说明,见oled.h文件) 
	
实验现象:
	本实验利用开发板的OLED/CAMERA接口连接ALIENTEK OLED模块，实现OLED模块的显示，下
	载成功后，通过把OLED模块插入开发板左下角的OLED/CAMERA模块接口，按下复位之后，就
	可以看到OLED模块不停的显示ASCII码和码值。 
	
注意事项: 
	1,开发板的OLED/CAMERA模块接口比OLED的插针多2个,OLED靠左插即可.
	2,例程默认用的8080方式驱动OLED模块,所以OLED模块的BS0,BS1都需要接VCC(默认接的VCC). 
	
					正点原子@ALIENTEK
					2014-10-25
					广州市星翼电子科技有限公司
					电话：020-38271790
					传真：020-36773971
					购买：http://shop62103354.taobao.com
					http://shop62057469.taobao.com
					公司网站：www.alientek.com
					技术论坛：www.openedv.com