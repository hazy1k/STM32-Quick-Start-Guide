/**
 ****************************************************************************************************
 * @file        timer.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-05-30
 * @brief       定时器 驱动代码
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
 * V1.0 20220530
 * 第一次发布
 *
 ****************************************************************************************************
 */

#include "./BSP/LED/led.h"
#include "./BSP/TIMER/timer.h"
#include "os.h"


extern volatile uint8_t aviframeup;
extern uint16_t reg_time;
/**
 * @brief       TIM6中断服务函数
 * @param       无
 * @retval      无
 */
void TIM6_DAC_IRQHandler(void)
{
    OSIntEnter();

    if (TIM6->SR & 0X0001)   /* 溢出中断 */
    {
        aviframeup = 1;
        reg_time++;
    }

    TIM6->SR &= ~(1 << 0);  /* 清除中断标志位 */

    OSIntExit();
}

/**
 * @brief       定时器6 定时中断初始化函数
 * @note
 *              基本定时器的时钟来自APB1,当D2PPRE1≥2分频的时候(下同)
 *              基本定时器的时钟为APB1时钟的2倍, 而APB1为120M, 所以定时器时钟 = 240Mhz
 *              定时器溢出时间计算方法: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=定时器工作频率,单位:Mhz
 *
 * @param       arr: 自动重装值。
 * @param       psc: 时钟预分频数
 * @retval      无
 */
void tim6_int_init(uint16_t arr, uint16_t psc)
{
    RCC->APB1LENR |= 1 << 4;   /* 使能定时器6时钟 */
    __nop();    /* 延时一点点时间, H7必须要, F7及更低版本则不需要
                 * STM32H7不能在使能时钟后立即操作寄存器, 需等待时钟正常
                 */
    TIM6->ARR = arr;           /* 设定计数器自动重装值 */
    TIM6->PSC = psc;           /* 设置预分频器  */
    TIM6->DIER |= 1 << 0;      /* 允许更新中断 */
    TIM6->CR1 |= 1 << 0;       /* 使能定时器TIMX */
    sys_nvic_init(1, 3, TIM6_DAC_IRQn, 2); /* 抢占1，子优先级3，组2 */
}

extern volatile uint16_t USART2_RX_STA;
/**
 * @brief       TIM7中断服务函数
 * @param       无
 * @retval      无
 */
void TIM7_IRQHandler(void)
{
    OSIntEnter();

    if (TIM7->SR & 0X0001)   /* 溢出中断 */
    {
        USART2_RX_STA |= 1 << 15;   /* 标记接收完成 */
        TIM7->SR &= ~(1 << 0);      /* 清除中断标志位 */
        TIM7->CR1 &= ~(1 << 0);     /* 关闭定时器7 */
    }

    OSIntExit();
}

/**
 * @brief       定时器7 定时中断初始化函数
 * @param       arr: 自动重装值。
 * @param       psc: 时钟预分频数
 * @retval      无
 */
void tim7_int_init(uint16_t arr, uint16_t psc)
{
    RCC->APB1LENR |= 1 << 5;   /* 使能定时器7时钟 */
    __nop();    /* 延时一点点时间, H7必须要, F7及更低版本则不需要
                 * STM32H7不能在使能时钟后立即操作寄存器, 需等待时钟正常
                 */
    TIM7->ARR = arr;           /* 设定计数器自动重装值 */
    TIM7->PSC = psc;           /* 设置预分频器  */
    TIM7->DIER |= 1 << 0;      /* 允许更新中断 */
    TIM7->CR1 |= 1 << 0;       /* 使能定时器TIMX */
    sys_nvic_init(1, 3, TIM7_IRQn, 2); /* 抢占1，子优先级3，组2 */
}


extern void usbapp_pulling(void);

volatile uint8_t framecnt;      /* 统一的帧计数器 */
volatile uint8_t framecntout;   /* 统一的帧计数器输出变量 */
/**
 * @brief       TIM8&TIM13 中断服务函数
 * @param       无
 * @retval      无
 */
void TIM8_UP_TIM13_IRQHandler(void)
{
    OSIntEnter();

    if (TIM8->SR & 0X0001)      /* 溢出中断 */
    {
        if (OSRunning != TRUE)  /* OS还没运行,借TIM8的中断,10ms一次,来扫描USB */
        {
            usbapp_pulling();
        }
        else
        {
            framecntout = framecnt;
            printf("frame:%d\r\n", framecntout); /* 打印帧率 */
            framecnt = 0;
        }
    }

    TIM8->SR &= ~(1 << 0);  /* 清除中断标志位 */

    OSIntExit();
}

/**
 * @brief       定时器8 定时中断初始化函数
 * @param       arr: 自动重装值。
 * @param       psc: 时钟预分频数
 * @retval      无
 */
