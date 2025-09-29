#include "atk_ncr.h"
#include "malloc.h"					   										   
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//
//本数字字母识别程序由ALIENTEK提供,我们提供2个LIB,供大家使用
//ATKNCR_M_Vx.x.lib和ATKNCR_N_Vx.x.lib的唯一区别是是否使用动态内存分配.
//其中:M,代表需要用到malloc的版本,必须实现alientek_ncr_malloc和alientek_ncr_free两个函数
//     N,代表普通版本,不需要实现alientek_ncr_malloc和alientek_ncr_free两个函数
//     Vx.x,代表当前识别程序的版本.		 	  
//功能:支持数字/小写字母/大写字母/混合四种识别模式.		  
//本识别程序使用起来相当简单.					   
//第一步:调用alientek_ncr_init函数,初始化识别程序
//第二步:获取输入的点阵数据(必须有2个及以上的不同点阵数据输入)
//第三步:调用alientek_ncr函数,得到识别结果.
//第四步:如果不需要再识别,则调用alientek_ncr_stop函数,终止识别.如果还需要继续,则重复2,3步即可.		   
//版本:V2.0
//正点原子@ALIENTEK
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved													    								  
//版权所有，盗版必究。
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//内存设置函数
void alientek_ncr_memset(char *p,char c,unsigned long len) 
{
	mymemset((u8*)p,(u8)c,(u32)len);
}	 					  
//内存申请函数
void *alientek_ncr_malloc(unsigned int size) 
{
	return mymalloc(SRAMIN,size);
}
//内存清空函数
void alientek_ncr_free(void *ptr) 
{
	myfree(SRAMIN,ptr);
}		  
		  



















