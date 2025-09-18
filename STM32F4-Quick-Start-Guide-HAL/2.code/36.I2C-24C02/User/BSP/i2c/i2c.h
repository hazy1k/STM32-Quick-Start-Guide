#ifndef __I2C_H__
#define __I2C_H__

#include "sys.h"

/* IO操作 */
#define I2C_SCL(x)        do{ x ? \
                              HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET) : \
                              HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET); \
                          }while(0)

#define I2C_SDA(x)        do{ x ? \
                              HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET) : \
                              HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET); \
                          }while(0)
#define I2C_READ_SDA     HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9)

void i2c_start(void);
void i2c_stop(void);
void i2c_ack(void);
void i2c_nack(void);
uint8_t i2c_wait_ack(void);
void i2c_send_byte(uint8_t txd);
uint8_t i2c_read_byte(uint8_t ack);
void i2c_init(void);

#endif /* __I2C_H__ */
