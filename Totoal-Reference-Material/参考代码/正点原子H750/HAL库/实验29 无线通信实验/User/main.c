/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-02
 * @brief       无线通信 实验
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
#include "./BSP/KEY/key.h"
#include "./BSP/NRF24L01/nrf24l01.h"


int main(void)
{
    uint8_t key, mode;
    uint16_t t = 0;
    uint8_t tmp_buf[33];
    
    sys_cache_enable();                 /* 打开L1-Cache */
    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(240, 2, 2, 4); /* 设置时钟, 480Mhz */
    delay_init(480);                    /* 延时初始化 */
    usart_init(115200);                 /* 串口初始化为115200 */
    mpu_memory_protection();            /* 保护相关存储区域 */
    led_init();                         /* 初始化LED */
    lcd_init();                         /* 初始化LCD */
    key_init();                         /* 初始化按键 */
    nrf24l01_init();                    /* 初始化NRF24L01 */

    lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
    lcd_show_string(30, 70, 200, 16, 16, "NRF24L01 TEST", RED);
    lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);

    while (nrf24l01_check())            /* 检查NRF24L01是否在线 */
    {
        lcd_show_string(30, 130, 200, 16, 16, "NRF24L01 Error", RED);
        delay_ms(200);
        lcd_fill(30, 130, 239, 130 + 16, WHITE);
        delay_ms(200);
    }

    lcd_show_string(30, 130, 200, 16, 16, "NRF24L01 OK", RED);

    while (1)   /* 提醒用户选择模式 */
    {
        key = key_scan(0);

        if (key == KEY0_PRES)
        {
            mode = 0;   /* 接收模式 */
            break;
        }
        else if (key == KEY1_PRES)
        {
            mode = 1;   /* 发送模式 */
            break;
        }

        t++;

        if (t == 100)   /* 显示提示信息 */
        {
            lcd_show_string(10, 150, 230, 16, 16, "KEY0:RX_Mode  KEY1:TX_Mode", RED); 
        }
        
        if (t == 200)   /* 关闭提示信息 */
        {
            lcd_fill(10, 150, 230, 150 + 16, WHITE);
            t = 0;
        }

        delay_ms(5);
    }

    lcd_fill(10, 150, 240, 166, WHITE);     /* 清空上面的显示 */

    if (mode == 0)  /* RX模式 */
    {
        lcd_show_string(30, 150, 200, 16, 16, "NRF24L01 RX_Mode", BLUE);
        lcd_show_string(30, 170, 200, 16, 16, "Received DATA:", BLUE);
        nrf24l01_rx_mode();     /* 进入RX模式 */

        while (1)
        {
            if (nrf24l01_rx_packet(tmp_buf) == 0)   /* 一旦接收到信息,则显示出来. */
            {
                tmp_buf[32] = 0;/* 加入字符串结束符 */
                lcd_show_string(0, 190, lcddev.width - 1, 32, 16, (char*)tmp_buf, BLUE);
            }
            else delay_us(100);

            t++;

            if (t == 10000)     /* 大约1s钟改变一次状态 */
            {
                t = 0;
                LED0_TOGGLE();
            }
        }
    }
    else    /* TX模式 */
    {
        lcd_show_string(30, 150, 200, 16, 16, "NRF24L01 TX_Mode", BLUE);
        nrf24l01_tx_mode();     /* 进入TX模式 */
        mode = ' ';             /* 从空格键开始发送 */

        while (1)
        {
            if (nrf24l01_tx_packet(tmp_buf) == 0)   /* 发送成功 */
            {
                lcd_show_string(30, 170, 239, 32, 16, "Sended DATA:", BLUE);
                lcd_show_string(0, 190, lcddev.width - 1, 32, 16, (char*)tmp_buf, BLUE);
                key = mode;

                for (t = 0; t < 32; t++)
                {
                    key++;

                    if (key > ('~'))key = ' ';

                    tmp_buf[t] = key;
                }

                mode++;

                if (mode > '~')mode = ' ';

                tmp_buf[32] = 0; /* 加入结束符 */
            }
            else
            {
                lcd_fill(0, 170, lcddev.width, 170 + 16 * 3, WHITE);    /* 清空显示 */
                lcd_show_string(30, 170, lcddev.width - 1, 32, 16, "Send Failed ", BLUE);
            }

            LED0_TOGGLE();
            delay_ms(200);
        };
    }
}








