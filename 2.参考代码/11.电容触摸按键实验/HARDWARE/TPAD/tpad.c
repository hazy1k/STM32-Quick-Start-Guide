#include "tpad.h"
#include "delay.h"
#include "usart.h"

#define TPAD_ARR_MAX_VAL 0xFFFFFFFF // TIM2(32λ)���ARRֵ
TIM_HandleTypeDef TIM2_Handler; // ��ʱ��2������
vu16 tpad_default_val = 0; // ����ʱ����������Ҫ��ʱ��

// ���ݰ�����ʼ��
uint8_t tpad_init(u16 psc)
{
    u16 buf[10];
    u16 temp;
    uint8_t i, j;
    TIM2_CH1_Cap_Init(TPAD_ARR_MAX_VAL, psc-1); // ��ʼ����ʱ��2ͨ��1�������Զ���װ��ֵ��Ԥ��Ƶֵ
	for(i = 0; i < 10; i++)
	{
		buf[i] = TPAD_Get_Val(); // ��ȡ���ݴ�������ֵ
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
	tpad_default_val = temp/6; // ����Ĭ��ֵ
	printf("tpad_default_val:%d\r\n",tpad_default_val);
	if(tpad_default_val>(vu16)TPAD_ARR_MAX_VAL/2)
	{
		return 1; // ���ݰ�����ʼ��ʧ��
	}
	return 0; // ���ݰ�����ʼ���ɹ�
}

// ��ʱ��2ͨ��1���벶������
void TIM2_CH1_Cap_Init(u32 arr, u16 psc)
{
	/* TIM2�������� */
	TIM_IC_InitTypeDef TIM2_CH1Config;
	TIM2_Handler.Instance = TIM2;
	TIM2_Handler.Init.Prescaler = psc; 
	TIM2_Handler.Init.CounterMode = TIM_COUNTERMODE_UP;
	TIM2_Handler.Init.Period = arr;
	TIM2_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	HAL_TIM_IC_Init(&TIM2_Handler);
	/* ���벶������ */
	TIM2_CH1Config.ICPolarity = TIM_ICPOLARITY_RISING; // �����ز���
	TIM2_CH1Config.ICSelection = TIM_ICSELECTION_DIRECTTI;
	TIM2_CH1Config.ICPrescaler = TIM_ICPSC_DIV1;
	TIM2_CH1Config.ICFilter = 0;
	HAL_TIM_IC_ConfigChannel(&TIM2_Handler, &TIM2_CH1Config, TIM_CHANNEL_1); // ����TIM2_CH1
	HAL_TIM_IC_Start(&TIM2_Handler, TIM_CHANNEL_1);
}

// TIM2�ײ�����
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

// ��λ���ݰ������ͷŵ��ݵ����������ʱ������ֵ
void TPAD_Reset(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	GPIO_Initure.Pin = GPIO_PIN_5;
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_Initure.Pull = GPIO_PULLDOWN;
	GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_Initure);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET); // �ͷŵ���
	delay_ms(5);
	__HAL_TIM_CLEAR_FLAG(&TIM2_Handler, TIM_FLAG_CC1|TIM_FLAG_UPDATE); // ����жϱ�־λ
	__HAL_TIM_SET_COUNTER(&TIM2_Handler, 0); // ���������
	GPIO_Initure.Mode = GPIO_MODE_AF_PP;
	GPIO_Initure.Pull = GPIO_NOPULL;
	GPIO_Initure.Alternate = GPIO_AF1_TIM2; // PA5����ΪTIM2_CH1
	HAL_GPIO_Init(GPIOA, &GPIO_Initure);
}

// ȡ�ò���ֵ
u16 TPAD_Get_Val(void)
{
	TPAD_Reset();
	while(__HAL_TIM_GET_FLAG(&TIM2_Handler, TIM_FLAG_CC1) == RESET) // ����������
	{
		if(__HAL_TIM_GET_COUNTER(&TIM2_Handler) >= TPAD_ARR_MAX_VAL-500)
		{
			return __HAL_TIM_GET_COUNTER(&TIM2_Handler); // �Ѿ���ʱ��ֱ�ӷ���CNTֵ
		}
	};
	return HAL_TIM_ReadCapturedValue(&TIM2_Handler, TIM_CHANNEL_1); // ��ȡ������ֵ
}

// ��ȡn�Σ�ȡ���ֵ
u16 TPAD_Get_MaxVal(u8 n)
{ 
	u16 temp = 0; 
	u16 res = 0; 
	u8 lcntnum = n*2/3; // ����2/3*n����Ч������,������Ч
	u8 okcnt = 0;
	while(n--)
	{
		temp = TPAD_Get_Val(); // �õ�һ��ֵ
		if(temp > (tpad_default_val*5/4))
			okcnt++; // ���ٴ���Ĭ��ֵ��5/4������Ч
		if(temp > res)
			res = temp;
	}
	if(okcnt >= lcntnum)
		return res; // ����2/3�ĸ���,Ҫ����Ĭ��ֵ��5/4������Ч
	else 
		return 0;
}  

// ��������ɨ��
u8 TPAD_Scan(u8 mode)
{
	static u8 keyen = 0; // 0,���Կ�ʼ���;>0,�����ܿ�ʼ���	 
	u8 res = 0;
	u8 sample = 3; // Ĭ�ϲ�������Ϊ3��	 
	u16 rval;
	if(mode)
	{
		sample = 6;	// ֧��������ʱ�����ò�������Ϊ6��
		keyen = 0;	// ֧������	  
	}
	rval = TPAD_Get_MaxVal(sample); // ����n�� 
	if(rval>(tpad_default_val*4/3)&&rval<(10*tpad_default_val)) // ����tpad_default_val+(1/3)*tpad_default_val,��С��10��tpad_default_val,����Ч
	{							 
		if(keyen==0)
			res = 1; // keyen==0,��Ч 
		//printf("r:%d\r\n",rval);		     	    					   
		keyen = 3; // ����Ҫ�ٹ�3��֮����ܰ�����Ч   
	} 
	if(keyen)
		keyen--; // ��������1		   							   		     	    					   
	return res;
}



