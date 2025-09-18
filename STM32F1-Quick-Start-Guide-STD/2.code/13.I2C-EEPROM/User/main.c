#include "stm32f10x.h"
#include "led.h"
#include "SysTick.h"
#include "usart.h"
#include "i2c_ee.h"
#include <stdio.h>

#define EEPROM_Firstpage 0x00 // EEPROM的起始地址
#define PAGE_SIZE 8           // AT24C02页大小
#define BUFFER_SIZE 256
#define I2C_TIMEOUT 10000     // 超时计数器值

uint8_t I2c_Buf_Write[BUFFER_SIZE]; // 写入缓冲区
uint8_t I2c_Buf_Read[BUFFER_SIZE];  // 读取缓冲区

uint8_t I2C_TEST(void);

int main(void) 
{
    SysTick_Init();
    BSP_LED_Init();
    USARTx_Init();
    I2C_EEPROM_Init();
    
    // 等待所有初始化完成
    Delay_ms(100);
    
    if(I2C_TEST() == 1)
    {
        LED_ON(GREEN_LED_Pin);
    }
    else
    {
        LED_ON(RED_LED_Pin);
    }
    while(1);
}

// I2C EEPROM读写测试
uint8_t I2C_TEST(void)
{
    uint16_t i = 0;
    
    // 初始化写入缓冲区
    printf("Preparing write data:\r\n");
    for(i = 0; i < BUFFER_SIZE; i++)
    {   
        I2c_Buf_Write[i] = i;
        printf("0x%02X ", I2c_Buf_Write[i]); 
        if(i % 16 == 15)   
            printf("\r\n");    
    }
    printf("\r\n");
    
    // 分页写入（AT24C02页大小为8字节）
    printf("Writing data to EEPROM...\r\n");
    uint16_t addr = EEPROM_Firstpage;
    uint8_t writeSuccess = 1;
    
    for(i = 0; i < BUFFER_SIZE; i += PAGE_SIZE)
    {
        uint8_t bytesToWrite = (BUFFER_SIZE - i) > PAGE_SIZE ? PAGE_SIZE : (BUFFER_SIZE - i);
        
        if(!I2C_EEPROM_WritePage(addr, &I2c_Buf_Write[i], bytesToWrite))
        {
            printf("Write failed at address 0x%02X\r\n", addr);
            writeSuccess = 0;
            break;
        }
        
        addr += bytesToWrite;
        Delay_ms(5); // EEPROM需要5ms写入时间
        printf(".");
    }
    
    if(!writeSuccess)
    {
        printf("\r\nWrite operation failed!\r\n");
        return 0;
    }
    
    printf("\r\nWrite completed successfully!\r\n");
    
    // 等待EEPROM完成所有内部写入
    Delay_ms(20);
    
    // 读取数据
    printf("Reading data from EEPROM...\r\n");
    if(!I2C_EEPROM_ReadBuffer(EEPROM_Firstpage, I2c_Buf_Read, BUFFER_SIZE))
    {
        printf("Read failed!\r\n");
        return 0;
    }
    printf("Read completed successfully!\r\n");
    
    // 验证数据
    printf("\r\nVerifying data:\r\n");
    uint8_t errors = 0;
    for(i = 0; i < BUFFER_SIZE; i++)
    {   
        if(I2c_Buf_Read[i] != I2c_Buf_Write[i])
        {
            errors++;
            printf("Error at 0x%02X: Wrote 0x%02X, Read 0x%02X\r\n", 
                  i, I2c_Buf_Write[i], I2c_Buf_Read[i]);
        }
        else 
        {
            printf("0x%02X ", I2c_Buf_Read[i]);
        }
        
        if(i % 16 == 15)  
            printf("\r\n");
    }
    
    if(errors > 0)
    {
        printf("\r\n%d errors found!\r\n", errors);
        return 0;
    }
    
    printf("\r\nAll data verified successfully!\r\n");
    return 1;
}
