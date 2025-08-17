#include "timer.h"
#include "led.h"

TIM_HandleTypeDef TIM3_Handler; // ����һ����ʱ�����
// ǰ��֪ʶ
/*
    arr:�Զ���װ��ֵ�������������ֵʱ���������Զ���װ�أ����������¼�����
    psc:Ԥ��Ƶֵ����������ʱ��Դ��Ƶ�ʣ��Դ������Ƽ�������Ƶ�ʡ�
    ��ʱ�����ʱ����㹫ʽ��
    Time= ((arr+1)*(psc+1))/fclk us ,�����fclk�Ƕ�ʱ������Ƶ�ʣ���λΪHz��
    �����������ʹ�õ��Ƕ�ʱ��3����ʱ������APB1�ϣ�ʱ��λHCLK/2
*/
void TIM_Config(uint16_t arr, uint16_t psc)
{
    TIM3_Handler.Instance = TIM3;
    TIM3_Handler.Init.Prescaler = psc;
    TIM3_Handler.Init.CounterMode = TIM_COUNTERMODE_UP; // ���ϼ���ģʽ
    TIM3_Handler.Init.Period = arr;
    TIM3_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1; // ʱ�ӷ�Ƶϵ��Ϊ1
    HAL_TIM_Base_Init(&TIM3_Handler);
    HAL_TIM_Base_Start_IT(&TIM3_Handler); // ������ʱ���ж�
}

// �˺����ᱻHAL_TIM_Base_Init()���ã�������ʼ����ʱ��
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM3)
    {
        __HAL_RCC_TIM3_CLK_ENABLE();
        HAL_NVIC_SetPriority(TIM_IRQn, 1, 3);
        HAL_NVIC_EnableIRQ(TIM_IRQn); // ������ʱ���ж�
    }
}

// �жϷ�����
void TIM3_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TIM3_Handler); // ����HAL����жϴ�����
}

// �ص��������жϷ����������
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM3)
    {
        LED1 = !LED1;
    }
}
