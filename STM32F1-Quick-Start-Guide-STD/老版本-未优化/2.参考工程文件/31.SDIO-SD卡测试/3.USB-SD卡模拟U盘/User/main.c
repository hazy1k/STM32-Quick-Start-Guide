#include "stm32f10x.h"
#include "sdio_sdcard.h"
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
	Set_System(); // Set system clock to 72MHz
	Set_USBClock(); // Set USB clock to 48MHz
	USB_Interrupts_Config(); // Configure USB interrupts
 	USB_Init(); // Initialize USB
 	while(bDeviceState != CONFIGURED); // Wait for USB device to be configured	
	printf("\r\n STM32 USB TEST\r\n");
	while (1)
  	{
    	LED1_ON();
    	USB_Delay(0x0FFFFF);
  }
}
