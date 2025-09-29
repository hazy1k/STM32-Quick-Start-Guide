/**
 ****************************************************************************************************
 * @file        usart.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-03-12
 * @brief       串口初始化代码(一般是串口1)，支持printf
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
 * V1.0 20200312
 * 第一次发布
 *
 ****************************************************************************************************
 */
 
#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"

/* 如果使用os,则包括下面的头文件即可. */
#if SYS_SUPPORT_OS
#include "os.h"   /* os 使用 */
#endif



/******************************************************************************************/
/* 加入以下代码, 支持printf函数, 而不需要选择use MicroLIB */

#if 1
#if (__ARMCC_VERSION >= 6010050)            /* 使用AC6编译器时 */
__asm(".global __use_no_semihosting\n\t");  /* 声明不使用半主机模式 */
__asm(".global __ARM_use_no_argv \n\t");    /* AC6下需要声明main函数为无参数格式，否则部分例程可能出现半主机模式 */

#else
/* 使用AC5编译器时, 要在这里定义__FILE 和 不使用半主机模式 */
#pragma import(__use_no_semihosting)

struct __FILE
{
    int handle;
    /* Whatever you require here. If the only file you are using is */
    /* standard output using printf() for debugging, no file handling */
    /* is required. */
};

#endif

/* 不使用半主机模式，至少需要重定义_ttywrch\_sys_exit\_sys_command_string函数,以同时兼容AC6和AC5模式 */
int _ttywrch(int ch)
{
    ch = ch;
    return ch;
}

/* 定义_sys_exit()以避免使用半主机模式 */
void _sys_exit(int x)
{
    x = x;
}

char *_sys_command_string(char *cmd, int len)
{
    return NULL;
}

/* FILE 在 stdio.h里面定义. */
FILE __stdout;

/* 重定义fputc函数, printf函数最终会通过调用fputc输出字符串到串口 */
int fputc(int ch, FILE *f)
{
    while ((USART_UX->ISR & 0X40) == 0);    /* 等待上一个字符发送完成 */

    USART_UX->TDR = (uint8_t)ch;            /* 将要发送的字符 ch 写入到DR寄存器 */
    return ch;
}
#endif
/******************************************************************************************/


#if USART_EN_RX     /* 如果使能了接收 */

/* 接收缓冲, 最大USART_REC_LEN个字节. */
uint8_t g_usart_rx_buf[USART_REC_LEN];

/*  接收状态
 *  bit15，      接收完成标志
 *  bit14，      接收到0x0d
 *  bit13~0，    接收到的有效字节数目
*/
uint16_t g_usart_rx_sta = 0;

/**
 * @brief       串口X中断服务函数
 * @param       无
 * @retval      无
 */
void USART_UX_IRQHandler(void)
{
    uint8_t rxdata;
#if SYS_SUPPORT_OS  /* 如果SYS_SUPPORT_OS为真，则需要支持OS. */
    OSIntEnter();
#endif

    if (USART_UX->ISR & (1 << 5))               /* 接收到数据 */
    {
        rxdata = USART_UX->RDR;

        if ((g_usart_rx_sta & 0x8000) == 0)     /* 接收未完成? */
        {
            if (g_usart_rx_sta & 0x4000)        /* 接收到了0x0d? */
            {
                if (rxdata != 0x0a)             /* 接收到了0x0a? (必须先接收到到0x0d,才检查0x0a) */
                {
                    g_usart_rx_sta = 0;         /* 接收错误, 重新开始 */
                }
                else
                {
                    g_usart_rx_sta |= 0x8000;   /* 收到了0x0a,标记接收完成了 */
                }
            }
            else      /* 还没收到0x0d */
            {
                if (rxdata == 0x0d)
                {
                    g_usart_rx_sta |= 0x4000;   /* 标记接收到了 0x0d */
                }
                else
                {
                    g_usart_rx_buf[g_usart_rx_sta & 0X3FFF] = rxdata;   /* 存储数据到 g_usart_rx_buf */
                    g_usart_rx_sta++;

                    if (g_usart_rx_sta > (USART_REC_LEN - 1))g_usart_rx_sta = 0;/* 接收数据溢出, 重新开始接收 */
                }
            }
        }
    }

    USART_UX->ICR |= 1 << 3;/* 清除溢出错误, 否则可能会卡死在串口中断服务函数里面 */
#if SYS_SUPPORT_OS  /* 如果SYS_SUPPORT_OS为真，则需要支持OS. */
    OSIntExit();
#endif
}
#endif

/**
 * @brief       串口X初始化函数
 * @param       sclk: 串口X的时钟源频率(单位: MHz)
 *              串口1 和 串口6 的时钟源来自: rcc_pclk2 = 120Mhz
 *              串口2 - 5 / 7 / 8 的时钟源来自: rcc_pclk1 = 120Mhz
 * @note        注意: 必须设置正确的sclk, 否则串口波特率就会设置异常.
 * @param       baudrate: 波特率, 根据自己需要设置波特率值
 * @retval      无
 */
void usart_init(uint32_t sclk, uint32_t baudrate)
{
    uint32_t temp;
    /* IO 及 时钟配置 */
    USART_TX_GPIO_CLK_ENABLE(); /* 使能串口TX脚时钟 */
    USART_RX_GPIO_CLK_ENABLE(); /* 使能串口RX脚时钟 */
    USART_UX_CLK_ENABLE();      /* 使能串口时钟 */

    sys_gpio_set(USART_TX_GPIO_PORT, USART_TX_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);/* 串口TX脚 模式设置 */

    sys_gpio_set(USART_RX_GPIO_PORT, USART_RX_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);/* 串口RX脚 模式设置 */

    sys_gpio_af_set(GPIOA, USART_TX_GPIO_PIN, USART_TX_GPIO_AF);   /* TX脚 复用功能选择, 必须设置正确 */
    sys_gpio_af_set(GPIOA, USART_RX_GPIO_PIN, USART_RX_GPIO_AF);   /* RX脚 复用功能选择, 必须设置正确 */

    temp = (sclk * 1000000 + baudrate / 2) / baudrate;  /* 得到USARTDIV@OVER8 = 0, 采用四舍五入计算 */
    /* 波特率设置 */
    USART_UX->BRR = temp;       /* 波特率设置@OVER8 = 0 */
    USART_UX->CR1 = 0;          /* 清零CR1寄存器 */
    USART_UX->CR1 |= 0 << 28;   /* 设置M1 = 0 */
    USART_UX->CR1 |= 0 << 12;   /* 设置M0 = 0 & M1 = 0, 选择8位字长 */
    USART_UX->CR1 |= 0 << 15;   /* 设置OVER8 = 0, 16倍过采样 */
    USART_UX->CR1 |= 1 << 3;    /* 串口发送使能 */
#if USART_EN_RX  /* 如果使能了接收 */
    /* 使能接收中断 */
    USART_UX->CR1 |= 1 << 2;    /* 串口接收使能 */
    USART_UX->CR1 |= 1 << 5;    /* 接收缓冲区非空中断使能 */
    sys_nvic_init(3, 3, USART_UX_IRQn, 2); /* 组2，最低优先级 */
#endif
    USART_UX->CR1 |= 1 << 0;    /* 串口使能 */
}














