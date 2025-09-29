/**
 ****************************************************************************************************
 * @file        usart2.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.1
 * @date        2022-05-27
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
 * V1.1 20220527
 * 1, 增加中断数据接收处理
 * 2, 增加usart2_printf函数
 *
 ****************************************************************************************************
 */

#include "./BSP/USART2/usart2.h"
#include "./BSP/TIMER/btim.h"
#include "stdarg.h"
#include "stdio.h"
#include "string.h"
#include "ucos_ii.h"


/**
 * @brief       串口2初始化函数
 * @param       sclk: 串口2的时钟源频率(单位: Hz)
 *              串口2 - 5 / 7 / 8 的时钟源来自: sclk = rcc_pclk1 = 120Mhz
 * @note        注意: 必须设置正确的sclk, 否则串口波特率就会设置异常.
 * @param       baudrate: 波特率, 根据自己需要设置波特率值
 * @retval      无
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
    
    /* 使能接收中断 */
    USART2->CR1 |= 1 << 2;    /* 串口接收使能 */
    USART2->CR1 |= 1 << 5;    /* 接收缓冲区非空中断使能 */
    sys_nvic_init(1, 2, USART2_IRQn, 2); /* 组2，最低优先级 */
}



/* 串口发送缓存区 */

#if !(__ARMCC_VERSION >= 6010050)   /* 不是AC6编译器，即使用AC5编译器时 */
__align(8) uint8_t USART2_TX_BUF[USART2_MAX_SEND_LEN];      /* 发送缓冲,最大USART2_MAX_SEND_LEN字节 */
#else                               /* 使用AC6编译器时 */
__ALIGNED(8) uint8_t USART2_TX_BUF[USART2_MAX_SEND_LEN];    /* 发送缓冲,最大USART2_MAX_SEND_LEN字节 */
#endif

/* 串口接收缓存区 */
uint8_t USART2_RX_BUF[USART2_MAX_RECV_LEN];                 /* 接收缓冲,最大USART2_MAX_RECV_LEN个字节 */


/**
 * 通过判断接收连续2个字符之间的时间差不大于10ms来决定是不是一次连续的数据.
 * 如果2个字符接收间隔超过10ms,则认为不是1次连续数据.也就是超过10ms没有接收到
 * 任何数据,则表示此次接收完毕.
 * 接收到的数据状态
 * [15]:0,没有接收到数据;1,接收到了一批数据.
 * [14:0]:接收到的数据长度
 */
volatile uint16_t USART2_RX_STA = 0;

void USART2_IRQHandler(void)
{
    uint8_t res;
    OSIntEnter();

    if (USART2->ISR & (1 << 5)) /* 接收到数据 */
    {
        res = USART2->RDR;

        if ((USART2_RX_STA & (1 << 15)) == 0) /* 接收完的一批数据,还没有被处理,则不再接收其他数据 */
        {
            if (USART2_RX_STA < USART2_MAX_RECV_LEN)    /* 还可以接收数据 */
            {
                TIM7->CNT = 0;          /* 计数器清空 */

                if (USART2_RX_STA == 0) /* 使能定时器7的中断 */
                {
                    TIM7->CR1 |= 1 << 0;    /* 使能定时器7 */
                }

                USART2_RX_BUF[USART2_RX_STA++] = res;   /* 记录接收到的值 */
            }
            else
            {
                USART2_RX_STA |= 1 << 15;   /* 强制标记接收完成 */
            }
        }
    }

    USART2->ICR |= 1 << 3;  /* 清除溢出错误,否则可能会卡死在串口中断服务函数里面 */
    OSIntExit();
}

/**
 * @brief       串口2,printf 函数
 *  @note       确保一次发送数据不超过USART2_MAX_SEND_LEN字节
 * @param       fmt  : 格式化
 *              ...  : 参数
 * @retval      无
 */
void usart2_printf(char *fmt, ...)
{
    uint16_t i, j;
    va_list ap;
    va_start(ap, fmt);
    vsprintf((char *)USART2_TX_BUF, fmt, ap);
    va_end(ap);
    i = strlen((const char *)USART2_TX_BUF);    /* 此次发送数据的长度 */

    for (j = 0; j < i; j++)     /* 循环发送数据 */
    {
        while ((USART2->ISR & 0X40) == 0);      /* 循环发送,直到发送完毕 */

        USART2->TDR = USART2_TX_BUF[j];
    }
}




