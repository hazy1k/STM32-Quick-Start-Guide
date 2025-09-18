#ifndef I2C_EEPROM_H
#define I2C_EEPROM_H

#include "stm32f10x.h"

// 硬件配置
#define EEPROM_I2C             I2C1
#define EEPROM_I2C_CLK         RCC_APB1Periph_I2C1
#define EEPROM_GPIO_PORT       GPIOB
#define EEPROM_GPIO_CLK        RCC_APB2Periph_GPIOB
#define EEPROM_SCL_PIN         GPIO_Pin_6
#define EEPROM_SDA_PIN         GPIO_Pin_7
#define EEPROM_ADDRESS         0xA0    // 7位地址 (0x50 << 1)
#define EEPROM_PAGE_SIZE       8       // AT24C02页大小
/* 
    AT24C02 2kb = 2048bit = 2048/8 B = 256 B
    32 pages of 8 bytes each
    Device Address
    1 0 1 0 A2 A1 A0 R/W
    1 0 1 0 0  0  0  0 = 0XA0
    1 0 1 0 0  0  0  1 = 0XA1 
    EEPROM地址宏定义
    #define EEPROM_Block0_ADDRESS 0xA0   
    #define EEPROM_Block1_ADDRESS 0xA2 
    #define EEPROM_Block2_ADDRESS 0xA4  
    #define EEPROM_Block3_ADDRESS 0xA6
*/  

// 超时设置
#define I2C_TIMEOUT            100000  // 超时计数器值
// 初始化函数
void I2C_EEPROM_Init(void);
// 读写函数
uint8_t I2C_EEPROM_WriteByte(uint16_t addr, uint8_t data);
uint8_t I2C_EEPROM_ReadByte(uint16_t addr, uint8_t* data);
uint8_t I2C_EEPROM_WritePage(uint16_t addr, uint8_t* data, uint8_t len);
uint8_t I2C_EEPROM_ReadBuffer(uint16_t addr, uint8_t* buffer, uint16_t len);

#endif // I2C_EEPROM_H