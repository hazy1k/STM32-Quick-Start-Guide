#include "exti.h"
#include "key.h"
#include "delay.h"
#include "led.h"

// �жϻص�����
void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(KEY_UP_PIN); // ����HAL����жϴ�����
}
void EXTI2_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(KEY2_PIN);
}
void EXTI3_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(KEY1_PIN);
}
void EXTI4_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(KEY0_PIN);
}

// �жϷ�����
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    delay_ms(100);
    switch(GPIO_Pin)
    {
        case KEY_UP_PIN:
            if(KEY_UP_GET == 1) // KEY_UP����
            {
                LED1 = !LED1;
                LED0 = !LED1;
            }
            break;
        case KEY0_PIN:
            if(KEY0_GET == 0)
            {
                LED0 = !LED0;
            }
            break;
        case KEY2_PIN:
            if(KEY2_GET == 0)
            {
                LED1 = !LED1;
            }
            break;
        case KEY1_PIN:
            if(KEY1_GET == 0)
            {
                LED0 = !LED0;
                LED1 = !LED1;
            }
            break;
    }
}

void EXTI_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    // KEY_UP�ߵ�ƽ��Ч
    GPIO_InitStructure.Pin = KEY_UP_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING; // �����ش���
    GPIO_InitStructure.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
    // ��ͨ�����͵�ƽ��Ч
    GPIO_InitStructure.Pin = KEY0_PIN | KEY1_PIN | KEY2_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_IT_FALLING; // �½��ش���
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStructure);
    // NVIC����
    // �ж���0��PA0���ӵ�EXTI0��KEY_up��
    HAL_NVIC_SetPriority(EXTI0_IRQn, 2, 0); // ��ռ���ȼ�Ϊ2�������ȼ�Ϊ0
    HAL_NVIC_EnableIRQ(EXTI0_IRQn); // ʹ���ж�
    // �ж���2��PE2���ӵ�EXTI2��KEY2��
    HAL_NVIC_SetPriority(EXTI2_IRQn, 2, 1); // ��ռ���ȼ�Ϊ2�������ȼ�Ϊ
    HAL_NVIC_EnableIRQ(EXTI2_IRQn);
    // �ж���3��PE3���ӵ�EXTI3��KEY1��
    HAL_NVIC_SetPriority(EXTI3_IRQn, 2, 2); // ��ռ���ȼ�Ϊ2�������ȼ�Ϊ
    HAL_NVIC_EnableIRQ(EXTI3_IRQn);
    // �ж���4��PE4���ӵ�EXTI4��KEY0��
    HAL_NVIC_SetPriority(EXTI4_IRQn, 2, 3); // ��ռ���ȼ�Ϊ2�������ȼ�Ϊ
    HAL_NVIC_EnableIRQ(EXTI4_IRQn);
}

