#include "stm32f10x.h"
#include "usart.h"
#include "I2C.h"
#include "led.h"
#include <string.h>

#define double_addr 10 // 小数存储地址
#define long_addr 70 // 长整数存储地址

uint8_t flag = 0;
uint8_t i;
long double double_buf[7] = {0};
int long_buf[7] = {0};

int main(void)
{
	LED_Init();
	USART_Config();
	I2C_EE_Init();
	printf("这是一个EEPROM读写小数和长整数的程序\r\n");
	I2C_EE_BufferRead(&flag, 0, 1); // 读取标志位
	if(flag != 0xEE) // 如果有标志位0xCD，表示之前已经写入过数据
	{
		printf("没有找到之前写入的数据，开始写入数据\r\n");
		flag = 0xEE; // 写入标志位
		I2C_EE_BufferWrite(&flag, 0, 1); // 写入标志位
		for(i = 0; i < 7; i++)
		{
			double_buf[i] = i+0.1; // 写入小数数据
			long_buf[i] = i*100+1; // 写入长整数数据
		}
		// 写入数据
		I2C_EE_BufferWrite((void*)double_buf, double_addr, sizeof(double_buf));
		I2C_EE_BufferWrite((void*)long_buf, long_addr, sizeof(long_buf));
		printf("我们向EEPROM写入的数据为：\r\n");
		for(i = 0; i < 7; i++)
		{
			printf("小数 = %LF\r\n", double_buf[i]);
			printf("长整数 = %d\r\n", long_buf[i]);
		}
		printf("写入数据成功，复位开发板，进行数据效验\r\n");
	}
	else // 如果有标志位0xCD，表示之前已经写入过数据
	{
		printf("找到之前写入的数据，开始读取数据\r\n");
		// 读取数据
		I2C_EE_BufferRead((void*)double_buf, double_addr, sizeof(double_buf));
		I2C_EE_BufferRead((void*)long_buf, long_addr, sizeof(long_buf));
		printf("我们读取的EEPROM数据为：\r\n");
		for(i = 0; i < 7; i++)
		{
			printf("小数 = %LF\r\n", double_buf[i]);
			printf("长整数 = %d\r\n", long_buf[i]);
		}
	}
	while(1)
	{
	}
}
