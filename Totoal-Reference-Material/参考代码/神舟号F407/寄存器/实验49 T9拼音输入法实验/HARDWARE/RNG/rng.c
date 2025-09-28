#include "rng.h"
#include "delay.h"
 //////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//RNG(随机数发生器)驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/5
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 

//初始化RNG
//返回值:0,成功;1,失败
u8 RNG_Init(void)
{
	u16 retry=0; 
	RCC->AHB2ENR=1<<6;	//开启RNG时钟,来自PLL48CLK
	RNG->CR|=1<<2;		//使能RNG
	while((RNG->SR&0X01)==0&&retry<10000)	//等待随机数就绪
	{
		retry++;
		delay_us(100);
	}
	if(retry>=10000)return 1;//随机数产生器工作不正常
	return 0;
}
//得到随机数
//返回值:获取到的随机数
u32 RNG_Get_RandomNum(void)
{	 
	while((RNG->SR&0X01)==0);	//等待随机数就绪  
	return RNG->DR;	
}
//得到某个范围内的随机数
//min,max,最小,最大值.
//返回值:得到的随机数(rval),满足:min<=rval<=max
int RNG_Get_RandomRange(int min,int max)
{ 
   return RNG_Get_RandomNum()%(max-min+1)+min;
}
















