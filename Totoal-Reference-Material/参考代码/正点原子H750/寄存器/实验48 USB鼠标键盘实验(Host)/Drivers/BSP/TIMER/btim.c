/**
 ****************************************************************************************************
 * @file        btim.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-03-14
 * @brief       基本定时器 驱动代码
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
 * V1.0 20200314
 * 第一次发布
 *
 ****************************************************************************************************
 */

#include "./BSP/LED/led.h"
#include "./BSP/TIMER/btim.h"
#include "./SYSTEM/usart/usart.h"


extern uint8_t g_ov_frame;  /* 在dcmi.c里面定义 */

/**
 * @brief       基本定时器TIMX中断服务函数
 * @param       无
 * @retval      无
 */
void BTIM_TIMX_INT_IRQHandler(void)
{
    if (BTIM_TIMX_INT->SR & 0X0001)   /* 溢出中断 */
    {
        printf("frame:%d\r\n", g_ov_frame);
        g_ov_frame = 0;
    }

    BTIM_TIMX_INT->SR &= ~(1 << 0); /* 清除中断标志位 */
}

/**
 * @brief       基本定时器TIMX定时中断初始化函数
 * @note
 *              基本定时器的时钟来自APB1,当D2PPRE1≥2分频的时候
 *              基本定时器的时钟为APB1时钟的2倍, 而APB1为120M, 所以定时器时钟 = 240Mhz
 *              定时器溢出时间计算方法: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=定时器工作频率,单位:Mhz
 *
 * @param       arr: 自动重装值。
 * @param       psc: 时钟预分频数
 * @retval      无
 */
void btim_timx_int_init(uint16_t arr, uint16_t psc)
{
    BTIM_TIMX_INT_CLK_ENABLE();
    __nop();    /* 延时一点点时间, H7必须要, F7及更低版本则不需要
                 * STM32H7不能在使能时钟后立即操作寄存器, 需等待时钟正常
                 */
    BTIM_TIMX_INT->ARR = arr;           /* 设定计数器自动重装值 */
    BTIM_TIMX_INT->PSC = psc;           /* 设置预分频器  */
    BTIM_TIMX_INT->DIER |= 1 << 0;      /* 允许更新中断 */
    BTIM_TIMX_INT->CR1 |= 1 << 0;       /* 使能定时器TIMX */
    sys_nvic_init(1, 3, BTIM_TIMX_INT_IRQn, 2); /* 抢占1，子优先级3，组2 */
}










