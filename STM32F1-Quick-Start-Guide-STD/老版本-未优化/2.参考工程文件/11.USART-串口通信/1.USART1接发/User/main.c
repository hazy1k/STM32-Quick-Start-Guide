#include "usart.h"
#include "stm32f10x.h"
#include "stdio.h"

int main(void)
{
  USART_Config();
  // ²âÊÔ·¢ËÍÒ»¸ö×Ö·û´®
	USART_SendString(USARTx, "USART1 Test\r\n");
  while(1)
  {

  }
}
