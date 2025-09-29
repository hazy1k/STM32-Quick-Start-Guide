/**
 ****************************************************************************************************
 * @file        atim.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.2
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
 * V1.1 20200320
 * 1, 增加atim_timx_cplm_pwm_init函数
 * 2, 增加atim_timx_cplm_pwm_set函数
 * V1.2 20200321
 * 1, 增加atim_timx_pwmin_chy_process函数
 * 2, 增加atim_timx_pwmin_chy_init函数
 * 3, 增加atim_timx_pwmin_chy_restart等函数
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


/**
 * @brief       高级定时器TIMX 互补输出 初始化函数（使用PWM模式1）
 * @note
 *              配置高级定时器TIMX 互补输出, 一路OCy 一路OCyN, 并且可以设置死区时间
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
void atim_timx_cplm_pwm_init(uint16_t arr, uint16_t psc)
{
    uint8_t chy = ATIM_TIMX_CPLM_CHY;

    ATIM_TIMX_CPLM_CHY_GPIO_CLK_ENABLE();   /* 通道X对应IO口时钟使能 */
    ATIM_TIMX_CPLM_CHYN_GPIO_CLK_ENABLE();  /* 通道X互补通道对应IO口时钟使能 */
    ATIM_TIMX_CPLM_BKIN_GPIO_CLK_ENABLE();  /* 通道X刹车输入对应IO口时钟使能 */
    ATIM_TIMX_CPLM_CLK_ENABLE();            /* 使能定时器时钟 */

    sys_gpio_set(ATIM_TIMX_CPLM_CHY_GPIO_PORT, ATIM_TIMX_CPLM_CHY_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);    /* TIMX_CHX 引脚模式设置 */

    sys_gpio_set(ATIM_TIMX_CPLM_CHYN_GPIO_PORT, ATIM_TIMX_CPLM_CHYN_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);    /* TIMX_CHXN 引脚模式设置 */

    sys_gpio_set(ATIM_TIMX_CPLM_BKIN_GPIO_PORT, ATIM_TIMX_CPLM_BKIN_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);    /* TIMX_BKIN 引脚模式设置 */

    sys_gpio_af_set(ATIM_TIMX_CPLM_CHY_GPIO_PORT,  ATIM_TIMX_CPLM_CHY_GPIO_PIN,  ATIM_TIMX_CPLM_CHY_GPIO_AF);   /* IO口复用功能选择 必须设置对!! */
    sys_gpio_af_set(ATIM_TIMX_CPLM_CHYN_GPIO_PORT, ATIM_TIMX_CPLM_CHYN_GPIO_PIN, ATIM_TIMX_CPLM_CHYN_GPIO_AF);  /* IO口复用功能选择 必须设置对!! */
    sys_gpio_af_set(ATIM_TIMX_CPLM_BKIN_GPIO_PORT, ATIM_TIMX_CPLM_BKIN_GPIO_PIN, ATIM_TIMX_CPLM_BKIN_GPIO_AF);  /* IO口复用功能选择 必须设置对!! */

    ATIM_TIMX_CPLM->ARR = arr;          /* 设定计数器自动重装值 */
    ATIM_TIMX_CPLM->PSC = psc;          /* 设置预分频器  */

    if (chy <= 2)
    {
        ATIM_TIMX_CPLM->CCMR1 |= 6 << (4 + 8 * (chy - 1));  /* CH1/2 PWM模式1 */
        ATIM_TIMX_CPLM->CCMR1 |= 1 << (3 + 8 * (chy - 1));  /* CH1/2 预装载使能 */
    }
    else if (chy <= 4)
    {
        ATIM_TIMX_CPLM->CCMR2 |= 6 << (4 + 8 * (chy - 3));  /* CH3/4 PWM模式1 */
        ATIM_TIMX_CPLM->CCMR2 |= 1 << (3 + 8 * (chy - 3));  /* CH3/4 预装载使能 */
    }

    ATIM_TIMX_CPLM->CCER |= 1 << (4 * (chy - 1));       /* OCy 输出使能 */
    ATIM_TIMX_CPLM->CCER |= 1 << (1 + 4 * (chy - 1));   /* OCy 低电平有效 */
    ATIM_TIMX_CPLM->CCER |= 1 << (2 + 4 * (chy - 1));   /* OCyN 输出使能 */
    ATIM_TIMX_CPLM->CCER |= 1 << (3 + 4 * (chy - 1));   /* OCyN 低电平有效 */

    ATIM_TIMX_CPLM->BDTR |= 0 << 16;    /* BKF[3:0]=0,BKIN检测不滤波 */
    ATIM_TIMX_CPLM->BDTR |= 0 << 13;    /* BKP = 0, BKIN低电平有效 */
    ATIM_TIMX_CPLM->BDTR |= 1 << 12;    /* BKE = 1, 使能BKIN检测 */

    ATIM_TIMX_CPLM->CR1 |= 1 << 7;      /* ARPE使能 */
    ATIM_TIMX_CPLM->CR1 |= 2 << 8;      /* CKD[1:0] = 10, tDTS = 4 * tCK_INT = Ft / 4 = 60Mhz*/
    ATIM_TIMX_CPLM->CR1 |= 1 << 0;      /* 使能定时器TIMX */
}

