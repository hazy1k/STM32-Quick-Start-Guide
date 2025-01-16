#include "key.h"
#include "delay.h"

void KEY_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    // ���Ⱦ��ǿ���ʱ����
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    // ����������GPIO
    GPIO_InitStructure.Pin = KEY_UP_PIN;            // key_up����
    GPIO_InitStructure.Mode = GPIO_MODE_INPUT;      // ����ģʽ
    GPIO_InitStructure.Pull = GPIO_PULLDOWN;        // ����
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;     // ����
    HAL_GPIO_Init(KEY_UP_GPIO, &GPIO_InitStructure);// ��ʼ��GPIO
    GPIO_InitStructure.Pin = KEY0_PIN|KEY1_PIN|KEY2_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.Pull = GPIO_PULLUP;           // ����
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStructure);
}

// ����ɨ�躯��˵��
/*
    ���ܣ�ɨ�谴�������ذ���ֵ
    ��������mode��0��ʾֻɨ��һ�Σ�1��ʾһֱɨ��
    ����ֵ��0��û���κΰ������£�1��KEY_UP����
    ��Ӧ���ȼ���KEY0 > KEY1 > KEY2 > KEY_UP
*/
uint8_t KEY_Scan(uint8_t mode)
{
    static uint8_t key_flag = 1; // �����ɿ���־λ
    if(mode == 1)
       key_flag = 1;
    // �ж��Ƿ��а�������
    if(key_flag && (KEY0_GET == 0 || KEY1_GET == 0 || KEY2_GET == 0 || KEY_UP_GET == 1)) // ��Ҫ������KEY_UP�Ǹߵ�ƽ��Ч   
    {
        delay_ms(10);
        key_flag = 0; // �������±�־λ
        if(KEY0_GET == 0) return KEY0_PRES;
        else if(KEY1_GET == 0) return KEY1_PRES;
        else if(KEY2_GET == 0) return KEY2_PRES;
        else if(KEY_UP_GET == 1) return KEY_UP_PRES;
    }
    else if(KEY0_GET == 1 && KEY1_GET == 1 && KEY2_GET == 1 && KEY_UP_GET == 0)
    {
        key_flag = 1; // �����ɿ�
        return 0;
    }
    return 0;
}       
