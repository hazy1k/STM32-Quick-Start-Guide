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


/**
 * @brief       串口2初始化函数
 * @param       sclk: 串口2的时钟源频率(单位: Hz)
 *              串口2 - 5 / 7 / 8 的时钟源来自: sclk = rcc_pclk1 = 120Mhz
 * @note        注意: 必须设置正确的sclk, 否则串口波特率就会设置异常.
 * @param       baudrate: 波特率, 根据自己需要设置波特率值
 */
void usart2_init(uint32_t sclk, uint32_t baudrate)
{
    uint32_t temp = 0;

    USART2_TX_GPIO_CLK_ENABLE();    /* 使能串口TX脚时钟 */
    USART2_RX_GPIO_CLK_ENABLE();    /* 使能串口RX脚时钟 */
    RCC->APB1LENR |= 1 << 17;       /* 使能串口2时钟 */

    sys_gpio_set(USART2_TX_GPIO_PORT, USART2_TX_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);/* 串口TX脚 模式设置 */

    sys_gpio_set(USART2_RX_GPIO_PORT, USART2_RX_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);/* 串口RX脚 模式设置 */

    sys_gpio_af_set(USART2_TX_GPIO_PORT, USART2_TX_GPIO_PIN, USART2_TX_GPIO_AF);    /* TX脚 复用功能选择, 必须设置正确 */
    sys_gpio_af_set(USART2_RX_GPIO_PORT, USART2_RX_GPIO_PIN, USART2_RX_GPIO_AF);    /* RX脚 复用功能选择, 必须设置正确 */

    temp = (sclk * 1000000 + baudrate / 2) / baudrate;  /* 得到USARTDIV@OVER8=0,采用四舍五入计算 */

    /* 波特率设置 */
    USART2->BRR = temp;         /*  波特率设置 */
    USART2->CR1 = 0;            /* 清零CR1寄存器 */
    USART2->CR1 |= 1 << 3;      /* 串口发送使能 */
    USART2->CR1 |= 1 << 13;     /* 串口使能 */
    USART2->CR1 |= 1 << 0;      /* 串口使能 */
}






