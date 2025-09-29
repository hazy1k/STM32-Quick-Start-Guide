/**
 ****************************************************************************************************
 * @file     main.c
 * @author   正点原子团队(ALIENTEK)
 * @version  V1.0
 * @date     2020-03-13
 * @brief    通用定时器输入捕获 实验
 * @license  Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
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


extern uint8_t  g_timxchy_cap_sta;  /* 输入捕获状态 */
extern uint16_t g_timxchy_cap_val;  /* 输入捕获值 */

int main(void)
{
    uint32_t temp = 0;
    uint8_t t = 0;

    sys_cache_enable();                         /* 打开L1-Cache */
    HAL_Init();                                 /* 初始化HAL库 */
    sys_stm32_clock_init(240, 2, 2, 4);         /* 设置时钟, 480Mhz */
    delay_init(480);                            /* 延时初始化 */
    usart_init(115200);                         /* 串口初始化为115200 */
    led_init();                                 /* 初始化LED */
    key_init();                                 /* 初始化按键 */
    gtim_timx_cap_chy_init(0XFFFF, 240 - 1);    /* 以1Mhz的频率计数 捕获 */

    while (1)
    {
        if (g_timxchy_cap_sta & 0X80)           /* 成功捕获到了一次高电平 */
        {
            temp = g_timxchy_cap_sta & 0X3F;
            temp *= 65536;                      /* 溢出时间总和 */
            temp += g_timxchy_cap_val;          /* 得到总的高电平时间 */
            printf("HIGH:%d us\r\n", temp);     /* 打印总的高点平时间 */
            g_timxchy_cap_sta = 0;              /* 开启下一次捕获*/
        }

        t++;

        if (t > 20)         /* 200ms进入一次 */
        {
            t = 0;
            LED0_TOGGLE();  /* LED0闪烁 ,提示程序运行 */
        }
        delay_ms(10);
    }
}


















