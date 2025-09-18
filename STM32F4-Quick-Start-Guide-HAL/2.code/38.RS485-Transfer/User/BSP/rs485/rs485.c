#include "rs485.h"
#include "delay.h"

UART_HandleTypeDef rs485_handle;

#ifdef RS485_EN_RX

uint8_t rs485_rx_buf[RS485_REC_LEN];
uint8_t rs485_rx_cnt = 0;

// 接收中断处理
void USART2_IRQHandler(void)
{
    uint8_t data;
    if((__HAL_UART_GET_FLAG(&rs485_handle, UART_FLAG_RXNE) != RESET))
    {
        HAL_UART_Receive(&rs485_handle, &data, 1, 1000);
        if(rs485_rx_cnt < RS485_REC_LEN) // 接收缓冲区未满
        {
            rs485_rx_buf[rs485_rx_cnt] = data; // 存入接收缓冲区
            rs485_rx_cnt++;
        }
    }

}

#endif // RS485_EN_RX

// 初始化RS485
void rs485_init(uint32_t baudrate)
{
    // RE-PG8 TX-PA2 RX-PA3 UX-USART2
    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_USART2_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStructure;
    /* GPIO 初始化 */
    GPIO_InitStructure.Pin = GPIO_PIN_2 | GPIO_PIN_3;
    GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStructure.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.Pin = GPIO_PIN_8;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStructure);
    /* USART2 初始化 */
    rs485_handle.Instance = USART2;
    rs485_handle.Init.BaudRate = baudrate;
    rs485_handle.Init.WordLength = UART_WORDLENGTH_8B;
    rs485_handle.Init.StopBits = UART_STOPBITS_1;
    rs485_handle.Init.Parity = UART_PARITY_NONE;
    rs485_handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    rs485_handle.Init.Mode = UART_MODE_TX_RX;
    HAL_UART_Init(&rs485_handle);
    __HAL_UART_DISABLE_IT(&rs485_handle, UART_IT_TC);
#ifdef RS485_EN_RX
    __HAL_UART_ENABLE_IT(&rs485_handle, UART_IT_RXNE); // 使能接收中断
    HAL_NVIC_EnableIRQ(USART2_IRQn);
    HAL_NVIC_SetPriority(USART2_IRQn, 3, 3);
#endif // RS485_EN_RX
    RS485_RE(0); // 默认接收模式
}

// 发送数据
void rs485_send_data(uint8_t *buf, uint8_t len)
{
    RS485_RE(1); // 打开发送模式
    HAL_UART_Transmit(&rs485_handle, buf, len, 1000);
    rs485_rx_cnt = 0;
    RS485_RE(0); // 回到接收模式
}

/**
 * @brief       RS485查询接收到的数据
 * @param       buf     : 接收缓冲区首地址
 * @param       len     : 接收到的数据长度
 *   @arg               0   , 表示没有接收到任何数据
 *   @arg               其他, 表示接收到的数据长度
 * @retval      无
 */
void rs485_receive_data(uint8_t *buf, uint8_t *len)
{
    uint8_t rxlen = rs485_rx_cnt;
    uint8_t i = 0;
    *len = 0;     // 默认为0
    delay_ms(10); // 等待10ms,连续超过10ms没有接收到一个数据,则认为接收结束 

    if (rxlen == rs485_rx_cnt && rxlen) // 接收到了数据,且接收完成了
    {
        for (i = 0; i < rxlen; i++)
        {
            buf[i] = rs485_rx_buf[i];
        }

        *len = rs485_rx_cnt; // 记录本次数据长度
        rs485_rx_cnt = 0;   
    }
}
