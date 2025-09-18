【*】程序简介
-工程名称：基本定时器-定时
-实验平台: 野火STM32-F103 指南者 开发板 
-ST固件库版本：3.5.0
【 ！】功能简介：
使用STM32的基本定时器定时，LED每1000ms闪烁一次(亮1000ms，灭1000ms)。
学习目的：学会使用基本定时器，注意区分三种不同种类的定时器。
【*】实验操作：
	 把编写好的程序下载到开发板，可以看到LED1以1S的频率闪烁一次
【*】 引脚分配
LED灯：
LED灯的三个阴极接到STM32三个引脚上，LED低电平亮。
	(RED)R26 引脚为PB5
	(GREEN)R24 引脚为PB0
	(BLUE)R44 引脚为PB1
【&】程序描述：  
    <TiMbase.c>:
	1.基本定时器嵌套向量中断控制器配置； 
	2.基本定时器模式配置（基本定时器不用管时钟分频因子、计数模式、重复计数器的值的设置）
	3.基本定时器初始化
	在例程的时钟设置与分频情况如下：
	APB1 定时器:（例程的TIM6和TIM7在APB1上）
	定时器时钟源TIMxCLK = 2 * PCLK1 =72M
	PCLK1 = HCLK / APB1 Prescaler =  SystemCoreClock/2 = 36MHz 
	时钟预分频数为 71,TIM_Prescaler= 71
	设定定时器频率为=TIMxCLK/(TIM_Prescaler+1)=1MHz
	APB2 定时器：
	定时器时钟源TIMxCLK = 1 * PCLK2 =72M
	PCLK2 = HCLK / APB2 Prescaler =  SystemCoreClock/1 = 72MHz 