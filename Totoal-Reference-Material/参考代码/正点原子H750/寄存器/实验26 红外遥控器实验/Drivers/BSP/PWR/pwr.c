/**
 ****************************************************************************************************
 * @file        pwr.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.3
 * @date        2020-03-26
 * @brief       低功耗模式 驱动代码
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
 * V1.0 20200325
 * 第一次发布
 * V1.1 20200325
 * 支持进入睡眠模式及唤醒
 * 新增pwr_wkup_key_init和pwr_enter_sleep函数
 * V1.2 20200325
 * 支持进入停止模式及唤醒
 * 新增pwr_enter_stop函数
 * V1.3 20200326
 * 支持进入待机模式及唤醒
 * 新增pwr_enter_standby函数
 *
 ****************************************************************************************************
 */

#include "./BSP/PWR/pwr.h"
#include "./BSP/LED/led.h"
#include "./BSP/LCD/lcd.h"


/**
 * @brief       初始化PVD电压监视器
 * @param       pls: 电压等级
 *   @arg       000,1.95V;  001,2.1V
 *   @arg       010,2.25V;  011,2.4V;
 *   @arg       100,2.55V;  101,2.7V;
 *   @arg       110,2.85V;  111,使用PVD_IN脚上的电压(与Vrefint比较)
 * @retval      无
 */
void pwr_pvd_init(uint8_t pls)
{
    PWR->CR1 &= ~(3 << 5);      /* PLS[2:0]清零 */
    PWR->CR1 |=  pls << 5;      /* PLS[2:0] = pls,注意不要超范围! */
    PWR->CR1 |= 1 << 4;         /* PVDE = 1,使能PVD检测 */

    EXTI_D1->IMR1 |= 1 << 16;   /* 开启line16 上的中断(PVD & AVD) */
    EXTI->FTSR1 |= 1 << 16;     /* line16 上事件下降沿触发 */
    EXTI->RTSR1 |= 1 << 16;     /* line16 上事件上升降沿触发 */

    sys_nvic_init(3, 3, PVD_AVD_IRQn, 2); /* 组2，最低优先级 */
}

/**
 * @brief       PVD/AVD中断服务函数
 * @param       无
 * @retval      无
 */
void PVD_AVD_IRQHandler(void)
{
    if (PWR->CSR1 & (1 << 4))   /* 电压比PLS所选电压还低 */
    {
        lcd_show_string(30, 130, 200, 16, 16, "PVD Low Voltage!", RED); /* LCD显示电压低 */
        LED1(0);                                                        /* 点亮绿灯, 表明电压低了 */
    }
    else
    {
        lcd_show_string(30, 130, 200, 16, 16, "PVD Voltage OK! ", BLUE);/* LCD显示电压正常 */
        LED1(1);                                                        /* 灭掉绿灯 */
    }

    EXTI->PR1 |= 1 << 16;   /* 清除line16的中断标志 */
}

/**
 * @brief       WK_UP按键 外部中断服务程序
 * @param       无
 * @retval      无
 */
void PWR_WKUP_INT_IRQHandler(void)
{
    EXTI_D1->PR1 = PWR_WKUP_GPIO_PIN;   /* 清除WKUP所在中断线 的中断标志位 */
}

/**
 * @brief       低功耗模式下的按键初始化(用于唤醒睡眠模式/停止模式)
 * @param       无
 * @retval      无
 */
void pwr_wkup_key_init(void)
{
    PWR_WKUP_GPIO_CLK_ENABLE();     /* WKUP时钟使能 */

    sys_gpio_set(PWR_WKUP_GPIO_PORT, PWR_WKUP_GPIO_PIN,
                 SYS_GPIO_MODE_IN, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PD);    /* WKUP引脚模式设置,下拉输入 */

    sys_nvic_ex_config(PWR_WKUP_GPIO_PORT, PWR_WKUP_GPIO_PIN, SYS_GPIO_RTIR);   /* WKUP配置为上升沿触发中断 */ 
    sys_nvic_init( 2, 2, PWR_WKUP_INT_IRQn, 2); /* 抢占2，子优先级2，组2 */
}

