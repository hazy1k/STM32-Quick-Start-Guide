实验器材:
	探索者STM32F4开发板
	
实验目的:
	学习GPIO作为输入的使用
	
硬件资源:
	1,DS0(连接在PF9) 
	2,蜂鸣器(连接在PF8) 
	3,按键KEY0(PE4)/KEY1(PE3)/KEY2(PE2)/KEY_UP(PA0,也称之为WK_UP)
	
	
实验现象:
	本实验,通过开发板上载有的4个按钮（KEY_UP、KEY0、KEY1和KEY2），来控制板
	上的2个LED（DS0和DS1）和蜂鸣器，其中WK_UP控制蜂鸣器，按一次叫，再按一次
	停；KEY2控制DS0，按一次亮，再按一次灭；KEY1控制DS1，效果同KEY2；KEY0则
	同时控制DS0和DS1，按一次，他们的状态就翻转一次。 
	
注意事项:
	无. 
	 

					正点原子@ALIENTEK
					2014-10-24
					广州市星翼电子科技有限公司
					电话：020-38271790
					传真：020-36773971
					购买：http://shop62103354.taobao.com
					http://shop62057469.taobao.com
					公司网站：www.alientek.com
					技术论坛：www.openedv.com
