#include "led.h"
#include "I2C.h"
#include "i2c_gpio.h"
#include "usart.h"

static void ee_Delay(__IO uint32_t nCount);
uint8_t EEPROM_Test(void);

int main()
{
    LED_Init();
    USART_Config();
    LED_BLUE();
    if(EEPROM_Test() == 1)
    {
        LED_GREEN();
    }
    else
    {
        LED_RED();
    }
    while(1)
    {
    }
}

// EEPROM测试函数
uint8_t EEPROM_Test(void)
{
    uint16_t i;
    uint8_t write_buf[EEPROM_SIZE];
    uint8_t read_buf[EEPROM_SIZE];
    if(EEPROM_CheckOk() == 0)    
    {
        printf("没有找到EEPROM\r\n");
        return 0;
    }
    for(i=0;i<EEPROM_SIZE;i++)
    {
        write_buf[i] = i;
    }
    if(EEPROM_WriteBytes(write_buf, 0, EEPROM_SIZE) == 0)
    {
        printf("EEPROM写入失败\r\n");
        return 0;
    }
    else
    {
        printf("EEPROM写入成功\r\n");
    }
    ee_Delay(0x0FFFFF);
    if(EEPROM_ReadBytes(read_buf, 0, EEPROM_SIZE) == 0)
    {
        printf("EEPROM读取失败\r\n");
        return 0;
    }
    else
    {
        printf("EEPROM读取成功,数据如下:\r\n");
    }
    for(i=0;i<EEPROM_SIZE;i++)
    {
        if(read_buf[i]!= write_buf[i])
        {
            printf("0x%02X", read_buf[i]);
            printf("错误，两次读取数据不一致\r\n");
            return 0;
        }
        printf("0x%02X ", read_buf[i]);
        if((i & 15) == 15)
        {
            printf("\r\n");
        }
    }
    printf("EEPROM测试通过\r\n");
    return 1;
}

static void ee_Delay(__IO uint32_t nCount) // 简单的延时函数
{
    for(; nCount != 0; nCount--);
}
