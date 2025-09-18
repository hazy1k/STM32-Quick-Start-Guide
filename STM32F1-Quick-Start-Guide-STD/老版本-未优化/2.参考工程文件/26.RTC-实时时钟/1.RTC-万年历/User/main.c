#include "stm32f10x.h"
#include "./usart/bsp_usart.h"
#include "./rtc/bsp_rtc.h"
#include "./lcd/bsp_ili9341_lcd.h"
#include "./key/bsp_key.h"  

struct rtc_time systmtime=
{
0,0,0,1,1,2000,0
};

extern __IO uint32_t TimeDisplay ;

int main()
{		
#ifdef  USE_LCD_DISPLAY
	
		ILI9341_Init ();       
		LCD_SetFont(&Font8x16);
		LCD_SetColors(RED,BLACK);

		ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);

		ILI9341_DispStringLine_EN(LINE(0),"        BH RTC demo");
#endif
	
	  USART_Config();		
		Key_GPIO_Config();
	  RTC_NVIC_Config();
	  RTC_CheckAndConfig(&systmtime);
	  while (1)
	  {
	    if (TimeDisplay == 1)
	    {
	      Time_Display( RTC_GetCounter(),&systmtime); 		  
	      TimeDisplay = 0;
	    }
			
			if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON  )
			{
				struct rtc_time set_time;
				Time_Regulate_Get(&set_time);
				Time_Adjust(&set_time);
				BKP_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);

			} 			
	  }
}