/**
 * @brief       进入CPU睡眠模式
 * @param       无
 * @retval      无
 */
void pwr_enter_sleep(void)
{
    EXTI_D1->PR1 = PWR_WKUP_GPIO_PIN;   /* 清除WKUP上的中断标志位 */ 
    sys_wfi_set();  /* 执行WFI指令, 进入待机模式 */
}

/**
 * @brief       进入停止模式
 * @param       无
 * @retval      无
 */
void pwr_enter_stop(void)
{
    EXTI_D1->PR1 = PWR_WKUP_GPIO_PIN;   /* 清除WKUP上的中断标志位 */

    /* 关闭VOS0
     * 480M版本H750芯片（V版本）新增Scale0设置,在进入停止/待机模式之前,必须先退出Scale0模式！！ 
     */
    sys_clock_set(200, 2, 2, 4);/* 设置时钟,400Mhz，降频 */
    RCC->APB4ENR |= 1 << 1;     /* 使能SYSCFGEN位 */
    SYSCFG->PWRCR &= ~(1 << 0); /* 设置ODEN位为0,关闭Overdrive,此时VCORE=1.2V */
    
    while ((PWR->D3CR & (1 << 13)) == 0);   /* 等待电压稳定 */

    PWR->CR1 |= 1 << 0;         /* 当SVOS3进入停止模式时, 稳压器处于低功耗模式. */
    
    /* 确认所有指令都已完成 */
    __DSB ();
    __ISB ();

    SCB->SCR |= 1 << 2;         /* 使能SLEEPDEEP位 */
    PWR->CPUCR &= ~(7 << 0);    /* PDDS_D1/D2/D3 = 0, 保持D1/D2/D3进入深度睡眠后,进入停止模式(PDDS=0) */

    sys_wfi_set();              /* 执行WFI指令, 进入停止模式 */
    
    SCB->SCR &= ~(1 << 2);      /* 关闭SLEEPDEEP位 */
}

/**
 * @brief       进入待机模式
 * @param       无
 * @retval      无
 */
void pwr_enter_standby(void)
{
    uint32_t tempreg;   /* 零时存储寄存器值用 */

    EXTI_D1->PR1 = PWR_WKUP_GPIO_PIN;   /* 清除WKUP上的中断标志位 */

    /* STM32F4/F7/H7,当开启了RTC相关中断后,必须先关闭RTC中断,再清中断标志位,然后重新设置
     * RTC中断,再进入待机模式才可以正常唤醒,否则会有问题.
     */
    PWR->CR1 |= 1 << 8;         /* 后备区域写使能 */
    /* 关闭RTC寄存器写保护 */
    RTC->WPR = 0xCA;
    RTC->WPR = 0x53;
    tempreg = RTC->CR & (0X0F << 12);   /* 记录原来的RTC中断设置 */
    RTC->CR &= ~(0XF << 12);    /* 关闭RTC所有中断 */
    RTC->ISR &= ~(0X3F << 8);   /* 清除所有RTC中断标志. */
    RTC->CR |= tempreg;         /* 重新设置RTC中断 */
    RTC->WPR = 0xFF;            /* 使能RTC寄存器写保护 */
    
    /* 关闭VOS0
     * 480M版本H750芯片（V版本）新增Scale0设置,在进入睡眠模式之前,必须先退出Scale0模式！！ 
     */
    sys_clock_set(200, 2, 2, 4);    /* 设置时钟,400Mhz，降频 */
    RCC->APB4ENR |= 1 << 1;         /* 使能SYSCFGEN位 */
    SYSCFG->PWRCR &= ~(1 << 0);     /* 设置ODEN位为0,关闭Overdrive,此时VCORE=1.2V */
    while ((PWR->D3CR & (1 << 13)) == 0);   /* 等待电压稳定 */
    
    sys_standby();  /* 进入待机模式 */
}










