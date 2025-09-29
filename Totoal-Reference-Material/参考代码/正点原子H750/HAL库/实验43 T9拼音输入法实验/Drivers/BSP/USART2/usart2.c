/**
 ****************************************************************************************************
 * @file        usart2.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-03
 * @brief       串口2 驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 STM32H750开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.0 20200403
 * 第一次发布
 *
 ****************************************************************************************************
 */

#include "./BSP/USART2/usart2.h"


UART_HandleTypeDef g_uart2_handle;  /* USART2句柄 */

/**
 * @brief       串口2初始化函数
 * @note        注意:串口2的时钟源频率在sys已经设置过了
 * @param       baudrate: 波特率, 根据自己需要设置波特率值
 */
void usart2_init(uint32_t baudrate)
{
    GPIO_InitTypeDef gpio_init_struct;

    __HAL_RCC_USART2_CLK_ENABLE();  /* 使能USART2时钟 */
    USART2_TX_GPIO_CLK_ENABLE();    /* 使能串口TX脚时钟 */
    USART2_RX_GPIO_CLK_ENABLE();    /* 使能串口RX脚时钟 */

    gpio_init_struct.Pin = USART2_TX_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_AF_PP;                /* 复用推挽输出 */
    gpio_init_struct.Pull = GPIO_PULLUP;                    /* 上拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;     /* 高速 */
    gpio_init_struct.Alternate = USART2_TX_GPIO_AF;         /* 复用为USART2 */
    HAL_GPIO_Init(USART2_TX_GPIO_PORT, &gpio_init_struct);  /* 初始化串口TX引脚 */

    gpio_init_struct.Pin = USART2_RX_GPIO_PIN;
    gpio_init_struct.Alternate = USART2_RX_GPIO_AF;         /* 复用为USART2 */
    HAL_GPIO_Init(USART2_RX_GPIO_PORT, &gpio_init_struct);  /* 初始化串口RX引脚 */

    g_uart2_handle.Instance = USART2;                       /* USART2 */
    g_uart2_handle.Init.BaudRate = baudrate;                /* 波特率 */
    g_uart2_handle.Init.WordLength = UART_WORDLENGTH_8B;    /* 字长为8位数据格式 */
    g_uart2_handle.Init.StopBits = UART_STOPBITS_1;         /* 一个停止位 */
    g_uart2_handle.Init.Parity = UART_PARITY_NONE;          /* 无奇偶校验位 */
    g_uart2_handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;    /* 无硬件流控 */
    g_uart2_handle.Init.Mode = UART_MODE_TX;                /* 发模式 */
    HAL_UART_Init(&g_uart2_handle);                         /* 使能USART2 */
}






