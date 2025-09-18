#include "bsp_init.h"
#include "flash.h"
#include "spi.h"

const uint8_t text_buf[] = "STM32 SPI TEST";
#define TEXT_SIZE sizeof(text_buf)

int main(void)
{
	uint8_t key_value;
	uint16_t i = 0;
	uint8_t data_buf[TEXT_SIZE];
	uint32_t flash_size;
    bsp_init();   
	spi_flash_init();
	LCD_ShowString(30,110,200,16,16,"KEY1:Write  KEY0:Read");
	LCD_ShowString(30,130,200,16,16,"SPI FLASH Ready!");
    while(1)
    {
		key_value = key_scan(0);
		if(key_value == KEY1_Press)
		{
			LCD_ShowString(30,150,200,16,16,"Start Write FLASH....");
			sprintf((char*)data_buf, "%s%d", (char*)text_buf, i);
			flash_write((uint8_t*)data_buf, flash_size-100, TEXT_SIZE);
			LCD_ShowString(30,150,200,16,16,"FLASH Write Finished!");
		}
		if(key_value == KEY0_Press)
		{
			LCD_ShowString(30,150,200,16,16,"Start Read FLASH... .");
			flash_read(data_buf, flash_size-100, TEXT_SIZE);
			LCD_ShowString(30,150,200,16,16,"FLASH Read Finished! ");
			LCD_ShowString(30,170,200,16,16,(char*)data_buf);
		}
		i++;
		if(i == 20)
		{
			LED_TOGGLE(LED1_GPIO_Pin);
			i = 0;
		}
        delay_ms(10); 
    }
}
