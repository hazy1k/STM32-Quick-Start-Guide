#include "stm32f10x.h"
#include "chipid.h"
#include "usart.h"

extern uint32_t Unique_ID[3];

int main(void)
{
  USART_Config();
  Get_Unique_ID();
  printf("Unique ID: %08X-%08X-%08X\r\n", Unique_ID[0],Unique_ID[1],Unique_ID[2]);
  printf("–æ∆¨flash¥Û–°: %d KB\r\n", *(__IO uint16_t*)(0x1FFFF7E0));
  while(1);
}