/**
 * @brief       定时器TIMX 设置输出比较值 & 死区时间
 * @param       ccr: 输出比较值
 * @param       dtg: 死区时间
 *   @arg       dtg[7:5]=0xx时, 死区时间 = dtg[7:0] * tDTS
 *   @arg       dtg[7:5]=10x时, 死区时间 = (64 + dtg[6:0]) * 2  * tDTS
 *   @arg       dtg[7:5]=110时, 死区时间 = (32 + dtg[5:0]) * 8  * tDTS
 *   @arg       dtg[7:5]=111时, 死区时间 = (32 + dtg[5:0]) * 16 * tDTS
 *   @note      tDTS = 1 / (Ft /  CKD[1:0]) = 1 / 60M = 16.67ns
 * @retval      无
 */
void atim_timx_cplm_pwm_set(uint16_t ccr, uint8_t dtg)
{
    ATIM_TIMX_CPLM->BDTR &= ~(0XFF << 0);   /* DTG[7:0]清零 */
    ATIM_TIMX_CPLM->BDTR |= dtg << 0;       /* DTG[7:0] = dtg, 设置死区时间 */
    ATIM_TIMX_CPLM->BDTR |= 1 << 15;        /* MOE=1,使能主输出 */
    ATIM_TIMX_CPLM_CHY_CCRY = ccr;          /* 设置比较寄存器 */
}

/* PWM输入状态(g_timxchy_cap_sta)
 * 0,没有成功捕获.
 * 1,已经成功捕获了
 */
uint8_t g_timxchy_pwmin_sta  = 0;   /* PWM输入状态 */
uint16_t g_timxchy_pwmin_psc  = 0;  /* PWM输入分频系数 */
uint32_t g_timxchy_pwmin_hval = 0 ; /* PWM的高电平脉宽 */
uint32_t g_timxchy_pwmin_cval = 0 ; /* PWM的周期宽度 */

/**
 * @brief       定时器TIMX 通道Y PWM输入模式 中断处理函数
 * @note
 *              因为TIM1/TIM8等有多个中断服务函数,而TIM2~5/TIM12/TIM15等普通定时器只有1个中断服务
 *              函数,为了更好的兼容,我们对中断处理统一放到atim_timx_pwin_chy_process函数里面进行处理
 *
 * @param       无
 * @retval      无
 */
