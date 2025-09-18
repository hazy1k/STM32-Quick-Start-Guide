#include "stm32f10x.h"
#include "usart.h"   
#include "led.h"
#include "hw_config.h" 
#include "usb_lib.h"
#include "usb_pwr.h" 

void USB_Delay(__IO uint32_t nCount)
{
	for(; nCount != 0; nCount--);
}

int main(void)
{
	USART_Config();
	LED_Init();	
	Set_System();
	Set_USBClock();
	USB_Interrupts_Config();
 	USB_Init();
 	while (bDeviceState != CONFIGURED);	 //µ»¥˝≈‰÷√ÕÍ≥…
  	while (1)
  	{
    	LED1_ON();
    	USB_Delay(0x0FFFFF);
  }
}
