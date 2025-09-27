#ifndef __DEBUG_USART_H
#define	__DEBUG_USART_H

#include "stm32f1xx.h"
#include <stdio.h>

//串口接收缓冲数组大小
#define UART_RX_BUFFER_SIZE 256 

extern unsigned char UART_RxBuffer[UART_RX_BUFFER_SIZE];
extern uint8_t receive_cmd;

//串口波特率
#define DEBUG_USART_BAUDRATE                    115200

//引脚定义
/*******************************************************/
#define DEBUG_USART                             USART1
#define DEBUG_USART_CLK_ENABLE()                __HAL_RCC_USART1_CLK_ENABLE();

#define DEBUG_USART_RX_GPIO_PORT                GPIOA
#define DEBUG_USART_RX_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOA_CLK_ENABLE()
#define DEBUG_USART_RX_PIN                      GPIO_PIN_10

#define DEBUG_USART_TX_GPIO_PORT                GPIOA
#define DEBUG_USART_TX_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOA_CLK_ENABLE()
#define DEBUG_USART_TX_PIN                      GPIO_PIN_9

#define DEBUG_USART_IRQHandler                  USART1_IRQHandler
#define DEBUG_USART_IRQ                 		    USART1_IRQn
/************************************************************/

/* 上位机宏定义 */

/* 数据头结构体 */
typedef __packed struct
{
  uint32_t head;    // 包头
  uint8_t ch;       // 通道
  uint32_t len;     // 包长度
  uint8_t cmd;      // 命令
//  uint8_t sum;      // 校验和
  
}packet_head_t;

#define PACKET_HEAD     0x59485A53    // 包头

/* 通道宏定义 */
#define CURVES_CH1      0x01
#define CURVES_CH2      0x02
#define CURVES_CH3      0x03
#define CURVES_CH4      0x04
#define CURVES_CH5      0x05

/* 指令(下位机 -> 上位机) */
#define SEED_TARGET_CMD      0x01     // 发送上位机通道的目标值
#define SEED_FACT_CMD        0x02     // 发送通道实际值
#define SEED_P_I_D_CMD       0x03     // 发送 PID 值（同步上位机显示的值）
#define SEED_START_CMD       0x04     // 发送启动指令（同步上位机按钮状态）
#define SEED_STOP_CMD        0x05     // 发送停止指令（同步上位机按钮状态）
#define SEED_PERIOD_CMD      0x06     // 发送周期（同步上位机显示的值）

/* 指令(上位机 -> 下位机) */
#define SET_P_I_D_CMD        0x10     // 设置 PID 值
#define SET_TARGET_CMD       0x11     // 设置目标值
#define START_CMD            0x12     // 启动指令
#define STOP_CMD             0x13     // 停止指令
#define RESET_CMD            0x14     // 复位指令
#define SET_PERIOD_CMD       0x15     // 设置周期

/* 索引值宏定义 */
#define HEAD_INDEX_VAL       0x3u     // 包头索引值（4字节）
#define CHX_INDEX_VAL        0x4u     // 通道索引值（1字节）
#define LEN_INDEX_VAL        0x8u     // 包长索引值（4字节）
#define CMD_INDEX_VAL        0x9u     // 命令索引值（1字节）

#define EXCHANGE_H_L_BIT(data)      ((((data) << 24) & 0xFF000000) |\
                                     (((data) <<  8) & 0x00FF0000) |\
                                     (((data) >>  8) & 0x0000FF00) |\
                                     (((data) >> 24) & 0x000000FF))     // 交换高低字节

#define COMPOUND_32BIT(data)        (((*(data-0) << 24) & 0xFF000000) |\
                                     ((*(data-1) << 16) & 0x00FF0000) |\
                                     ((*(data-2) <<  8) & 0x0000FF00) |\
                                     ((*(data-3) <<  0) & 0x000000FF))      // 合成为一个字

void uart_FlushRxBuffer(void);
void Usart_SendByte(uint8_t str);
void Usart_SendString(uint8_t *str);
void DEBUG_USART_Config(void);
//int fputc(int ch, FILE *f);
extern UART_HandleTypeDef UartHandle;
void set_computer_value(uint8_t cmd, uint8_t ch, void *data, uint8_t num);
uint8_t check_sum(uint8_t init, uint8_t *ptr,uint8_t len);
void parse_data(void);
#endif /* __USART1_H */
