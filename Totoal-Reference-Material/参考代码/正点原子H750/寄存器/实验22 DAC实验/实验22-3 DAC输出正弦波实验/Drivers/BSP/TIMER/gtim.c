/**
 ****************************************************************************************************
 * @file        gtim.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.2
 * @date        2020-03-14
 * @brief       通用定时器 驱动代码
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
 * V1.1 20200317
 * 新增gtim_timx_pwm_chy_init函数
 * V1.2 20200318
 * 1,新增gtim_timx_cap_chy_init函数
 * V1.3 20200329
 * 1,支持外部脉冲计数功能
 * 2,新增gtim_timx_cnt_chy_init,gtim_timx_cnt_chy_get_count和gtim_timx_cnt_chy_restart三个函数
 * V1.4 20200329
 * 1,新增gtim_timx_npwm_chy_init函数
 * 2,新增gtim_timx_npwm_chy_set函数
 *
 ****************************************************************************************************
 */

#include "./BSP/TIMER/gtim.h"
#include "./BSP/LED/led.h"

/**
 * @brief       通用定时器TIMX中断服务函数
 * @param       无
 * @retval      无
 */
void GTIM_TIMX_INT_IRQHandler(void)
{
    if (GTIM_TIMX_INT->SR & 0X0001)   /* 溢出中断 */
    {
        LED1_TOGGLE();
    }

    GTIM_TIMX_INT->SR &= ~(1 << 0); /* 清除中断标志位 */
}

/**
 * @brief       通用定时器TIMX定时中断初始化函数
 * @note
 *              通用定时器的时钟来自APB1,当D2PPRE1≥2分频的时候
 *              通用定时器的时钟为APB1时钟的2倍, 而APB1为120M, 所以定时器时钟 = 240Mhz
 *              定时器溢出时间计算方法: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=定时器工作频率,单位:Mhz
 *
 * @param       arr: 自动重装值。
 * @param       psc: 时钟预分频数
 * @retval      无
 */
void gtim_timx_int_init(uint16_t arr, uint16_t psc)
{
    GTIM_TIMX_INT_CLK_ENABLE();
    __nop();    /* 延时一点点时间, H7必须要, F7及更低版本则不需要
                 * STM32H7不能在使能时钟后立即操作寄存器, 需等待时钟正常
                 */
    GTIM_TIMX_INT->ARR = arr;           /* 设定计数器自动重装值 */
    GTIM_TIMX_INT->PSC = psc;           /* 设置预分频器  */
    GTIM_TIMX_INT->DIER |= 1 << 0;      /* 允许更新中断 */
    GTIM_TIMX_INT->CR1 |= 1 << 0;       /* 使能定时器TIMX */
    sys_nvic_init(1, 3, GTIM_TIMX_INT_IRQn, 2); /* 抢占1，子优先级3，组2 */
}

/**
 * @brief       通用定时器TIMX 通道Y PWM输出 初始化函数（使用PWM模式1）
 * @note
 *              通用定时器的时钟来自APB1,当D2PPRE1≥2分频的时候
 *              通用定时器的时钟为APB1时钟的2倍, 而APB1为120M, 所以定时器时钟 = 240Mhz
 *              定时器溢出时间计算方法: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=定时器工作频率,单位:Mhz
 *
 * @param       arr: 自动重装值。
 * @param       psc: 时钟预分频数
 * @retval      无
 */
