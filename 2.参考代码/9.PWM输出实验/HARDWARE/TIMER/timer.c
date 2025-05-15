#include "timer.h"
#include "led.h"

// ǰ��֪ʶ
/*
    arr:�Զ���װ��ֵ�������������ֵʱ���������Զ���װ�أ����������¼�����
    psc:Ԥ��Ƶֵ����������ʱ��Դ��Ƶ�ʣ��Դ������Ƽ�������Ƶ�ʡ�
    ��ʱ�����ʱ����㹫ʽ��
    Time= ((arr+1)*(psc+1))/fclk us ,�����fclk�Ƕ�ʱ������Ƶ�ʣ���λΪHz��
    �����������ʹ�õ��Ƕ�ʱ��3����ʱ������APB1�ϣ�ʱ��λHCLK/2
*/
TIM_HandleTypeDef TIM14_Handler; // ��ʱ��TIM14���
TIM_OC_InitTypeDef TIM14_CH1Handler; // ��ʱ��TIM14ͨ��1���

// TIM14 PWM��ʼ�� 
void TIM14_PWM_Init(u16 arr, u16 psc)
{  
    TIM14_Handler.Instance = TIM14;          	   
    TIM14_Handler.Init.Prescaler = psc;            
    TIM14_Handler.Init.CounterMode = TIM_COUNTERMODE_UP;
    TIM14_Handler.Init.Period = arr;               
    TIM14_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_PWM_Init(&TIM14_Handler); 
    TIM14_CH1Handler.OCMode=TIM_OCMODE_PWM1; // ģʽѡ��PWM1
    TIM14_CH1Handler.Pulse = arr/2;          // ���ñȽ�ֵ,��ֵ����ȷ��ռ�ձȣ�Ĭ�ϱȽ�ֵΪ�Զ���װ��ֵ��һ��,��ռ�ձ�Ϊ50%
    TIM14_CH1Handler.OCPolarity = TIM_OCPOLARITY_LOW; // ����Ƚϼ���Ϊ�� 
    HAL_TIM_PWM_ConfigChannel(&TIM14_Handler,&TIM14_CH1Handler,TIM_CHANNEL_1); // ����TIM14ͨ��1
    HAL_TIM_PWM_Start(&TIM14_Handler,TIM_CHANNEL_1); // ����PWMͨ��1
}

// ��ʱ���ײ�������ʱ��ʹ�ܣ���������
// �˺����ᱻHAL_TIM_PWM_Init()����
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
	GPIO_InitTypeDef GPIO_Initure;
	__HAL_RCC_TIM14_CLK_ENABLE();			
	__HAL_RCC_GPIOF_CLK_ENABLE();			
	GPIO_Initure.Pin = GPIO_PIN_9;          
	GPIO_Initure.Mode = GPIO_MODE_AF_PP;  	
	GPIO_Initure.Pull = GPIO_PULLUP;        
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;   
	GPIO_Initure.Alternate = GPIO_AF9_TIM14; // PF9����ΪTIM14_CH1
	HAL_GPIO_Init(GPIOF,&GPIO_Initure);
}

// ����TIMͨ��4��ռ�ձ�
void TIM_SetTIM14Compare1(u32 compare)
{
	TIM14->CCR1 = compare; // ����ռ�ձ� 
}
