#include "stm32f10x.h"
#include "fatfs_flash_spi.h"
#include "usart.h"	
#include "led.h"  
#include "hw_config.h" 
#include "usb_lib.h"
#include "usb_pwr.h"

static void USB_Delay(__IO uint32_t nCount)
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
 	while (bDeviceState != CONFIGURED);	
  while (1)
  {
    LED2_ON();
    USB_Delay(0x0FFFFF);
  }
}