static void atim_timx_pwmin_chy_process(void)
{
    uint8_t chy = ATIM_TIMX_PWMIN_CHY;      /* 需要捕获的通道 */
    static uint8_t sflag = 0;               /* 启动PWMIN输入检测标志 */

    if (g_timxchy_pwmin_sta)
    {
        g_timxchy_pwmin_psc = 0;
        ATIM_TIMX_PWMIN->SR = 0;            /* 清除所有中断标记 */
        ATIM_TIMX_PWMIN->CNT = 0;
        return ;
    }

    if (ATIM_TIMX_PWMIN->SR & (1 << 0))     /* 发生了溢出中断 */
    { 
        ATIM_TIMX_PWMIN->SR &= ~(1 << 0);   /* 清除溢出中断标记 */

        if ((ATIM_TIMX_PWMIN->SR & (1 << chy)) == 0)   /* 没有发生周期捕获中断,且捕获未完成 */
        {
            sflag = 0;
            if (g_timxchy_pwmin_psc == 0)   /* 从0 到 1 */
            {
                g_timxchy_pwmin_psc ++;
            }
            else
            {
                if (g_timxchy_pwmin_psc == 65535)   /* 已经最大了,可能是无输入状态 */
                {
                    g_timxchy_pwmin_psc = 0;        /* 重新恢复不分频 */
                }
                else if (g_timxchy_pwmin_psc > 32767)     /* 不能倍增了 */
                {
                    g_timxchy_pwmin_psc = 65535;    /* 直接等于最大分频系数 */
                }
                else
                {
                    g_timxchy_pwmin_psc += g_timxchy_pwmin_psc; /* 倍增 */
                }
            }

            ATIM_TIMX_PWMIN->PSC = g_timxchy_pwmin_psc;
            ATIM_TIMX_PWMIN->CNT = 0;
            ATIM_TIMX_PWMIN->SR = 0;        /* 清除所有中断标记 */
            return ;
        }
    }

    if (sflag == 0)   /* 第一次采集到捕获中断 */
    {
        if (ATIM_TIMX_PWMIN->SR & (1 << chy))   /* 检测到了第一次周期捕获中断 */
        { 
            sflag = 1;              /* 标记第一次周期已经捕获, 第二次周期捕获可以开始了 */
        }

        ATIM_TIMX_PWMIN->SR = 0;    /* 清除所有中断标记 */
        return ;                    /* 完成此次操作 */
    }

    if (g_timxchy_pwmin_sta == 0)   /* 还没有成功捕获 */
    {
        if (ATIM_TIMX_PWMIN->SR & (1 << chy))   /* 检测到了周期捕获中断 */
        {
            if (chy == 1)
            {
                g_timxchy_pwmin_hval = ATIM_TIMX_PWMIN->CCR2 + 1;   /* 高定平脉宽捕获值 */
                g_timxchy_pwmin_cval = ATIM_TIMX_PWMIN->CCR1 + 1;   /* 周期捕获值 */
            }
            else
            {
                g_timxchy_pwmin_hval = ATIM_TIMX_PWMIN->CCR1 + 1;   /* 高定平脉宽捕获值 */
                g_timxchy_pwmin_cval = ATIM_TIMX_PWMIN->CCR2 + 1;   /* 周期捕获值 */
            }

            if (g_timxchy_pwmin_hval < g_timxchy_pwmin_cval)        /* 高电平脉宽必定小于周期长度 */
            {
                g_timxchy_pwmin_sta = 1;        /* 标记捕获成功 */

                if (g_timxchy_pwmin_psc == 0)   /* 分频系数为0的时候, 修正读取数据 */
                {
                    g_timxchy_pwmin_hval++;     /* 修正系数为1, 加1 */
                    g_timxchy_pwmin_cval++;     /* 修正系数为1, 加1 */
                }

                sflag = 0;
                /* 每次捕获PWM输入成功后, 停止捕获,避免频繁中断影响系统正常代码运行 */
                ATIM_TIMX_PWMIN->CR1  &= ~(1 << 0); /* 关闭定时器TIMX */
                ATIM_TIMX_PWMIN->SR = 0;            /* 清除所有中断标志位 */
                ATIM_TIMX_PWMIN->DIER &= ~(7 << 0); /* 关闭溢出中断/CC1IF中断/CC2IF中断 */
            }
            else
            {
                atim_timx_pwmin_chy_restart();
            }
        }
    }

    ATIM_TIMX_PWMIN->SR = 0; /* 清除所有中断标记 */
}

/**
 * @brief       定时器TIMX 更新/溢出 中断服务函数
 *   @note      TIM1/TIM8的这个函数仅用于更新/溢出中断服务,捕获在另外一个函数!
 *              其他普通定时器则更新/溢出/捕获,都在这个函数里面处理!
 * @param       无
 * @retval      无
 */
void ATIM_TIMX_PWMIN_IRQHandler(void)
{
    atim_timx_pwmin_chy_process();
}

/**
 * @brief       定时器TIMX 输入捕获 中断服务函数
 *   @note      仅TIM1/TIM8有这个函数,其他普通定时器没有这个中断服务函数!
 * @param       无
 * @retval      无
 */
void ATIM_TIMX_PWMIN_CC_IRQHandler(void)
{
    atim_timx_pwmin_chy_process();
}

/**
 * @brief       定时器TIMX 通道Y PWM输入模式 初始化函数
 * @note
 *              通用定时器的时钟来自APB1,当D2PPRE1≥2分频的时候
 *              通用定时器的时钟为APB1时钟的2倍, 而APB1为120M, 所以定时器时钟 = 240Mhz
 *              定时器溢出时间计算方法: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=定时器工作频率,单位:Mhz
 *
 *              本函数初始化的时候: 使用psc=0, arr固定为65535. 得到采样时钟频率为240Mhz,精度4.167ns
 *
 * @param       无
 * @retval      无
 */
