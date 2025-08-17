#include "timer.h"
#include "led.h"

TIM_HandleTypeDef TIM14_Handle; // ��ʱ��14���
TIM_OC_InitTypeDef TIM14_CH1Handle; // ��ʱ��14ͨ��1���
TIM_HandleTypeDef TIM5_Handle; // ��ʱ��5���

// TIM14 PWM��ʼ��
void TIM14_PWM_Init(uint16_t arr, uint16_t psc)
{
    TIM14_Handle.Instance = TIM14;
    TIM14_Handle.Init.Prescaler = psc;
    TIM14_Handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    TIM14_Handle.Init.Period = arr;
    TIM14_Handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_PWM_Init(&TIM14_Handle);
    TIM14_CH1Handle.OCMode = TIM_OCMODE_PWM1;
    TIM14_CH1Handle.Pulse = arr/2;
    TIM14_CH1Handle.OCPolarity = TIM_OCNPOLARITY_LOW;
    HAL_TIM_PWM_ConfigChannel(&TIM14_Handle, &TIM14_CH1Handle, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&TIM14_Handle, TIM_CHANNEL_1);
}

// ��ʱ��14�ײ�����
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    __HAL_RCC_TIM14_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_TIM14; // PF9����ΪTIM14_CH1
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
}

// ����TIM14ͨ����ռ�ձ�
void TIM_SetTIM14Compare1(uint16_t compare)
{
    TIM14->CCR1 = compare;
}

// ��ʱ��5�����벶���ʼ��
void TIM5_CH1_Cap_Init(uint32_t arr, uint16_t psc)
{
    TIM_IC_InitTypeDef TIM_ICInitStructure;
    TIM5_Handle.Instance = TIM5;
    TIM5_Handle.Init.Prescaler = psc;
    TIM5_Handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    TIM5_Handle.Init.Period = arr;
    TIM5_Handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_IC_Init(&TIM5_Handle);
    // �����ز���
    TIM_ICInitStructure.ICPolarity = TIM_ICPOLARITY_RISING;
    TIM_ICInitStructure.ICSelection = TIM_ICSELECTION_DIRECTTI; // ӳ�䵽���벶��ͨ��
    TIM_ICInitStructure.ICPrescaler = TIM_ICPSC_DIV1; // ����Ƶ
    TIM_ICInitStructure.ICFilter = 0; // ���˲�
    HAL_TIM_IC_ConfigChannel(&TIM5_Handle, &TIM_ICInitStructure, TIM_CHANNEL_1);
    HAL_TIM_IC_Start_IT(&TIM5_Handle, TIM_CHANNEL_1); // �������벶���ж�
    __HAL_TIM_ENABLE_IT(&TIM5_Handle, TIM_IT_UPDATE); // ���������ж�
}

// ��ʱ��5�ײ�����
void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    __HAL_RCC_TIM5_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM5; // PA0����ΪTIM5_CH1
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    HAL_NVIC_SetPriority(TIM5_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(TIM5_IRQn);
}

// ��ʱ��5�жϷ�����
void TIM5_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TIM5_Handle);
}

// ����TIM����/�ȽϼĴ���ֵ
uint32_t TIM_GetTIM14Capture1(void)
{
    return HAL_TIM_ReadCapturedValue(&TIM14_Handle, TIM_CHANNEL_1); // ��ȡ����ֵ
}

// ����״̬
// [7]:0,û�гɹ�����1,�ɹ�����
// [6]:0,û�в��񵽵͵�ƽ��1,���񵽵͵�ƽ
// [5:0]:����͵�ƽ������Ĵ���������32λ��ʱ����1us��������1�����������Ϊ2^32-1 = 4294�룩
uint8_t TIM5CH1_CapTure_STA = 0; // ���벶���־
uint32_t TIM5CH1_CapTure_VAL; // ���벶��ֵ

// ��ʱ�������жϣ�����������жϴ���ص�����
// �˺��ᱻHAL_TIM_IRQHandler����
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) // ��ʱ������ж�
{
    if((TIM5CH1_CapTure_STA & 0x80) == 0) // ��δ�ɹ�����
    {
        if(TIM5CH1_CapTure_STA & 0x40) // �Ѿ����񵽸ߵ�ƽ
        {
            if((TIM5CH1_CapTure_STA & 0x3F) == 0x3F) // �ߵ�ƽ̫����
            {
                TIM5CH1_CapTure_STA |= 0x80; // �ɹ������־
                TIM5CH1_CapTure_VAL = 0xFFFFFFFF; // ����ֵ��Ϊ��� 
            }
        }
        else // ����һ�ε͵�ƽ
        {
            TIM5CH1_CapTure_STA++;
        }

    }
}

// ��ʱ�����벶���жϴ���ص�����
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) // ���벶���ж�
{
    if((TIM5CH1_CapTure_STA & 0x80) == 0) // ��δ�ɹ�����
    {
        if(TIM5CH1_CapTure_STA & 0x40) // ��׽��һ���½���
        {
            TIM5CH1_CapTure_STA |= 0x80; // �ɹ������־
            TIM5CH1_CapTure_VAL = HAL_TIM_ReadCapturedValue(&TIM5_Handle, TIM_CHANNEL_1); // ��ȡ��ǰ����ֵ
            TIM_RESET_CAPTUREPOLARITY(&TIM5_Handle, TIM_CHANNEL_1); // ��λ������
            TIM_SET_CAPTUREPOLARITY(&TIM5_Handle, TIM_CHANNEL_1, TIM_ICPOLARITY_RISING); // �л�����׽������
        }
        else // ��׽��һ��������
        {
            TIM5CH1_CapTure_STA = 0; // ��λ״̬
            TIM5CH1_CapTure_VAL = 0; // ����ֵ����
            TIM5CH1_CapTure_STA |= 0x40; // ���������ر�־
            __HAL_TIM_DISABLE(&TIM5_Handle); // �رն�ʱ��
            __HAL_TIM_SET_COUNTER(&TIM5_Handle, 0); // ��λ������
            TIM_RESET_CAPTUREPOLARITY(&TIM5_Handle, TIM_CHANNEL_1); // ��λ������
            TIM_SET_CAPTUREPOLARITY(&TIM5_Handle, TIM_CHANNEL_1, TIM_ICPOLARITY_FALLING); // �л�����׽�½���
            __HAL_TIM_ENABLE(&TIM5_Handle); // ������ʱ��
        }
    }
}