void gtim_timx_pwm_chy_init(uint16_t arr, uint16_t psc)
{
    uint8_t chy = GTIM_TIMX_PWM_CHY;
    GTIM_TIMX_PWM_CHY_GPIO_CLK_ENABLE();    /* TIMX 通道 IO口时钟使能 */
    GTIM_TIMX_PWM_CHY_CLK_ENABLE();         /* TIMX 时钟使能 */

    sys_gpio_set(GTIM_TIMX_PWM_CHY_GPIO_PORT, GTIM_TIMX_PWM_CHY_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);    /* TIMX PWM CHY 引脚模式设置 */

    sys_gpio_af_set(GTIM_TIMX_PWM_CHY_GPIO_PORT, GTIM_TIMX_PWM_CHY_GPIO_PIN, GTIM_TIMX_PWM_CHY_GPIO_AF);/* IO口复用功能选择 必须设置对!! */

    GTIM_TIMX_PWM->ARR = arr;       /* 设定计数器自动重装值 */
    GTIM_TIMX_PWM->PSC = psc;       /* 设置预分频器  */
    GTIM_TIMX_PWM->BDTR |= 1 << 15; /* 使能MOE位(仅TIM15/16/17 有此寄存器,必须设置MOE才能输出PWM), 其他通用定时器,
                                     * 这个寄存器是无效的, 所以设置/不设置并不影响结果, 为了兼容这里统一改成设置MOE位
                                     */

    if (chy <= 2)
    {
        GTIM_TIMX_PWM->CCMR1 |= 6 << (4 + 8 * (chy - 1));   /* CH1/2 PWM模式1 */
        GTIM_TIMX_PWM->CCMR1 |= 1 << (3 + 8 * (chy - 1));   /* CH1/2 预装载使能 */
    }
    else if (chy <= 4)
    {
        GTIM_TIMX_PWM->CCMR2 |= 6 << (4 + 8 * (chy - 3));   /* CH3/4 PWM模式1 */
        GTIM_TIMX_PWM->CCMR2 |= 1 << (3 + 8 * (chy - 3));   /* CH3/4 预装载使能 */
    }

    GTIM_TIMX_PWM->CCER |= 1 << (4 * (chy - 1));        /* OCy 输出使能 */
    GTIM_TIMX_PWM->CCER |= 1 << (1 + 4 * (chy - 1));    /* OCy 低电平有效 */
    GTIM_TIMX_PWM->CR1 |= 1 << 7;   /* ARPE使能 */
    GTIM_TIMX_PWM->CR1 |= 1 << 0;   /* 使能定时器TIMX */
}

/* 输入捕获状态(g_timxchy_cap_sta)
 * [7]  :0,没有成功的捕获;1,成功捕获到一次.
 * [6]  :0,还没捕获到高电平;1,已经捕获到高电平了.
 * [5:0]:捕获高电平后溢出的次数,最多溢出63次,所以最长捕获值 = 63*65536 + 65535 = 4194303
 *       注意:为了通用,我们默认ARR和CCRy都是16位寄存器,对于32位的定时器(如:TIM5),也只按16位使用
 *       按1us的计数频率,最长溢出时间为:4194303 us, 约4.19秒
 */
uint8_t g_timxchy_cap_sta = 0;  /* 输入捕获状态 */
uint32_t g_timxchy_cap_val =0 ; /* 输入捕获值 */

/**
 * @brief       通用定时器TIMX 输入捕获 中断服务函数
 * @param       无
 * @retval      无
 */
void GTIM_TIMX_CAP_IRQHandler(void)
{
    uint16_t tsr = GTIM_TIMX_CAP->SR;       /* 获取中断状态 */
    uint8_t chy = GTIM_TIMX_CAP_CHY;        /* 需要捕获的通道 */

    if ((g_timxchy_cap_sta & 0X80) == 0)    /* 还未成功捕获 */
    {
        if (tsr & (1 << 0)) /* 溢出中断 */
        {
            if (g_timxchy_cap_sta & 0X40)   /* 已经捕获到高电平了 */
            {
                if ((g_timxchy_cap_sta & 0X3F) == 0X3F) /* 高电平太长了 */
                {
                    g_timxchy_cap_sta |= 0X80;  /* 标记成功捕获了一次 */
                    g_timxchy_cap_val = 0XFFFF;
                }
                else    /* 还可以累加高电平长度 */
                {
                    g_timxchy_cap_sta++;   
                }
            }
        }

        if (tsr & (1 << chy))   /* 通道y 发生了捕获事件 */
        {
            if (g_timxchy_cap_sta & 0X40)   /* 捕获到一个下降沿 */
            {
                g_timxchy_cap_sta |= 0X80;  /* 标记成功捕获到一次高电平脉宽 */
                g_timxchy_cap_val = GTIM_TIMX_CAP_CHY_CCRX; /* 获取当前的捕获值. */
                
                GTIM_TIMX_CAP->CCER &= ~(1 << (1 + 4 * (chy - 1)));/* CCyP = 0 设置为上升沿捕获 */ 
            }
            else    /* 还未开始,第一次捕获上升沿 */
            {
                g_timxchy_cap_val = 0;
                g_timxchy_cap_sta = 0X40;   /* 标记捕获到了上升沿 */
                GTIM_TIMX_CAP->CNT = 0;     /* 计数器清空 */ 
                GTIM_TIMX_CAP->CCER |= 1 << (1 + 4 * (chy - 1));    /* CCyP = 1 设置为下降沿捕获 */ 
            }
        }
    }

    GTIM_TIMX_CAP->SR = 0;     /* 清除所有中断标志位 */
}

