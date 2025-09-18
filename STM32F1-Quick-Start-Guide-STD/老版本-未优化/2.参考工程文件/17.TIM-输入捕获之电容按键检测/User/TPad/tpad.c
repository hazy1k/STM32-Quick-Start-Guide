#include "tpad.h"

uint16_t tpad_default_val;

// 电容按键GPIO初始化
void TPAD_TIM_GPIO_Config(void) 
{
  	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(TPAD_TIM_CH_GPIO_CLK, ENABLE); 
    GPIO_InitStructure.GPIO_Pin =  TPAD_TIM_CH_PIN;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
  	GPIO_Init(TPAD_TIM_CH_PORT, &GPIO_InitStructure);
}
// 前置知识
/*
	1.ARR：自动重载寄存器的值，本工程设置为0xFFFF，即计数器的最大值
	2.CNTxCLK：计数器的时钟频率，APB1的时钟频率为72M，所以CNTxCLK=72/(psc+1) = 1MHz
	3.PWN信号的周期T：T = ARR*(psc+1)/72 = ARR*1
	4.占空比P：P = CCR/(ARR+1)，其中CCR为比较寄存器的值，代表高电平在周期的时间
*/
// 定时器模式配置
static void TPAD_TIM_Mode_Config(void)
{
  	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;
	TPAD_TIM_APBxClock_FUN(TPAD_TIM_CLK,ENABLE);
/*--------------------时基结构体初始化-------------------------*/
	TIM_TimeBaseStructure.TIM_Period = TPAD_TIM_Period;	
	TIM_TimeBaseStructure.TIM_Prescaler = TPAD_TIM_Prescaler;	
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;		
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;		
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;	
	TIM_TimeBaseInit(TPAD_TIM, &TIM_TimeBaseStructure);
/*--------------------输入捕获结构体初始化----------------------*/		
	TIM_ICInitStructure.TIM_Channel = TPAD_TIM_CHANNEL_x;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICFilter = 0;
	TIM_ICInit(TPAD_TIM, &TIM_ICInitStructure);
	TIM_Cmd(TPAD_TIM, ENABLE);
}

void TPAD_Reset(void) // 电容按键复位
{	
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(TPAD_TIM_CH_GPIO_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin =  TPAD_TIM_CH_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(TPAD_TIM_CH_PORT, &GPIO_InitStructure);
	GPIO_ResetBits(TPAD_TIM_CH_PORT, TPAD_TIM_CH_PIN);
	Delay_ms(5);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_Init(TPAD_TIM_CH_PORT, &GPIO_InitStructure);
}

uint16_t TPAD_Get_Val(void) // 获取定时器捕获值
{		
	TPAD_Reset();
	// 当电容按键复位放电之后，计数器清0开始计数
  	TIM_SetCounter (TPAD_TIM, 0);
	TIM_ClearITPendingBit(TPAD_TIM, TPAD_TIM_IT_CCx | TIM_IT_Update);
	// 等待捕获上升沿，当电容按键充电到1.8V左右的时候，就会被认为是上升沿
	while(TIM_GetFlagStatus (TPAD_TIM, TPAD_TIM_IT_CCx) == RESET)
	{
		if (TIM_GetCounter(TPAD_TIM) > TPAD_TIM_Period-100) // 超时处理
		{
			return TIM_GetCounter (TPAD_TIM);
		} 			
	}
	return TPAD_TIM_GetCapturex_FUN(TPAD_TIM); // 获取捕获值
} 

void TPAD_TIM_Init(void)
{
	TPAD_TIM_GPIO_Config();
	TPAD_TIM_Mode_Config();		
}

// 初始化触摸按键，获得空载的时候触摸按键的充电时间
uint8_t TPAD_Init(void)
{
	uint16_t temp;
	TPAD_TIM_Init();
	temp = TPAD_Get_Val();
	// 调试的时候可以把捕获的值打印出来，看看默认的充电时间是多少
	printf("电容按键默认充电时间为: %d us\n",temp);
	if((TPAD_DEFAULT_VAL_MIN<temp) && (temp<TPAD_DEFAULT_VAL_MAX)) // 确保默认的充电时间在合理范围内
	{
		tpad_default_val = temp;
		return 0;  // 成功
	}
	else
    {
		return 1; // 失败
    }
}

// 获得多个触摸按键的最大值
uint16_t TPAD_Get_MaxVal( uint8_t num )
{
	uint16_t temp=0, res=0;
	while(num--)
	{
		temp = TPAD_Get_Val();
		if( temp > res ) 
			res = temp;
	}	
	return res;	
} 

// 按键扫描函数
uint8_t TPAD_Scan(void)
{
	static uint8_t keyen=0;	
	uint8_t res=0,sample=3; 
	uint16_t scan_val;		
	// 根据sample值采样多次，并取最大值，小的一般是干扰或者是误触摸
	scan_val = TPAD_Get_MaxVal(sample); 
    // 当扫描的值大于空载值加上默认的门限值之后，表示按键按下
	if(scan_val > (tpad_default_val+TPAD_GATE_VAL))
	{						
    	// 再次检测，类似于机械按键的去抖
		scan_val = TPAD_Get_MaxVal(sample);		
		if((keyen == 0 )&&(scan_val > (tpad_default_val+TPAD_GATE_VAL)))
				res = 1;	// 有效的按键
		keyen = 2;
	}
	if(keyen > 0)
		keyen--;
	return res;
}
