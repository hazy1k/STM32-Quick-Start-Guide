#include "stm32f10x.h"
#include "usart.h"
#include "SysTick.h"
#include "led.h"
#include "stdio.h"

static void show_info(void)
{
  printf("\r\n LED Control Commands :\r\n");
  printf("1. RED LED ON\r\n");
  printf("2. GREEN LED ON\r\n");
  printf("3. BLUE LED ON\r\n");
  printf("4. RED LED OFF\r\n");
  printf("5. GREEN LED OFF\r\n");
  printf("6. BLUE LED OFF\r\n");
  printf("7. TURN OFF ALL LEDS");
}

int main()
{
  char ch;
	SysTick_Init();
  BSP_LED_Init();
  USARTx_Init();
  show_info();
  while(1)
  {
    ch = getchar();
		switch(ch)
    {
      case '1':
        LED_ON(RED_LED_Pin);
        break;
      case '2':
        LED_ON(GREEN_LED_Pin);
        break;
      case '3':
        LED_ON(BLUE_LED_Pin);
        break;
      case '4':
        LED_OFF(RED_LED_Pin);
        break;
      case '5':
        LED_OFF(GREEN_LED_Pin);
        break;
      case '6':
        LED_OFF(BLUE_LED_Pin);
        break;
      case '7':
        LED_OFF(RED_LED_Pin);
        LED_OFF(GREEN_LED_Pin);
        LED_OFF(BLUE_LED_Pin);
        break;
      default:
        break;  
    }
  }
}
