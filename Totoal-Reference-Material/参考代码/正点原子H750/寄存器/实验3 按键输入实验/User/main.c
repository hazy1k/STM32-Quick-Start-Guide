/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-03-13
 * @brief       按键输入实验
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

int main(void)
{
    uint8_t key; 

    sys_stm32_clock_init(240, 2, 2, 4); /* 设置时钟, 480Mhz */
    delay_init(480);            /* 延时初始化 */
    led_init();                 /* 初始化LED */
    key_init();                 /* 初始化按键 */
    LED0(0);                    /* 先点亮LED0 */

    while (1)
    {
        key = key_scan(0);      /* 得到键值 */

        if (key)
        {
            switch (key)
            {
                case WKUP_PRES:     /* 控制LED0(RED)翻转 */
                    LED0_TOGGLE();  /* LED0状态取反 */
                    break;

                case KEY1_PRES:     /* 控制LED1(GREEN)翻转 */
                    LED1_TOGGLE();  /* LED1状态取反 */
                    break;

                case KEY0_PRES:     /* 控制LED2(BLUE)翻转 */
                    LED2_TOGGLE();  /* LED2状态取反 */
                    break;
            } 
        }
        else
        {
            delay_ms(10);
        }
    }
}
















