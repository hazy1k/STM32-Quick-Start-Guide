#include "led.h"
#include "usart.h"

static void Show_Info(void) // 显示RGB彩灯控制信息
{
	USART_SendString(USARTx, (uint8_t*)"Please input the RGB value:\r\n");
	USART_SendString(USARTx, (uint8_t*)"1  -----   红色\n");
	USART_SendString(USARTx, (uint8_t*)"2  -----   绿色\n");
	USART_SendString(USARTx, (uint8_t*)"3  -----   蓝色\n");
	USART_SendString(USARTx, (uint8_t*)"4  -----   黄色\n");
	USART_SendString(USARTx, (uint8_t*)"5  -----   紫色\n");
	USART_SendString(USARTx, (uint8_t*)"6  -----   青色\n");
	USART_SendString(USARTx, (uint8_t*)"7  -----   白色\n");
	USART_SendString(USARTx, (uint8_t*)"8  -----   关闭\n");
}

int main(void)
{
	char temp;
    // 初始化外设，打印提示信息
    LED_Init();
    USART_Config();
    Show_Info();
    while(1)
    {
		temp = USART_ReceiveData(USARTx);
		switch(temp)
		{
			case '1': LED_RED(); break;
			case '2': LED_GREEN(); break;
			case '3': LED_BLUE(); break;
			case '4': LED_YELLOW(); break;
			case '5': LED_PURPLE(); break;
			case '6': LED_CYAN(); break;
			case '7': LED_WHITE(); break;
			case '8': LED_RGBOFF(); break;
		}
	}
}
