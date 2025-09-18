#include "iwdg.h"

IWDG_HandleTypeDef IWDG_Handler;

// ǰ��֪ʶ
/*
    IWDG������Ҫ����������
    ��Ƶϵ����IWDG_Prescaler_4 ~ IWDG_Prescaler_256
    �Զ���װ��ֵ��0~0x0FFF
    ʱ����㹫ʽ��
    Time = ((��Ƶϵ��*��װ��ֵ)/LSIƵ��
    ����LSIƵ��ͨ��Ϊ32.768KHz
*/

void IWDG_Init(uint8_t prescaler, uint16_t reload)
{
    IWDG_Handler.Instance = IWDG;           // ѡ��IWDG����
    IWDG_Handler.Init.Prescaler = prescaler;// ���÷�Ƶϵ��
    IWDG_Handler.Init.Reload = reload;      // �����Զ���װ��ֵ
    HAL_IWDG_Init(&IWDG_Handler);           // ��ʼ��IWDG
}

void IWDG_Feed(void)
{
    HAL_IWDG_Refresh(&IWDG_Handler);        // ι��
}