/**
 * @brief       通用定时器TIMX 通道Y 输入捕获 初始化函数
 * @note
 *              通用定时器的时钟来自APB1,当D2PPRE1≥2分频的时候
 *              通用定时器的时钟为APB1时钟的2倍, 而APB1为120M, 所以定时器时钟 = 240Mhz
 *              定时器溢出时间计算方法: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=定时器工作频率,单位:Mhz
 *
 * @param       arr: 自动重装值
 * @param       psc: 时钟预分频数
 * @retval      无
 */
void gtim_timx_cap_chy_init(uint16_t arr, uint16_t psc)
{
    uint8_t chy = GTIM_TIMX_CAP_CHY;
    GTIM_TIMX_CAP_CHY_GPIO_CLK_ENABLE();   /* TIMX 通道IO口时钟使能 */
    GTIM_TIMX_CAP_CHY_CLK_ENABLE();        /* TIMX 时钟使能 */

    sys_gpio_set(GTIM_TIMX_CAP_CHY_GPIO_PORT, GTIM_TIMX_CAP_CHY_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PD);    /* TIMX PWM CHY 复用功能 下拉 */

    sys_gpio_af_set(GTIM_TIMX_CAP_CHY_GPIO_PORT, GTIM_TIMX_CAP_CHY_GPIO_PIN, GTIM_TIMX_CAP_CHY_GPIO_AF); /* IO口复用功能选择 必须设置对!! */

    GTIM_TIMX_CAP->ARR = arr;       /* 设定计数器自动重装值 */
    GTIM_TIMX_CAP->PSC = psc;       /* 设置预分频器  */

    if (chy <= 2)
    {
        GTIM_TIMX_CAP->CCMR1 |= 1 << 8 * (chy - 1);        /* CCyS[1:0]   = 01 选择输入端 IC1/2映射到TI1/2上 */
        GTIM_TIMX_CAP->CCMR1 |= 0 << (2 + 8 * (chy - 1));  /* ICyPSC[1:0] = 00 输入捕获不分频,全捕获 */
        GTIM_TIMX_CAP->CCMR1 |= 0 << (4 + 8 * (chy - 1));  /* ICyF[3:0]   = 00 输入端滤波 不滤波 */
    }
    else if (chy <= 4)
    {
        GTIM_TIMX_CAP->CCMR2 |= 1 << 8 * (chy - 3);        /* CCyS[1:0]   = 01 选择输入端 IC3/4映射到TI3/4上 */
        GTIM_TIMX_CAP->CCMR2 |= 0 << (2 + 8 * (chy - 3));  /* ICyPSC[1:0] = 00 输入捕获不分频,全捕获 */
        GTIM_TIMX_CAP->CCMR2 |= 0 << (4 + 8 * (chy - 3));  /* ICyF[3:0]   = 00 输入端滤波 不滤波 */
    }

    GTIM_TIMX_CAP->CCER |= 1 << (4 * (chy - 1));       /* CCyE = 1 输入捕获使能 */
    GTIM_TIMX_CAP->CCER |= 0 << (1 + 4 * (chy - 1));   /* CCyP = 0 捕获上升沿 ,注意:CCyNP使用默认值0 */

    GTIM_TIMX_CAP->EGR  |= 1 << 0;  /* 软件控制产生更新事件,使写入PSC的值立即生效,否则将会要等到定时器溢出才会生效 */
    GTIM_TIMX_CAP->DIER |= 1 << 1;  /* 允许捕获中断 */
    GTIM_TIMX_CAP->DIER |= 1 << 0;  /* 允许更新中断 */
    GTIM_TIMX_CAP->CR1  |= 1 << 0;  /* 使能定时器TIMX */

    sys_nvic_init(1, 3, GTIM_TIMX_CAP_IRQn, 2);/* 抢占1，子优先级3，组2 */
}

