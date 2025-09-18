#include "tpad.h"
#include "delay.h"
#include "usart.h"

#define TPAD_ARR_MAX_VAL 0xFFFFFFFF // TIM2(32位)最大ARR值
TIM_HandleTypeDef TIM2_Handler; // 定时器2任务句柄
vu16 tpad_default_val = 0; // 空载时，计数器需要的时间

// 电容按键初始化
uint8_t tpad_init(u16 psc)
{
    u16 buf[10];
    u16 temp;
    uint8_t i, j;
    TIM2_CH1_Cap_Init(TPAD_ARR_MAX_VAL, psc-1); // 初始化定时器2通道1，设置自动重装载值和预分频值
	for(i = 0; i < 10; i++)
	{
		buf[i] = TPAD_Get_Val(); // 读取电容触摸按键值
	}
	for(i = 0; i < 9; i++)
	{
		for(j = i+1; j < 10; j++)
		{
			if(buf[i]>buf[j])
			{
				temp = buf[i];
				buf[i] = buf[j];
				buf[j] = temp;
			}
		}
	}
	temp = 0;
	for(i = 2; i < 8; i++)
	{
		temp += buf[i];
	}
	tpad_default_val = temp/6; // 计算默认值
	printf("tpad_default_val:%d\r\n",tpad_default_val);
	if(tpad_default_val>(vu16)TPAD_ARR_MAX_VAL/2)
	{
		return 1; // 电容按键初始化失败
	}
	return 0; // 电容按键初始化成功
}

// 定时器2通道1输入捕获配置
void TIM2_CH1_Cap_Init(u32 arr, u16 psc)
{
	/* TIM2基础配置 */
	TIM_IC_InitTypeDef TIM2_CH1Config;
	TIM2_Handler.Instance = TIM2;
	TIM2_Handler.Init.Prescaler = psc; 
	TIM2_Handler.Init.CounterMode = TIM_COUNTERMODE_UP;
	TIM2_Handler.Init.Period = arr;
	TIM2_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	HAL_TIM_IC_Init(&TIM2_Handler);
	/* 输入捕获配置 */
	TIM2_CH1Config.ICPolarity = TIM_ICPOLARITY_RISING; // 上升沿捕获
	TIM2_CH1Config.ICSelection = TIM_ICSELECTION_DIRECTTI;
	TIM2_CH1Config.ICPrescaler = TIM_ICPSC_DIV1;
	TIM2_CH1Config.ICFilter = 0;
	HAL_TIM_IC_ConfigChannel(&TIM2_Handler, &TIM2_CH1Config, TIM_CHANNEL_1); // 配置TIM2_CH1
	HAL_TIM_IC_Start(&TIM2_Handler, TIM_CHANNEL_1);
}

// TIM2底层驱动
void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
{
	GPIO_InitTypeDef GPIO_Initure;
	__HAL_RCC_TIM2_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_Initure.Pin = GPIO_PIN_5;
	GPIO_Initure.Mode = GPIO_MODE_AF_PP;
	GPIO_Initure.Pull = GPIO_NOPULL;
	GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_Initure.Alternate = GPIO_AF1_TIM2;
	HAL_GPIO_Init(GPIOA, &GPIO_Initure);
}

// 复位电容按键：释放电容点亮，清楚定时器计数值
void TPAD_Reset(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	GPIO_Initure.Pin = GPIO_PIN_5;
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_Initure.Pull = GPIO_PULLDOWN;
	GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_Initure);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET); // 释放电容
	delay_ms(5);
	__HAL_TIM_CLEAR_FLAG(&TIM2_Handler, TIM_FLAG_CC1|TIM_FLAG_UPDATE); // 清除中断标志位
	__HAL_TIM_SET_COUNTER(&TIM2_Handler, 0); // 清除计数器
	GPIO_Initure.Mode = GPIO_MODE_AF_PP;
	GPIO_Initure.Pull = GPIO_NOPULL;
	GPIO_Initure.Alternate = GPIO_AF1_TIM2; // PA5复用为TIM2_CH1
	HAL_GPIO_Init(GPIOA, &GPIO_Initure);
}

// 取得捕获值
u16 TPAD_Get_Val(void)
{
	TPAD_Reset();
	while(__HAL_TIM_GET_FLAG(&TIM2_Handler, TIM_FLAG_CC1) == RESET) // 捕获上升沿
	{
		if(__HAL_TIM_GET_COUNTER(&TIM2_Handler) >= TPAD_ARR_MAX_VAL-500)
		{
			return __HAL_TIM_GET_COUNTER(&TIM2_Handler); // 已经超时，直接返回CNT值
		}
	};
	return HAL_TIM_ReadCapturedValue(&TIM2_Handler, TIM_CHANNEL_1); // 读取计数器值
}

// 读取n次，取最大值
u16 TPAD_Get_MaxVal(u8 n)
{ 
	u16 temp = 0; 
	u16 res = 0; 
	u8 lcntnum = n*2/3; // 至少2/3*n的有效个触摸,才算有效
	u8 okcnt = 0;
	while(n--)
	{
		temp = TPAD_Get_Val(); // 得到一次值
		if(temp > (tpad_default_val*5/4))
			okcnt++; // 至少大于默认值的5/4才算有效
		if(temp > res)
			res = temp;
	}
	if(okcnt >= lcntnum)
		return res; // 至少2/3的概率,要大于默认值的5/4才算有效
	else 
		return 0;
}  

// 触摸按键扫描
u8 TPAD_Scan(u8 mode)
{
	static u8 keyen = 0; // 0,可以开始检测;>0,还不能开始检测	 
	u8 res = 0;
	u8 sample = 3; // 默认采样次数为3次	 
	u16 rval;
	if(mode)
	{
		sample = 6;	// 支持连按的时候，设置采样次数为6次
		keyen = 0;	// 支持连按	  
	}
	rval = TPAD_Get_MaxVal(sample); // 采样n次 
	if(rval>(tpad_default_val*4/3)&&rval<(10*tpad_default_val)) // 大于tpad_default_val+(1/3)*tpad_default_val,且小于10倍tpad_default_val,则有效
	{							 
		if(keyen==0)
			res = 1; // keyen==0,有效 
		//printf("r:%d\r\n",rval);		     	    					   
		keyen = 3; // 至少要再过3次之后才能按键有效   
	} 
	if(keyen)
		keyen--; // 计数器减1		   							   		     	    					   
	return res;
}



