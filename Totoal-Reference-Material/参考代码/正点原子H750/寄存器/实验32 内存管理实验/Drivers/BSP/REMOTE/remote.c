/**
 ****************************************************************************************************
 * @file        remote.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-02
 * @brief       红外遥控解码 驱动代码
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
 * V1.0 20200402
 * 第一次发布
 *
 ****************************************************************************************************
 */

#include "./BSP/REMOTE/remote.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"


/**
 * @brief       红外遥控初始化
 *   @note      设置IO以及定时器的输入捕获
 * @param       无
 * @retval      无
 */
void remote_init(void)
{
    uint8_t chy = REMOTE_IN_TIMX_CHY;
    
    REMOTE_IN_GPIO_CLK_ENABLE();            /* 红外接入引脚GPIO时钟使能 */
    REMOTE_IN_TIMX_CHY_CLK_ENABLE();        /* 定时器时钟使能 */

    sys_gpio_set(REMOTE_IN_GPIO_PORT, REMOTE_IN_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);    /* IO模式设置 */

    sys_gpio_af_set(REMOTE_IN_GPIO_PORT, REMOTE_IN_GPIO_PIN, REMOTE_IN_GPIO_AF);    /* 复用功能设置 */

    REMOTE_IN_TIMX->ARR = 10000;            /* 设定计数器自动重装值 最大10ms溢出 */
    REMOTE_IN_TIMX->PSC = 240 - 1;          /* 预分频器,1M的计数频率, 1us加1. */

    if (chy <= 2)
    {
        REMOTE_IN_TIMX->CCMR1 |= 1 << 8 * (chy - 1);        /* CCyS[1:0]   = 01 选择输入端 IC1/2映射到TI1/2上 */
        REMOTE_IN_TIMX->CCMR1 |= 0 << (2 + 8 * (chy - 1));  /* ICyPSC[1:0] = 00 输入捕获不分频,全捕获 */
        REMOTE_IN_TIMX->CCMR1 |= 0 << (4 + 8 * (chy - 1));  /* ICyF[3:0]   = 00 输入端滤波 不滤波 */
    }
    else if (chy <= 4)
    {
        REMOTE_IN_TIMX->CCMR2 |= 1 << 8 * (chy - 3);        /* CCyS[1:0]   = 01 选择输入端 IC3/4映射到TI3/4上 */
        REMOTE_IN_TIMX->CCMR2 |= 0 << (2 + 8 * (chy - 3));  /* ICyPSC[1:0] = 00 输入捕获不分频,全捕获 */
        REMOTE_IN_TIMX->CCMR2 |= 0 << (4 + 8 * (chy - 3));  /* ICyF[3:0]   = 00 输入端滤波 不滤波 */
    }

    REMOTE_IN_TIMX->CCER |= 1 << (4 * (chy - 1));       /* CCyE = 1 输入捕获使能 */
    REMOTE_IN_TIMX->CCER |= 0 << (1 + 4 * (chy - 1));   /* CCyP = 0 捕获上升沿 ,注意:CCyNP使用默认值0 */

    REMOTE_IN_TIMX->EGR  |= 1 << 0;         /* 软件控制产生更新事件,使写入PSC的值立即生效,否则将会要等到定时器溢出才会生效 */
    REMOTE_IN_TIMX->DIER |= 1 << chy;       /* 允许捕获中断 */
    REMOTE_IN_TIMX->DIER |= 1 << 0;         /* 允许更新中断 */
    REMOTE_IN_TIMX->CR1  |= 1 << 0;         /* 使能定时器TIMX */
    
    sys_nvic_init(1, 2, REMOTE_IN_TIMX_IRQn, 2);    /* 抢占1，子优先级2，组2 */
    
    /* 如果是通用定时器 ,不需要设置CC中断 */
    sys_nvic_init(1, 3, REMOTE_IN_TIMX_CC_IRQn, 2); /* 抢占1，子优先级3，组2 */
}


/* 遥控器接收状态
 * [7]  : 收到了引导码标志
 * [6]  : 得到了一个按键的所有信息
 * [5]  : 保留
 * [4]  : 标记上升沿是否已经被捕获
 * [3:0]: 溢出计时器
 */
uint8_t g_remote_sta = 0;
uint32_t g_remote_data = 0; /* 红外接收到的数据 */
uint8_t  g_remote_cnt = 0;  /* 按键按下的次数 */

/**
 * @brief       定时器溢出中断服务函数
 * @param       无
 * @retval      无
 */
