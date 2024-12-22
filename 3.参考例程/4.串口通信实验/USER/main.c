#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "string.h"

extern UART_HandleTypeDef UART1_Handler; // ���ھ��

int main(void)
{
    HAL_Init();
		Stm32_Clock_Init(336,8,2,7);
    UART_Init(115200);  // ��ʼ������1��������115200

    uint8_t buf[100];   // ���ջ�����
    uint16_t len = 0;   // ���յ��ĳ���

    while(1)
    {
        memset(buf, 0, sizeof(buf));  // ��ս��ջ�����
        HAL_UART_Receive(&UART1_Handler, buf, 100, 100); // ��������
        
        // �ҵ����յ�������ĩβ���ֶ���ӽ�����
        len = strlen((char*)buf); 
        if (len > 0 && buf[len - 1] != '\0') 
        {
            buf[len] = '\0'; // ȷ���ַ�����NULL��β
        }

        HAL_UART_Transmit(&UART1_Handler, buf, len, 100); // ���ͽ��յ����ַ���
    }
}
