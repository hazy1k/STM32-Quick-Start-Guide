/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-02
 * @brief       红外遥控 实验
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
#include "./USMART/usmart.h"
#include "./BSP/MPU/mpu.h"
#include "./BSP/LED/led.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/REMOTE/remote.h"


int main(void)
{
    uint8_t key;
    uint8_t t = 0;
    char *str = 0;
    sys_stm32_clock_init(240, 2, 2, 4); /* 设置时钟, 480Mhz */
    delay_init(480);                    /* 延时初始化 */
    usart_init(120, 115200);            /* 串口初始化为115200 */
    mpu_memory_protection();            /* 保护相关存储区域 */
    led_init();                         /* 初始化LED */
    lcd_init();                         /* 初始化LCD */
    remote_init();                      /* 红外接收初始化 */

    lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
    lcd_show_string(30, 70, 200, 16, 16, "REMOTE TEST", RED);
    lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
    lcd_show_string(30, 110, 200, 16, 16, "KEYVAL:", RED);
    lcd_show_string(30, 130, 200, 16, 16, "KEYCNT:", RED);
    lcd_show_string(30, 150, 200, 16, 16, "SYMBOL:", RED);

    while (1)
    {
        key = remote_scan();

        if (key)
        {
            lcd_show_num(86, 110, key, 3, 16, BLUE);    /* 显示键值 */
            lcd_show_num(86, 130, g_remote_cnt, 3, 16, BLUE);   /* 显示按键次数 */

            switch (key)
            {
                case 0:
                    str = "ERROR";
                    break;

                case 69:
                    str = "POWER";
                    break;

                case 70:
                    str = "UP";
                    break;

                case 64:
                    str = "PLAY";
                    break;

                case 71:
                    str = "ALIENTEK";
                    break;

                case 67:
                    str = "RIGHT";
                    break;

                case 68:
                    str = "LEFT";
                    break;

                case 7:
                    str = "VOL-";
                    break;

                case 21:
                    str = "DOWN";
                    break;

                case 9:
                    str = "VOL+";
                    break;

                case 22:
                    str = "1";
                    break;

                case 25:
                    str = "2";
                    break;

                case 13:
                    str = "3";
                    break;

                case 12:
                    str = "4";
                    break;

                case 24:
                    str = "5";
                    break;

                case 94:
                    str = "6";
                    break;

                case 8:
                    str = "7";
                    break;

                case 28:
                    str = "8";
                    break;

                case 90:
                    str = "9";
                    break;

                case 66:
                    str = "0";
                    break;

                case 74:
                    str = "DELETE";
                    break;
            }

            lcd_fill(86, 150, 116 + 8 * 8, 170 + 16, WHITE);    /* 清楚之前的显示 */
            lcd_show_string(86, 150, 200, 16, 16, str, BLUE);   /* 显示SYMBOL */
        }
        else
        {
            delay_ms(10);
        }

        t++;

        if (t == 20)
        {
            t = 0;
            LED0_TOGGLE();  /* LED0闪烁 */
        }
    }
}


































