#ifndef __I2C_GPIO_H__
#define __I2C_GPIO_H__

#include "stm32f10x.h"

#define I2C_WR 0 // 写控制位
#define I2C_RD 1 // 读控制位

// GPIO端口定义
#define I2C_PORT GPIOB
#define I2C_SCL_PIN GPIO_Pin_6
#define I2C_SDA_PIN GPIO_Pin_7

// 读写SCL和SDA
#define I2C_SCL_1() GPIO_SetBits(I2C_PORT, I2C_SCL_PIN)
#define I2C_SCL_0() GPIO_ResetBits(I2C_PORT, I2C_SCL_PIN)
#define I2C_SDA_1() GPIO_SetBits(I2C_PORT, I2C_SDA_PIN)
#define I2C_SDA_0() GPIO_ResetBits(I2C_PORT, I2C_SDA_PIN)
#define I2C_SDA_READ() GPIO_ReadInputDataBit(I2C_PORT, I2C_SDA_PIN)

void i2c_gpio_Init(void);
static void i2c_delay(void);
void i2c_start(void);
void i2c_stop(void);
void i2c_SendByte(uint8_t Byte);
uint8_t i2c_ReadByte(void);
uint8_t i2c_WaitAck(void);
void i2c_Ack(void);
void i2c_Nack(void);
uint8_t i2c_CheckDevice(uint8_t Device_Address);


#endif /* __I2C_GPIO_H__ */
