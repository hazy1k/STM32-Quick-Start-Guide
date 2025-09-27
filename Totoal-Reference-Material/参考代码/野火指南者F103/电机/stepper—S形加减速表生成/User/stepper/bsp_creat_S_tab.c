/**
  ******************************************************************************
  * @file    bsp_creat_S_tab.c
  * @author  fire
  * @version V1.0
  * @date    2020-xx-xx
  * @brief   S加减速生成表
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F103 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
#include "./stepper/bsp_creat_S_tab.h"

/*

在这里采用的是数学匀变速的数学模型，但是速度也并

不是匀速变化的，所以准确的说是加速度的匀变数学模型。

a-t 曲线如下 （V-t曲线 请看 文档）

	| 
 a|   /|\             
	|  / | \
	| /  |  \            
	|/___|___\_t 		
	0   t/2
		
以上就是加速度的匀变模型（默认初始的加速度为0）

那么有了这个模型就很容易可以算出一些不为人知的秘密！！！

首先有了以上的模型就可以算出每一时刻的加速度，所以先看 

0~t/2 这段 a=K*T ，其中K为加速度a的斜率

将加速度积分就是速度，所以 V = a dt = K*T dt

得： V = K*T^2/2

将速度积分就是位移，所以 S = V dt = K*T^2/2 dt 

得： S = K*T^3/6

*/

Speed_s Speed ;
uint8_t print_flag=0;


/**
  * @brief  计算加速表
  * @param  Vo  初始速度
  * @param  Vt	t时刻的速度
  * @param  T	加速完成时间
  *	@note 		无
  * @retval 无
  */

void CalculateSpeedTab(int Vo, int Vt, float T)
{
	int32_t i = 0;

	int32_t Vm =0;      // 中间点速度
	float K = 0;   		// 加加速度 加速度的斜率
	float Tn = 0;     	// Tn时刻
	float DeltaV = 0; 	// 每一时刻的速度
	float TimeDel = 0;	// 时间间隔

	Speed.Vo = CONVER(Vo);    // 起速:Step/s
	Speed.Vt = CONVER(Vt);    // 末速:Step/s
	
	T = T / 2;						//加加速段的时间（加速度斜率>0的时间）
	
	Vm = (Speed.Vo + Speed.Vt) / 2;	//计算中点的速度
	
	K = ( ( 2 * ((Vm) - (Speed.Vo)) ) / pow((T),2) );// 根据中点速度计算加加速度
	
	Speed.AccelHalfStep  = (int)( ( (K) * pow( (T) ,3) ) / 6 );// 加加速需要的步数
	
	/* 申请内存空间存放速度表 */
	Speed.AccelHalfStep  = abs(Speed.AccelHalfStep ); // 减速计算的时候防止出现负数
	if( Speed.AccelHalfStep  % 2 != 0)     // 由于浮点型数据转换成整形数据带来了误差,所以这里加1
	{
		Speed.AccelHalfStep  += 1;
	}
	Speed.AccelTotalStep = Speed.AccelHalfStep * 2;           // 加速段的步数
	
	if(FORM_LEN<Speed.AccelTotalStep)
	{
		printf("FORM_LEN 缓存长度不足\r\n,请将 FORM_LEN 修改为 %d \r\n",Speed.AccelTotalStep);
		return ;
	}
	  
	/* 目标的速度曲线是对时间的方程,将时间与步数对应，所以在此将时间分成
		与步数对应的份数,并且计算出 ,这样就可以计算出相应的速度*/
	TimeDel = T / Speed.AccelHalfStep;

	for(i = 0; i <= Speed.AccelHalfStep; i++)
	{
		Tn = i * TimeDel;						// 第i个时刻的Tn
		DeltaV = 0.5f * K * pow(Tn,2);        	// 在Tn时刻所对应的速度  dv = 1/2 * K * t^2;
		Speed.Form[i] = Speed.Vo + DeltaV;		// 得到每一时刻对应的速度,由于考虑到初速度不为0的情况，所以与Vo相加求和
																				
		Speed.Form [ Speed.AccelTotalStep - i] = Speed.Vt - DeltaV ;        // 加加速过程与减加速是中心对称,可以直接求出后半段速度
																			// 减加速过程对称点的速度
	}
	/* 输出速度表内容 */
	for(i = 0; i <= Speed.AccelTotalStep ; i++)
	{
		printf("i,%.3f;speed,%.3f\n",(float)i,Speed.Form[i]);	
	}
	/* 清空表 */
	memset(Speed.Form,0,FORM_LEN*sizeof(float));
}