/* 记录定时器计数器的溢出次数, 方便计算总脉冲个数 */
uint32_t g_timxchy_cnt_ofcnt = 0 ;  /* 计数溢出次数 */

/**
 * @brief       通用定时器TIMX 脉冲计数 更新中断服务函数
 * @param       无
 * @retval      无
 */
void GTIM_TIMX_CNT_IRQHandler(void)
{ 
    if (GTIM_TIMX_CNT->SR & (1 << 0))   /* 溢出中断 */
    {
        g_timxchy_cnt_ofcnt++;          /* 累计溢出次数 */
    } 
    GTIM_TIMX_CNT->SR = 0;              /* 清除所有中断标志位 */
}

/**
 * @brief       通用定时器TIMX 通道Y 脉冲计数 初始化函数
 * @note
 *              本函数选择通用定时器的时钟选择: 外部时钟源模式1(SMS[2:0] = 111)
 *              这样CNT的计数时钟源就来自 TIMX_CH1/CH2, 可以实现外部脉冲计数(脉冲接入CH1/CH2)
 *
 *              时钟分频数 = psc, 一般设置为0, 表示每一个时钟都会计数一次, 以提高精度.
 *              通过读取CNT和溢出次数, 经过简单计算, 可以得到当前的计数值, 从而实现脉冲计数
 *
 * @param       arr: 自动重装值 
 * @retval      无
 */
void gtim_timx_cnt_chy_init(uint16_t psc)
{
    uint8_t chy = GTIM_TIMX_CNT_CHY;
    GTIM_TIMX_CNT_CHY_GPIO_CLK_ENABLE();   /* TIMX 通道IO口时钟使能 */
    GTIM_TIMX_CNT_CHY_CLK_ENABLE();        /* TIMX 时钟使能 */

    sys_gpio_set(GTIM_TIMX_CNT_CHY_GPIO_PORT, GTIM_TIMX_CNT_CHY_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PD);    /* TIMX PWM CHY 复用功能 下拉 */

    sys_gpio_af_set(GTIM_TIMX_CNT_CHY_GPIO_PORT, GTIM_TIMX_CNT_CHY_GPIO_PIN, GTIM_TIMX_CNT_CHY_GPIO_AF); /* IO口复用功能选择 必须设置对!! */

    GTIM_TIMX_CNT->ARR = 65535; /* 设定计数器自动重装值为最大 */
    GTIM_TIMX_CNT->PSC = psc;   /* 设置预分频器  */

    GTIM_TIMX_CNT->CCMR1 |= 1 << 8 * (chy - 1);        /* CCyS[1:0]   = 01 选择输入端 IC1/2映射到TI1/2上 */
    GTIM_TIMX_CNT->CCMR1 |= 0 << (2 + 8 * (chy - 1));  /* ICyPSC[1:0] = 00 输入捕获不分频,全捕获 */
    GTIM_TIMX_CNT->CCMR1 |= 0 << (4 + 8 * (chy - 1));  /* ICyF[3:0]   = 00 输入端滤波 不滤波 */
    
    GTIM_TIMX_CNT->CCER |= 1 << (4 * (chy - 1));       /* CCyE = 1 输入捕获使能 */
    GTIM_TIMX_CNT->CCER |= 0 << (1 + 4 * (chy - 1));   /* CCyP = 0 捕获上升沿,即上升沿计数 ,注意:CCyNP使用默认值0 */

    GTIM_TIMX_CNT->SMCR |= (4 + chy) << 4;  /* TS[4:0] = 5/6  触发选择: 5,TI1FP1(TIMX_CH1);  6,TI2FP2(TIMX_CH2); */
    GTIM_TIMX_CNT->SMCR |= 7 << 0;          /* SMS[2:0] = 7   从模式选择: 外部时钟模式1 */

    
    GTIM_TIMX_CNT->EGR  |= 1 << 0;  /* 软件控制产生更新事件,使写入PSC的值立即生效,否则将会要等到定时器溢出才会生效 */
    GTIM_TIMX_CNT->DIER |= 1 << 0;  /* 允许更新中断 */
    GTIM_TIMX_CNT->CR1  |= 1 << 0;  /* 使能定时器TIMX */

    sys_nvic_init(1, 3, GTIM_TIMX_CNT_IRQn, 2);/* 抢占1，子优先级3，组2 */
}

