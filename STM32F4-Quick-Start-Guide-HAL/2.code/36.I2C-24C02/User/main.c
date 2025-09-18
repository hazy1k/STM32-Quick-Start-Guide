#include "bsp_init.h"
#include "stdio.h"
#include "24c02.h"

// 要写入的字符串
const uint8_t text_buf[] = {"STM32 I2C TEST"};
#define TEXT_SIZE sizeof(text_buf)

int main(void)
{
	uint16_t i = 0;
	uint8_t data[TEXT_SIZE];
	bsp_init();
	at24cxx_init();
	LCD_ShowString(30,110,200,16,16,"KEY1:Write  KEY0:Read");
	while(at24cxx_check())
	{
		LCD_ShowString(30,130,200,16,16,"24C02 Check Failed!");
	}
	LCD_ShowString(30,130,200,16,16,"24C02 Ready!");
	while(1)
	{
		if(HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY0_GPIO_Pin) == 0) // KEY0按下写入
		{
			LCD_ShowString(30,150,200,16,16,"Start Write 24C02....");
			at24cxx_write(0, (uint8_t*)text_buf, TEXT_SIZE);
			LCD_ShowString(30,150,200,16,16,"24C02 Write Finished!");
		}
		if(HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY1_GPIO_Pin) == 0)
		{
			LCD_ShowString(30,150,200,16,16,"Start Read 24C02.... ");
			at24cxx_read(0, data, TEXT_SIZE);
			LCD_ShowString(30,150,200,16,16,"The Data Readed Is:  ");
			LCD_ShowString(30,170,200,16,16,(char*)data);
		}
		i++;
		if(i == 20)
		{
			LED_TOGGLE(LED0_GPIO_Pin);
			i = 0;
		}
	delay_ms(10);
	}
}
