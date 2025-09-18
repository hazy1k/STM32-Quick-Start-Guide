#ifndef __RS485_H__
#define __RS485_H__

#include "sys.h"

#define RS485_EN_RX 1     // 使能接收使能
#define RS485_REC_LEN 64  // 定义最大接收字节数 64

/* 控制RS485_RE脚, 控制RS485发送/接收状态
 * RS485_RE = 0, 进入接收模式
 * RS485_RE = 1, 进入发送模式
 */
#define RS485_RE(x)   do{ x ? \
                          HAL_GPIO_WritePin(GPIOG, GPIO_PIN_9, GPIO_PIN_SET) : \
                          HAL_GPIO_WritePin(GPIOG, GPIO_PIN_9, GPIO_PIN_RESET); \
                      }while(0)


void rs485_init(uint32_t baudrate);
void rs485_send_data(uint8_t *buf, uint8_t len);
void rs485_receive_data(uint8_t *buf, uint8_t *len);

#endif /* __RS485_H__ */