void atim_timx_pwmin_chy_init(void)
{
    uint16_t chy = ATIM_TIMX_PWMIN_CHY;
    ATIM_TIMX_PWMIN_CHY_GPIO_CLK_ENABLE();  /* TIMX 通道IO口时钟使能 */
    ATIM_TIMX_PWMIN_CHY_CLK_ENABLE();       /* TIMX 时钟使能 */

    sys_gpio_set(ATIM_TIMX_PWMIN_CHY_GPIO_PORT, ATIM_TIMX_PWMIN_CHY_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PD);    /* TIMX PWM CHY 复用功能 下拉 */

    sys_gpio_af_set(ATIM_TIMX_PWMIN_CHY_GPIO_PORT, ATIM_TIMX_PWMIN_CHY_GPIO_PIN, ATIM_TIMX_PWMIN_CHY_GPIO_AF);  /* IO口复用功能选择 必须设置对!! */

    ATIM_TIMX_PWMIN->ARR = 65535;         /* 设定计数器自动重装值,固定为最大值 */
    ATIM_TIMX_PWMIN->PSC = 0;             /* 设置预分频器,默认设置为0  */

    if (chy == 1)
    {
        ATIM_TIMX_PWMIN->CCMR1 |= 1 << 0;   /* CC1S[1:0] = 01  选择输入端 IC1映射到TI1上 */
        ATIM_TIMX_PWMIN->CCMR1 |= 2 << 8;   /* CC2S[1:0] = 10  选择输入端 IC2映射到TI1上 */
        ATIM_TIMX_PWMIN->CCER |= 0 << 1;    /* CC1P      = 0   CC1NP默认是0, CCR1捕获上升沿 */
        ATIM_TIMX_PWMIN->CCER |= 1 << 5;    /* CC2P      = 1   CC2NP默认是0, CCR2捕获下降沿 */
        ATIM_TIMX_PWMIN->SMCR |= 5 << 4;    /* TS[4:0]   = 101 触发选择TI1FP1 */
    }
    else
    {
        ATIM_TIMX_PWMIN->CCMR1 |= 2 << 0;   /* CC1S[1:0] = 10  选择输入端 IC1映射到TI2上 */
        ATIM_TIMX_PWMIN->CCMR1 |= 1 << 8;   /* CC2S[1:0] = 01  选择输入端 IC2映射到TI2上 */
        ATIM_TIMX_PWMIN->CCER |= 1 << 1;    /* CC1P      = 1   CC1NP默认是0, CCR1捕获下降沿 */
        ATIM_TIMX_PWMIN->CCER |= 0 << 5;    /* CC2P      = 0   CC2NP默认是0, CCR2捕获上升沿 */
        ATIM_TIMX_PWMIN->SMCR |= 6 << 4;    /* TS[4:0]   = 110 触发选择TI1FP2 */
    }

    ATIM_TIMX_PWMIN->SMCR |= 4 << 0;        /* SMS[2:0]  = 100 复位模式 */
    ATIM_TIMX_PWMIN->CCER |= 1 << 0;        /* CC1E      = 1   使能输入通道1 */
    ATIM_TIMX_PWMIN->CCER |= 1 << 4;        /* CC2E      = 1   使能输入通道2 */

    ATIM_TIMX_PWMIN->DIER |= 1 << 0;        /* 使能溢出中断 */
    ATIM_TIMX_PWMIN->DIER |= 1 << chy;      /* 使能通道1/2捕获中断 */
    ATIM_TIMX_PWMIN->CR1  |= 1 << 0;        /* 使能定时器TIMX */

    sys_nvic_init(1, 3, ATIM_TIMX_PWMIN_IRQn, 2);   /* 抢占1，子优先级3，组2 */

    /* TIM1/TIM8 有独立的输入捕获中断服务函数 */
    if (ATIM_TIMX_PWMIN == TIM1 || ATIM_TIMX_PWMIN == TIM8)
    {
        sys_nvic_init(1, 3, ATIM_TIMX_PWMIN_CC_IRQn, 2);/* 抢占1，子优先级3，组2 */
    }
}

/**
 * @brief       定时器TIMX PWM输入模式 重新启动捕获
 * @param       无
 * @retval      无
 */
void atim_timx_pwmin_chy_restart(void)
{
    uint16_t chy = ATIM_TIMX_PWMIN_CHY;

    sys_intx_disable();                 /* 关闭中断 */

    g_timxchy_pwmin_sta = 0;            /* 清零状态,重新开始检测 */
    g_timxchy_pwmin_psc = 0;            /* 分频系数清零 */

    ATIM_TIMX_PWMIN->PSC = 0;           /* 以最大的计数频率采集,以得到最好的精度 */
    ATIM_TIMX_PWMIN->CNT = 0;           /* 计数器清零 */

    ATIM_TIMX_PWMIN->DIER |= 1 << chy;  /* 使能通道1/2捕获中断 */
    ATIM_TIMX_PWMIN->DIER |= 1 << 0;    /* 使能溢出中断 */
    ATIM_TIMX_PWMIN->CR1  |= 1 << 0;    /* 使能定时器TIMX */
    ATIM_TIMX_PWMIN->SR = 0;            /* 清零所有状态标志 */

    sys_intx_enable();                  /* 打开中断 */

}

