void tim8_int_init(uint16_t arr, uint16_t psc)
{
    RCC->APB2ENR |= 1 << 1;   /* 使能定时器8时钟 */
    __nop();    /* 延时一点点时间, H7必须要, F7及更低版本则不需要
                 * STM32H7不能在使能时钟后立即操作寄存器, 需等待时钟正常
                 */
    TIM8->ARR = arr;           /* 设定计数器自动重装值 */
    TIM8->PSC = psc;           /* 设置预分频器  */
    TIM8->DIER |= 1 << 0;      /* 允许更新中断 */
    TIM8->CR1 |= 1 << 0;       /* 使能定时器TIMX */
    sys_nvic_init(1, 3, TIM8_UP_TIM13_IRQn, 2); /* 抢占1，子优先级3，组2 */
}

/**
 * @brief       定时器3 通道1和2 PWM 初始化函数
 * @param       arr: 自动重装值。
 * @param       psc: 时钟预分频数
 * @retval      无
 */
void tim3_ch12_pwm_init(uint16_t arr, uint16_t psc)
{
    RCC->APB1LENR |= 1 << 1;    /* TIM3时钟使能 */
    RCC->AHB4ENR |= 1 << 1;     /* 使能PORTB时钟 */

//    sys_gpio_set(GPIOB, SYS_GPIO_PIN4,  /* TIM3_CH1 引脚模式设置 */
//                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);

    sys_gpio_set(GPIOB, SYS_GPIO_PIN5,  /* TIM3_CH2 引脚模式设置 */
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);

    sys_gpio_af_set(GPIOB, SYS_GPIO_PIN4, 2);   /* IO口复用功能选择 必须设置对!! */
    sys_gpio_af_set(GPIOB, SYS_GPIO_PIN5, 2);   /* IO口复用功能选择 必须设置对!! */

    TIM3->ARR = arr;            /* 设定计数器自动重装值 */
    TIM3->PSC = psc;            /* 预分频器不分频 */

    /* CH1配置 */
    TIM3->CCMR1 |= 6 << 4;      /* CH1 PWM1模式 */
    TIM3->CCMR1 |= 1 << 3;      /* CH1 预装载使能 */
    TIM3->CCER |= 1 << 0;       /* OC1 输出使能 */
    TIM3->CCER |= 1 << 1;       /* OC1 低电平有效 */

    /* CH2配置 */
    TIM3->CCMR1 |= 6 << 12;     /* CH2 PWM1模式 */
    TIM3->CCMR1 |= 1 << 11;     /* CH2 预装载使能 */
    TIM3->CCER |= 1 << 4;       /* OC2 输出使能 */
    TIM3->CCER |= 0 << 5;       /* OC2 高电平有效 */

    TIM3->CR1 |= 1 << 7;        /* ARPE使能 */
    TIM3->CR1 |= 1 << 0;        /* 使能定时器3 */
}

/**
 * @brief       定时器15 通道1和2 PWM 初始化函数
 * @param       arr: 自动重装值。
 * @param       psc: 时钟预分频数
 * @retval      无
 */
void tim15_ch12_pwm_init(uint16_t arr, uint16_t psc)
{
    RCC->APB2ENR |= 1 << 16;    /* TIM15时钟使能 */

    __nop();    /* 延时一点点时间, H7必须要, F7及更低版本则不需要
                 * STM32H7不能在使能时钟后立即操作寄存器, 需等待时钟正常
                 */

    /* GPIO在 其他位置设置 */
    //    sys_gpio_set(GPIOE, SYS_GPIO_PIN5,  /* TIM15_CH1 引脚模式设置 */
    //                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);
    //
    //    sys_gpio_set(GPIOE, SYS_GPIO_PIN6,  /* TIM15_CH2 引脚模式设置 */
    //                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);

    //    sys_gpio_af_set(GPIOE, SYS_GPIO_PIN5, 4);   /* IO口复用功能选择 必须设置对!! */
    //    sys_gpio_af_set(GPIOE, SYS_GPIO_PIN6, 4);   /* IO口复用功能选择 必须设置对!! */

    TIM15->ARR = arr;           /* 设定计数器自动重装值 */
    TIM15->PSC = psc;           /* 预分频器不分频 */

    /* CH1配置 */
    TIM15->CCMR1 |= 6 << 4;     /* CH1 PWM1模式 */
    TIM15->CCMR1 |= 1 << 3;     /* CH1 预装载使能 */
    TIM15->CCER |= 1 << 0;      /* OC1 输出使能 */
    TIM15->CCER |= 1 << 1;      /* OC1 低电平有效 */

    /* CH2配置 */
    TIM15->CCMR1 |= 6 << 12;    /* CH2 PWM1模式 */
    TIM15->CCMR1 |= 1 << 11;    /* CH2 预装载使能 */
    TIM15->CCER |= 1 << 4;      /* OC2 输出使能 */
    TIM15->CCER |= 1 << 5;      /* OC2 低电平有效 */

    TIM15->BDTR|=1<<15;         /* 使能MOE位 */
    TIM15->CR1 |= 1 << 7;       /* ARPE使能 */
    TIM15->CR1 |= 1 << 0;       /* 使能定时器15 */
}















