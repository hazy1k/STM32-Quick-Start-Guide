#ifndef __I2C_H
#define __I2C_H

#include "stm32f10x.h"

#define EEPROM_ADDR 0xA0
#define EEPROM_PAGE_SIZE 8
#define EEPROM_SIZE 256

uint8_t EEPROM_CheckOk(void);
uint8_t EEPROM_ReadBytes(uint8_t *_pReadBuf, uint16_t _usAddress, uint16_t _usSize);
uint8_t EEPROM_WriteBytes(uint8_t *_pWriteBuf, uint16_t _usAddress, uint16_t _usSize);
void EEPROM_Erase(void); 

#endif /* __I2C_H */
