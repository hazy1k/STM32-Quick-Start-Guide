/**
 ****************************************************************************************************
 * @file        atim.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-03-19
 * @brief       高级定时器 驱动代码
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
 * V1.0 20200319
 * 第一次发布
 *
 ****************************************************************************************************
 */

#include "./BSP/TIMER/atim.h"
#include "./BSP/LED/led.h"


/**
 * @brief       高级定时器TIMX 输出比较模式 初始化函数（使用输出比较模式）
 * @note
 *              配置高级定时器TIMX 4路输出比较模式PWM输出,实现50%占空比,不同相位控制
 *              注意,本例程输出比较模式,每2个计数周期才能完成一个PWM输出,因此输出频率减半
 *              另外,我们还可以开启中断在中断里面修改CCRx,从而实现不同频率/不同相位的控制
 *              但是我们不推荐这么使用,因为这可能导致非常频繁的中断,从而占用大量CPU资源
 *
 *              高级定时器的时钟来自APB1,当D2PPRE2≥2分频的时候
 *              高级定时器的时钟为APB2时钟的2倍, 而APB2为120M, 所以定时器时钟 = 240Mhz
 *              定时器溢出时间计算方法: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=定时器工作频率,单位:Mhz
 *
 * @param       arr: 自动重装值。
 * @param       psc: 时钟预分频数
 * @retval      无
 */
void atim_timx_comp_pwm_init(uint16_t arr, uint16_t psc)
{
    ATIM_TIMX_COMP_CH1_GPIO_CLK_ENABLE();   /* 通道1对应IO口时钟使能 */
    ATIM_TIMX_COMP_CH2_GPIO_CLK_ENABLE();   /* 通道2对应IO口时钟使能 */
    ATIM_TIMX_COMP_CH3_GPIO_CLK_ENABLE();   /* 通道3对应IO口时钟使能 */
    ATIM_TIMX_COMP_CH4_GPIO_CLK_ENABLE();   /* 通道4对应IO口时钟使能 */
    ATIM_TIMX_COMP_CLK_ENABLE();            /* 使能定时器时钟 */
    
    sys_gpio_set(ATIM_TIMX_COMP_CH1_GPIO_PORT, ATIM_TIMX_COMP_CH1_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);    /* TIMX_CH1 引脚模式设置 */
    
    sys_gpio_set(ATIM_TIMX_COMP_CH2_GPIO_PORT, ATIM_TIMX_COMP_CH2_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);    /* TIMX_CH2 引脚模式设置 */
    
    sys_gpio_set(ATIM_TIMX_COMP_CH3_GPIO_PORT, ATIM_TIMX_COMP_CH3_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);    /* TIMX_CH3 引脚模式设置 */
    
    sys_gpio_set(ATIM_TIMX_COMP_CH4_GPIO_PORT, ATIM_TIMX_COMP_CH4_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);    /* TIMX_CH4 引脚模式设置 */
    
    sys_gpio_af_set(ATIM_TIMX_COMP_CH1_GPIO_PORT, ATIM_TIMX_COMP_CH1_GPIO_PIN, ATIM_TIMX_COMP_CH1_GPIO_AF); /* IO口复用功能选择 必须设置对!! */
    sys_gpio_af_set(ATIM_TIMX_COMP_CH2_GPIO_PORT, ATIM_TIMX_COMP_CH2_GPIO_PIN, ATIM_TIMX_COMP_CH2_GPIO_AF); /* IO口复用功能选择 必须设置对!! */
    sys_gpio_af_set(ATIM_TIMX_COMP_CH3_GPIO_PORT, ATIM_TIMX_COMP_CH3_GPIO_PIN, ATIM_TIMX_COMP_CH3_GPIO_AF); /* IO口复用功能选择 必须设置对!! */
    sys_gpio_af_set(ATIM_TIMX_COMP_CH4_GPIO_PORT, ATIM_TIMX_COMP_CH4_GPIO_PIN, ATIM_TIMX_COMP_CH4_GPIO_AF); /* IO口复用功能选择 必须设置对!! */

    ATIM_TIMX_COMP->ARR = arr;          /* 设定计数器自动重装值 */
    ATIM_TIMX_COMP->PSC = psc;          /* 设置预分频器  */
    ATIM_TIMX_COMP->BDTR |= 1 << 15;    /* 使能MOE位(高级定时器,必须设置MOE才能输出PWM), 通用定时器TIM15/16/17也有,其他定时器
                                         * 则没有这个寄存器是无效的, 所以设置/不设置并不影响结果, 为了兼容这里统一改成设置MOE位
                                         */
    ATIM_TIMX_COMP->CCMR1 |= 3 << 4;    /* 通道1 输出比较模式 匹配时翻转 */
    ATIM_TIMX_COMP->CCMR1 |= 1 << 3;    /* 通道1 预装载使能 */

    ATIM_TIMX_COMP->CCMR1 |= 3 << 12;   /* 通道2 输出比较模式 匹配时翻转 */
    ATIM_TIMX_COMP->CCMR1 |= 1 << 11;   /* 通道2 预装载使能 */

    ATIM_TIMX_COMP->CCMR2 |= 3 << 4;    /* 通道3 输出比较模式 匹配时翻转 */
    ATIM_TIMX_COMP->CCMR2 |= 1 << 3;    /* 通道3 预装载使能 */

    ATIM_TIMX_COMP->CCMR2 |= 3 << 12;   /* 通道4 输出比较模式 匹配时翻转 */
    ATIM_TIMX_COMP->CCMR2 |= 1 << 11;   /* 通道4 预装载使能 */

    ATIM_TIMX_COMP->CCER |= 1 << 0;     /* OC1 输出使能 */ 
    ATIM_TIMX_COMP->CCER |= 1 << 4;     /* OC2 输出使能 */ 
    ATIM_TIMX_COMP->CCER |= 1 << 8;     /* OC3 输出使能 */ 
    ATIM_TIMX_COMP->CCER |= 1 << 12;    /* OC4 输出使能 */ 

    ATIM_TIMX_COMP->CR1 |= 1 << 7;      /* ARPE使能 */ 
    ATIM_TIMX_COMP->CR1 |= 1 << 0;      /* 使能定时器TIMX */ 
}
 



















