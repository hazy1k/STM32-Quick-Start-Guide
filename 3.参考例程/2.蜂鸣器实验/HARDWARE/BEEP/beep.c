#include "beep.h"

void BEEP_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    __HAL_RCC_GPIOF_CLK_ENABLE(); // ʹ��GPIOFʱ��
    GPIO_InitStructure.Pin = BEEP_PIN; // PF8
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP; // �������
    GPIO_InitStructure.Pull = GPIO_PULLUP; // ����
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH; // ����
    HAL_GPIO_Init(GPIOF, &GPIO_InitStructure); // ��ʼ��GPIO
    BEEP_OFF(); // Ĭ��Ϩ�������
}