/**
 * @brief       通用定时器TIMX 通道Y 获取当前计数值
 * @param       无
 * @retval      当前计数值
 */
uint32_t gtim_timx_cnt_chy_get_count(void)
{
    uint32_t count = 0;
    count = g_timxchy_cnt_ofcnt * 65536;/* 计算溢出次数对应的计数值 */
    count += GTIM_TIMX_CNT->CNT;        /* 加上当前CNT的值 */
    return count;
}

/**
 * @brief       通用定时器TIMX 通道Y 重启计数器
 * @param       无
 * @retval      当前计数值
 */
void gtim_timx_cnt_chy_restart(void)
{
    GTIM_TIMX_CNT->CR1  &= (1 << 0);    /* 关闭定时器TIMX */
    g_timxchy_cnt_ofcnt = 0;            /* 累加器清零 */
    GTIM_TIMX_CNT->CNT = 0;             /* 计数器清零 */
    GTIM_TIMX_CNT->CR1  |= 1 << 0;      /* 使能定时器TIMX */
}

/* g_npwm_remain表示当前还剩下多少个脉冲要发送
 * 每次最多发送256个脉冲
 */
static uint32_t g_npwm_remain = 0;

/**
 * @brief       通用定时器TIMX NPWM中断服务函数
 * @param       无
 * @retval      无
 */
void GTIM_TIMX_NPWM_IRQHandler(void)
{
    uint16_t npwm = 0;

    if (GTIM_TIMX_NPWM->SR & 0X0001)    /* 溢出中断, 且RCR=0 */
    {
        if (g_npwm_remain > 256)        /* 还有大于256个脉冲需要发送 */
        {
            g_npwm_remain = g_npwm_remain - 256;
            npwm = 256;
        }
        else if (g_npwm_remain % 256)   /* 还有位数（不到256）个脉冲要发送 */
        {
            npwm = g_npwm_remain % 256;
            g_npwm_remain = 0;          /* 没有脉冲了 */
        }

        if (npwm)   /* 有脉冲要发送 */
        {
            GTIM_TIMX_NPWM->RCR = npwm - 1; /* 设置重复计数寄存器值为npwm-1, 即npwm个脉冲 */
            GTIM_TIMX_NPWM->EGR |= 1 << 0;  /* 产生一次更新事件,以更新RCR寄存器 */
            GTIM_TIMX_NPWM->CR1 |= 1 << 0;  /* 使能定时器TIMX */
        }
        else
        {
            GTIM_TIMX_NPWM->CR1 &= ~(1 << 0);   /* 关闭定时器TIMX */
        }
    }

    GTIM_TIMX_NPWM->SR &= ~(1 << 0);    /* 清除中断标志位 */
}

