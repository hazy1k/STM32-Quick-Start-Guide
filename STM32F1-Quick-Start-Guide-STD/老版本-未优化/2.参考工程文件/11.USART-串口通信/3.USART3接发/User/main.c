#include "usart.h"
#include "stm32f10x.h"
#include "stdio.h"

int main(void)
{
  USART_Config();
	USART_SendString(USARTx, (uint8_t*)"USART3 Test\r\n");
  while(1)
  {

  }
}
