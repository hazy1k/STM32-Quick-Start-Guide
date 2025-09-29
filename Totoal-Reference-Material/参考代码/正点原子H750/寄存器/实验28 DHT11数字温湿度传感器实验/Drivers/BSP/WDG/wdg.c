/**
 ****************************************************************************************************
 * @file        wdg.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.1
 * @date        2020-03-13
 * @brief       看门狗 驱动代码
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
 * V1.1 20200312
 * 新增wwdg_init和wwdg_set_counter函数
 *
 ****************************************************************************************************
 */

#include "./BSP/WDG/wdg.h"
#include "./BSP/LED/led.h"

/**
 * @brief       初始化独立看门狗 
 * @param       prer: 分频数:0~7(只有低3位有效!)
 *   @arg       分频因子 = 4 * 2^prer. 但最大值只能是256!
 * @param       rlr: 自动重装载值,0~0XFFF. 
 * @note        时间计算(大概):Tout=((4 * 2^prer) * rlr) / 32 (ms). 
 * @retval      无
 */
void iwdg_init(uint8_t prer, uint16_t rlr)
{
    IWDG1->KR = 0X5555; /* 使能对IWDG->PR和IWDG->RLR的写 */
    IWDG1->PR = prer;   /* 设置分频系数 */
    IWDG1->RLR = rlr;   /* 从加载寄存器 IWDG->RLR */
    IWDG1->KR = 0XAAAA; /* reload */
    IWDG1->KR = 0XCCCC; /* 使能看门狗 */
}

/**
 * @brief       喂独立看门狗
 * @param       无
 * @retval      无
 */
void iwdg_feed(void)
{
    IWDG1->KR = 0XAAAA; /* reload */
}

/* 保存WWDG计数器的设置值,默认为最大. */
uint8_t g_wwdg_cnt = 0x7f;

/**
 * @brief       初始化窗口看门狗
 * @param       tr: T[6:0],计数器值
 * @param       tw: W[6:0],窗口值
 * @note        fprer:分频系数（WDGTB）,范围:0~7,表示2^WDGTB分频
 *              Fwwdg=PCLK3/(4096*2^fprer). 一般PCLK3=120Mhz
 *              溢出时间=(4096*2^fprer)*(tr-0X3F)/PCLK3
 *              假设fprer=4,tr=7f,PCLK3=120Mhz
 *              则溢出时间=4096*16*64/120Mhz=34.95ms
 * @retval      无
 */
void wwdg_init(uint8_t tr, uint8_t wr, uint8_t fprer)
{
    RCC->APB3ENR |= 1 << 6;     /* 使能wwdg时钟 */
    g_wwdg_cnt = tr & g_wwdg_cnt;/* 初始化WWDG_CNT. */
    RCC->GCR |= 1 << 0;         /* WW1RSC=1,复位WWDG1,硬件清零 */
    WWDG1->CFR |= fprer << 11;  /* PCLK1/4096再除2^fprer */
    WWDG1->CFR &= 0XFF80;
    WWDG1->CFR |= wr;           /* 设定窗口值 */
    WWDG1->CR |= g_wwdg_cnt;    /* 设定计数器值 */
    WWDG1->CR |= 1 << 7;        /* 开启看门狗 */
    sys_nvic_init(2, 3, WWDG_IRQn, 2); /* 抢占2，子优先级3，组2 */
    WWDG1->SR = 0X00;           /* 清除提前唤醒中断标志位 */
    WWDG1->CFR |= 1 << 9;       /* 使能提前唤醒中断 */
}

/**
 * @brief       重设置WWDG计数器的值
 * @param       无
 * @retval      无
 */
void wwdg_set_counter(uint8_t cnt)
{
    WWDG1->CR = (cnt & 0x7F);   /* 重设置7位计数器 */
}

/**
 * @brief       窗口看门狗中断服务程序
 * @param       无
 * @retval      无
 */
void WWDG_IRQHandler(void)
{  
    if (WWDG1->SR & 0X01)               /* 先判断是否发生了WWDG提前唤醒中断 */
    {
        WWDG1->SR = 0X00;               /* 清除提前唤醒中断标志位 */
        wwdg_set_counter(g_wwdg_cnt);   /* 重设窗口看门狗的值! */
        LED1_TOGGLE();                  /* 绿灯闪烁 */
    }
}








