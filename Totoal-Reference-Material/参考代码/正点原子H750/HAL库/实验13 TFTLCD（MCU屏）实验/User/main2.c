/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-03-23
 * @brief       TFTLCD(MCU屏) 实验
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

#include "stdlib.h"
#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/KEY/key.h"
#include "./BSP/MPU/mpu.h"
#include "./BSP/LCD/lcd.h"


uint8_t lcd_id[12];

/**
 * @brief       设置LCD扫描方向
 * @param       x:0~7，表示L2R_U2D到D2U_R2L共八种方向
 * @retval      无
 */
void lcd_direction(uint8_t x)
{
    switch (x)
    {
        case 0:
            lcd_scan_dir(L2R_U2D);  /* 从左到右,从上到下 */
            break;

        case 1:
            lcd_scan_dir(L2R_D2U);  /* 从左到右,从下到上 */
            break;

        case 2:
            lcd_scan_dir(R2L_U2D);  /* 从右到左,从上到下 */
            break;

        case 3:
            lcd_scan_dir(R2L_D2U);  /* 从右到左,从下到上 */
            break;

        case 4:
            lcd_scan_dir(U2D_L2R);  /* 从上到下,从左到右 */
            break;

        case 5:
            lcd_scan_dir(U2D_R2L);  /* 从上到下,从右到左 */
            break;

        case 6:
            lcd_scan_dir(D2U_L2R);  /* 从下到上,从左到右 */
            break;

        case 7:
            lcd_scan_dir(D2U_R2L);  /* 从下到上,从右到左 */ 
            break;
    }
}

int main(void)
{
    uint8_t x = 0;
    sys_cache_enable();                 /* 打开L1-Cache */
    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(240, 2, 2, 4); /* 设置时钟, 480Mhz */
    delay_init(480);                    /* 延时初始化 */
    usart_init(115200);                 /* 串口初始化为115200 */
    led_init();                         /* 初始化LED */
    mpu_memory_protection();            /* 保护相关存储区域 */
    lcd_init();                         /* 初始化LCD */
    g_point_color = RED;
    sprintf((char *)lcd_id, "LCD ID:%04X", lcddev.id);  /* 将LCD ID打印到lcd_id数组 */

    while (1)
    {
        lcd_direction(x);  /* 设置LCD扫描方向 */

        switch (x)
        {
            case 0:
                lcd_clear(BROWN);
                break;

            case 1:
                lcd_clear(BLACK);
                break;

            case 2:
                lcd_clear(BLUE);
                break;

            case 3:
                lcd_clear(RED);
                break;

            case 4:
                lcd_clear(MAGENTA);
                break;

            case 5:
                lcd_clear(GREEN);
                break;

            case 6:
                lcd_clear(CYAN);
                break;

            case 7:
                lcd_clear(YELLOW);
                break;
        }

        lcd_show_string(64, 10, 96, 32, 32, "X--->", RED);
        lcd_show_string(10, 64, 32, 32, 32, "Y", RED);
        lcd_show_string(10, 96, 32, 32, 32, "|", RED);
        lcd_show_string(10, 112, 32, 32, 32, "|", RED);
        lcd_show_string(10, 144, 32, 32, 32, "|", RED);
        lcd_show_string(10, 176, 32, 32, 32, "V", RED);
        delay_ms(3000);

        lcd_show_string(64, 64, 240, 32, 32, "MiniPRO STM32", RED);
        lcd_show_string(64, 104, 240, 24, 24, "TFTLCD TEST", RED);
        lcd_show_string(64, 134, 240, 16, 16, "ATOM@ALIENTEK", RED);
        lcd_show_string(64, 154, 240, 16, 16, (char*)lcd_id, RED);  /* 显示LCD ID */
        lcd_show_string(64, 174, 240, 12, 12, "2020/3/23", RED);

        x++;
        if (x == 8)x = 0;

        LED0_TOGGLE();  /* 红灯闪烁 */
        delay_ms(3000);
    }
}











