#include "stm32f10x.h"
#include "usart.h"
#include "SysTick.h"

int main()
{
	SysTick_Init();
    USARTx_Init();
    usart_sendstring(USARTx, "hello world\r\n");
    while(1)
    {
		usart_sendstring(USARTx, "hello world\r\n");
		Delay_ms(1000);
    }
}
