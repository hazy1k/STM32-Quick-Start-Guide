实验器材:
	探索者STM32F4开发板
	
实验目的:
	学习UCOSII的任务调度,初步认识UCOSII
	
硬件资源:
	1,DS0(连接在PF9)
	2,DS1(连接在PF10) 
	
实验现象:
	本章我们在UCOSII里面创建3个任务：开始任务、LED0任务和LED1任务，开始任务用于创建其他（LED0和LED1）任
	务，之后挂起；LED0任务用于控制DS0的亮灭，DS0每秒钟亮80ms；LED1任务用于控制DS1的亮灭，DS1亮300ms，灭
	300ms，依次循环。

注意事项: 
	1,本实验改自跑马灯实验。
	2,本实验使用ucosii版本为V2.91。
		
		  
					正点原子@ALIENTEK
					2014-10-24
					广州市星翼电子科技有限公司
					电话：020-38271790
					传真：020-36773971
					购买：http://shop62103354.taobao.com
					http://shop62057469.taobao.com
					公司网站：www.alientek.com
					技术论坛：www.openedv.com



















