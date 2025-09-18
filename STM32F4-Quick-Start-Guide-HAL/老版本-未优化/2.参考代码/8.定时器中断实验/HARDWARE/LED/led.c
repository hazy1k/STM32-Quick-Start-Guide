#include "led.h"

void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOF_CLK_ENABLE(); // ��ʼ��GPIOFʱ��          
    GPIO_Initure.Pin = LED0_PIN | LED1_PIN;  // ʹ��LED0_PIN��LED1_PIN
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP; // �������
    GPIO_Initure.Pull = GPIO_PULLUP;         // ����
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;    // ����
    HAL_GPIO_Init(GPIOF, &GPIO_Initure);

    // ��ʼĬ��ȫ��
    HAL_GPIO_WritePin(LED1_GPIO, LED1_PIN, GPIO_PIN_SET); 
    HAL_GPIO_WritePin(LED0_GPIO, LED0_PIN, GPIO_PIN_SET); 
}

// void HAL_GPIO_WritePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState)
// �ú�����������ָ��GPIO�˿ڵ�ָ�����ŵĵ�ƽ״̬
// ����������
// GPIOx��GPIO�˿ڣ�������GPIOA��GPIOB��GPIOC��GPIOD��GPIOE��GPIOF�ȡ�
// GPIO_PIN��GPIO���ţ�������GPIO_PIN_0~GPIO_PIN_15��
// PinState��GPIO���ŵĵ�ƽ״̬��������GPIO_PIN_RESET��GPIO_PIN_SET��
// ����GPIO_PIN_RESETΪ�͵�ƽ��GPIO_PIN_SETΪ�ߵ�ƽ��
