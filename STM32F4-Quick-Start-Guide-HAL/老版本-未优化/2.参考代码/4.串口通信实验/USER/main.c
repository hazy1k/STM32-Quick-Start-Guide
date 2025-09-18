#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "string.h"

extern UART_HandleTypeDef UART1_Handler; // 串口句柄

int main(void)
{
    HAL_Init();
		Stm32_Clock_Init(336,8,2,7);
    UART_Init(115200);  // 初始化串口1，波特率115200

    uint8_t buf[100];   // 接收缓冲区
    uint16_t len = 0;   // 接收到的长度

    while(1)
    {
        memset(buf, 0, sizeof(buf));  // 清空接收缓冲区
        HAL_UART_Receive(&UART1_Handler, buf, 100, 100); // 接收数据
        
        // 找到接收到的数据末尾并手动添加结束符
        len = strlen((char*)buf); 
        if (len > 0 && buf[len - 1] != '\0') 
        {
            buf[len] = '\0'; // 确保字符串以NULL结尾
        }

        HAL_UART_Transmit(&UART1_Handler, buf, len, 100); // 发送接收到的字符串
    }
}