/**
 * @brief       通用定时器TIMX 通道Y 输出指定个数PWM 初始化函数
 * @note
 *              通用定时器的时钟来自APB1,当D2PPRE1≥2分频的时候
 *              通用定时器的时钟为APB1时钟的2倍, 而APB1为120M, 所以定时器时钟 = 240Mhz
 *              定时器溢出时间计算方法: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=定时器工作频率,单位:Mhz
 *
 * @param       arr: 自动重装值
 * @param       psc: 时钟预分频数
 * @retval      无
 */
void gtim_timx_npwm_chy_init(uint16_t arr, uint16_t psc)
{
    uint8_t chy = GTIM_TIMX_NPWM_CHY;
    GTIM_TIMX_NPWM_CHY_GPIO_CLK_ENABLE();   /* TIMX 通道IO口时钟使能 */
    GTIM_TIMX_NPWM_CHY_CLK_ENABLE();        /* TIMX 时钟使能 */

    sys_gpio_set(GTIM_TIMX_NPWM_CHY_GPIO_PORT, GTIM_TIMX_NPWM_CHY_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);    /* TIMX PWM CHY 引脚模式设置 */

    sys_gpio_af_set(GTIM_TIMX_NPWM_CHY_GPIO_PORT, GTIM_TIMX_NPWM_CHY_GPIO_PIN, GTIM_TIMX_NPWM_CHY_GPIO_AF); /* IO口复用功能选择 必须设置对!! */

    GTIM_TIMX_NPWM->ARR = arr;       /* 设定计数器自动重装值 */
    GTIM_TIMX_NPWM->PSC = psc;       /* 设置预分频器  */
    GTIM_TIMX_NPWM->BDTR |= 1 << 15; /* 使能MOE位(仅TIM15/16/17 有此寄存器,必须设置MOE才能输出PWM), 其他通用定时器,
                                     * 这个寄存器是无效的, 所以设置/不设置并不影响结果, 为了兼容这里统一改成设置MOE位
                                     */

    if (chy <= 2)
    {
        GTIM_TIMX_NPWM->CCMR1 |= 6 << (4 + 8 * (chy - 1));  /* CH1/2 PWM模式1 */
        GTIM_TIMX_NPWM->CCMR1 |= 1 << (3 + 8 * (chy - 1));  /* CH1/2 预装载使能 */
    }
    else if (chy <= 4)
    {
        GTIM_TIMX_NPWM->CCMR2 |= 6 << (4 + 8 * (chy - 3));  /* CH3/4 PWM模式1 */
        GTIM_TIMX_NPWM->CCMR2 |= 1 << (3 + 8 * (chy - 3));  /* CH3/4 预装载使能 */
    }

    GTIM_TIMX_NPWM->CCER |= 1 << (4 * (chy - 1));       /* OCy 输出使能 */
    GTIM_TIMX_NPWM->CCER |= 0 << (1 + 4 * (chy - 1));   /* OCy 高电平有效 */

    GTIM_TIMX_NPWM->CR1 |= 1 << 7;  /* ARPE使能 */
    GTIM_TIMX_NPWM->DIER |= 1 << 0; /* 允许更新中断 */
    GTIM_TIMX_NPWM->CR1 |= 1 << 0;  /* 使能定时器TIMX */

    sys_nvic_init(1, 3, GTIM_TIMX_NPWM_IRQn, 2);/* 抢占1，子优先级3，组2 */
}

/**
 * @brief       通用定时器TIMX NPWM设置PWM个数
 * @param       rcr: PWM的个数, 1~2^32次方个
 * @retval      无
 */
void gtim_timx_npwm_chy_set(uint32_t npwm)
{
    if (npwm == 0)return ;

    g_npwm_remain = npwm;               /* 保存脉冲个数 */
    GTIM_TIMX_NPWM->EGR |= 1 << 0;      /* 产生一次更新事件,在中断里面处理脉冲输出 */
    GTIM_TIMX_NPWM->CR1 |= 1 << 0;      /* 使能定时器TIMX */
}















