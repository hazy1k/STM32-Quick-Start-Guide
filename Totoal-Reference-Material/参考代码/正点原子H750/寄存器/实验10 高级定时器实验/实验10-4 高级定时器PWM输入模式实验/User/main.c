/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-03-13
 * @brief       高级定时器PWM输入模式 实验
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 Mini Pro H750开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/KEY/key.h"
#include "./BSP/TIMER/gtim.h"
#include "./BSP/TIMER/atim.h"


extern uint16_t g_timxchy_pwmin_psc;    /* PWM输入状态 */
extern uint16_t g_timxchy_pwmin_sta;    /* PWM输入状态 */
extern uint32_t g_timxchy_pwmin_hval;   /* PWM的高电平脉宽 */
extern uint32_t g_timxchy_pwmin_cval;   /* PWM的周期宽度 */

int main(void)
{
    uint8_t t = 0;
    double ht, ct, f, tpsc;
    sys_stm32_clock_init(240, 2, 2, 4); /* 设置时钟, 480Mhz */
    delay_init(480);                    /* 延时初始化 */
    usart_init(120, 115200);            /* 串口初始化为115200 */
    led_init();                         /* 初始化LED */ 
    gtim_timx_pwm_chy_init(100 - 1, 24 - 1);/* 10Mhz的计数频率, 100Khz PWM */
    atim_timx_pwmin_chy_init();         /* 初始化PWM输入捕获 */

    GTIM_TIMX_PWM_CHY_CCRX = 20;        /* 低电平宽度20,高电平宽度80 */

    while (1)
    {
        delay_ms(10);
        t++;

        if (t >= 20)    /* 每200ms输出一次结果,并闪烁LED0,提示程序运行 */
        {
            if (g_timxchy_pwmin_sta)    /* 捕获了一次数据 */
            {
                printf("\r\n");                                     /* 输出空,另起一行 */
                printf("PWM PSC  :%d\r\n", g_timxchy_pwmin_psc);    /* 打印分频系数 */
                printf("PWM Hight:%d\r\n", g_timxchy_pwmin_hval);   /* 打印高电平脉宽 */
                printf("PWM Cycle:%d\r\n", g_timxchy_pwmin_cval);   /* 打印周期 */
                tpsc = ((double)g_timxchy_pwmin_psc + 1)/240;       /* 得到PWM采样时钟周期时间 */ 
                ht = g_timxchy_pwmin_hval * tpsc;                   /* 计算高电平时间 */
                ct = g_timxchy_pwmin_cval * tpsc;                   /* 计算周期长度 */
                f = (1 / ct) * 1000000;                             /* 计算频率 */
                printf("PWM Hight time:%.3fus\r\n", ht);            /* 打印高电平脉宽长度 */
                printf("PWM Cycle time:%.3fus\r\n", ct);            /* 打印周期时间长度 */
                printf("PWM Frequency :%.3fHz\r\n", f);             /* 打印频率 */ 
                
                atim_timx_pwmin_chy_restart(); /* 重启PWM输入检测 */
            } 

            LED0_TOGGLE();  /* LED0(RED)闪烁 */
            t = 0;
        }
    }
}















