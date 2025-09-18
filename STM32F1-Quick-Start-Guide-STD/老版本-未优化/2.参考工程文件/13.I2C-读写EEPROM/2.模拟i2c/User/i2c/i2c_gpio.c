#include "i2c_gpio.h"

void i2c_gpio_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = I2C_SCL_PIN | I2C_SDA_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	i2c_stop();
}

// 模拟I2C总线位延迟，最快400KHz
static void i2c_delay(void)
{
	uint8_t i;
	for(i = 0; i < 10; i++);
}

// MCU发起I2C总线启动信号
void i2c_start(void)
{
	// 当SCL高电平时，SDA出现一个下降沿表示I2C总线启动信号
	I2C_SCL_1();
	I2C_SDA_1();
	i2c_delay();
	I2C_SDA_0(); // 拉低SDA，产生一个启动信号
	i2c_delay();
	I2C_SCL_0(); // 拉低SCL，产生一个时钟
	i2c_delay();
}

// MCU发起I2C总线停止信号
void i2c_stop(void)
{
	// 当SCL高电平，SDA低电平时，SDA出现一个上升沿表示I2C总线停止信号
	I2C_SCL_1();
	I2C_SDA_0();
	i2c_delay();
	I2C_SDA_1(); // 拉高SDA，产生一个停止信号
}

// MCU向I2C总线设备发送8bit数据
void i2c_SendByte(uint8_t Byte)
{
	uint8_t i;
	for(i = 0; i < 8; i++ ) // 先发送高7位
	{
		if(Byte & 0x80) // 若最高位为1，则SDA为高电平
		{
			I2C_SDA_1();
		}
		else
		{
			I2C_SDA_0();
		}
		i2c_delay();
		I2C_SCL_1();
		i2c_delay();
		I2C_SCL_0();
		if(i == 7)
		{
			I2C_SDA_1(); // 释放总线
		}
		Byte <<= 1; // 左移一位
		i2c_delay();
	}
}

// MCU从I2C总线设备接收8bit数据
uint8_t i2c_ReadByte(void)
{
	uint8_t i, value = 0;
	for(i = 0; i < 8; i++)
	{
		value <<= 1; // 左移一位
		I2C_SCL_1();
		i2c_delay();
		if (I2C_SDA_READ())
		{
			value++;
		}
		I2C_SCL_0();
		i2c_delay();
	}
	return value;
}

// 等待MCU产生一个时钟，读取器件的ACK应答信号
uint8_t i2c_WaitAck(void)
{
	uint16_t STA;
	I2C_SDA_1(); 
	i2c_delay();
	I2C_SCL_1();
	i2c_delay();
	if(I2C_SDA_READ())
	{
		STA = 1;
	}
	else
	{
		STA = 0;
	}
	I2C_SCL_0();
	i2c_delay();
	return STA;
}

// MCU产生一个ACK信号
void i2c_Ack(void)
{
	I2C_SDA_0();
	i2c_delay();
	I2C_SCL_1();
	i2c_delay();
	I2C_SCL_0();
	i2c_delay();
	I2C_SDA_1();
}

// MCU产生一个NACK信号
void i2c_Nack(void)
{
	I2C_SDA_1();
	i2c_delay();
	I2C_SCL_1();
	i2c_delay();
	I2C_SCL_0();
	i2c_delay();
}

// 检测I2C总线设备，MCU发送设备地址，读取设备应答状况判断设备是否存在
uint8_t i2c_CheckDevice(uint8_t Device_Address)
{
	uint8_t ucAck;
	i2c_gpio_Init();
	i2c_start();
	i2c_SendByte(Device_Address | I2C_WR); // 发送设备地址+写位
	ucAck = i2c_WaitAck(); // 等待ACK应答
	i2c_stop();
	return ucAck;
}