void REMOTE_IN_TIMX_IRQHandler(void)
{
    uint16_t tsr;
    
    tsr = REMOTE_IN_TIMX->SR;

    if (tsr & 0X01)             /* 处理溢出 */
    {
        if (g_remote_sta & 0x80)      /* 上次有数据被接收到了 */
        {
            g_remote_sta &= ~0X10;    /* 取消上升沿已经被捕获标记 */

            if ((g_remote_sta & 0X0F) == 0X00)
            {
                g_remote_sta |= 1 << 6; /* 标记已经完成一次按键的键值信息采集 */
            }
            
            if ((g_remote_sta & 0X0F) < 14)
            {
                g_remote_sta++;
            }
            else
            {
                g_remote_sta &= ~(1 << 7);    /* 清空引导标识 */
                g_remote_sta &= 0XF0; /* 清空计数器 */
            }
        }
    }

    REMOTE_IN_TIMX->SR &= 0;    /* 清除中断标志位 */
}

/**
 * @brief       定时器输入捕获中断服务函数
 * @param       无
 * @retval      无
 */
void REMOTE_IN_TIMX_CC_IRQHandler(void)
{
    uint16_t dval;  /* 下降沿时计数器的值 */
    uint16_t tsr = REMOTE_IN_TIMX->SR;       /* 获取中断状态 */
    uint8_t chy = REMOTE_IN_TIMX_CHY;       /* 需要捕获的通道 */

    if (tsr & (1 << chy))   /* 处理捕获(chy通道)中断 */
    {
        if (RDATA)  /* 上升沿捕获 */
        {
            REMOTE_IN_TIMX->CCER |= 1 << (1 + 4 * (chy - 1));   /* CCyP = 1 设置为下降沿捕获 */  
            REMOTE_IN_TIMX->CNT = 0;        /* 清空定时器值 */
            g_remote_sta |= 0X10;           /* 标记上升沿已经被捕获 */
        }
        else    /* 下降沿捕获 */
        {
            dval = REMOTE_IN_TIMX_CCRY;     /* 读取CCRx也可以清CC1IF标志位 */ 
            REMOTE_IN_TIMX->CCER &= ~(1 << (1 + 4 * (chy - 1)));/* CCyP = 0 设置为上升沿捕获 */ 

            if (g_remote_sta & 0X10)        /* 完成一次高电平捕获 */
            {
                if (g_remote_sta & 0X80)    /* 接收到了引导码 */
                {

                    if (dval > 300 && dval < 800)   /* 560为标准值,560us */
                    {
                        g_remote_data <<= 1;/* 左移一位. */
                        g_remote_data |= 0; /* 接收到0 */
                    }
                    else if (dval > 1400 && dval < 1800)    /* 1680为标准值,1680us */
                    {
                        g_remote_data <<= 1;/* 左移一位. */
                        g_remote_data |= 1; /* 接收到1 */
                    }
                    else if (dval > 2000 && dval < 3000)    /* 得到按键键值增加的信息 2500为标准值2.5ms */
                    {
                        g_remote_cnt++;         /* 按键次数增加1次 */
                        g_remote_sta &= 0XF0;   /* 清空计时器 */
                    }
                }
                else if (dval > 4200 && dval < 4700)    /* 4500为标准值4.5ms */
                {
                    g_remote_sta |= 1 << 7; /* 标记成功接收到了引导码 */
                    g_remote_cnt = 0;       /* 清除按键次数计数器 */
                }
            }

            g_remote_sta &= ~(1 << 4);
        }
    }

    REMOTE_IN_TIMX->SR = 0;     /* 清除中断标志位 */
}

/**
 * @brief       处理红外按键(类似按键扫描)
 * @param       无
 * @retval      0   , 没有任何按键按下
 *              其他, 按下的按键键值
 */
uint8_t remote_scan(void)
{
    uint8_t sta = 0;
    uint8_t t1, t2;

    if (g_remote_sta & (1 << 6))    /* 得到一个按键的所有信息了 */
    {
        t1 = g_remote_data >> 24;   /* 得到地址码 */
        t2 = (g_remote_data >> 16) & 0xff;  /* 得到地址反码 */

        if ((t1 == (uint8_t)~t2) && t1 == REMOTE_ID)    /* 检验遥控识别码(ID)及地址 */
        {
            t1 = g_remote_data >> 8;
            t2 = g_remote_data;

            if (t1 == (uint8_t)~t2)
            {
                sta = t1;           /* 键值正确 */
            }
        }

        if ((sta == 0) || ((g_remote_sta & 0X80) == 0)) /* 按键数据错误/遥控已经没有按下了 */
        {
            g_remote_sta &= ~(1 << 6);  /* 清除接收到有效按键标识 */
            g_remote_cnt = 0;           /* 清除按键次数计数器 */
        }
    }

    return sta;
}
































