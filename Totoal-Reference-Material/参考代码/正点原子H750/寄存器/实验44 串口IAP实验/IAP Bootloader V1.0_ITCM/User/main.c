/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-05
 * @brief       串口IAP 实验-IAP Bootloader V1.0_ITCM
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
#include "./BSP/STMFLASH/stmflash.h"
#include "./BSP/NORFLASH/norflash.h"
#include "./IAP/iap.h"


int main(void)
{
    uint8_t t;
    uint8_t key;
    uint32_t oldcount = 0;              /* 老的串口接收数据值 */
    uint32_t applenth = 0;              /* 接收到的app代码长度 */
    uint8_t clearflag = 0;
    uint16_t id = 0;

    sys_stm32_clock_init(240, 2, 2, 4); /* 设置时钟, 480Mhz */
    
    /* 中断向量表放在ITCM RAM,无偏移 */
    sys_nvic_set_vector_table(D1_ITCMRAM_BASE, 0);
    
    delay_init(480);                    /* 延时初始化 */
    usart_init(120, 115200);            /* 串口初始化为115200 */
    mpu_memory_protection();            /* 保护相关存储区域 */
    lcd_init();                         /* 初始化LCD */
    led_init();                         /* 初始化LED */
    key_init();                         /* 初始化按键 */
    norflash_init();                    /* NORFLASH(W25Q64)初始化 */


    id = norflash_read_id(); /* 读取FLASH ID */
    
    while ((id == 0) || (id == 0XFFFF)) /* 检测不到FLASH芯片 */
    {
        lcd_show_string(30, 150, 200, 16, 16, "FLASH Check Failed!", RED);
        delay_ms(500);
        lcd_show_string(30, 150, 200, 16, 16, "Please Check!      ", RED);
        delay_ms(500);
        LED0_TOGGLE();      /* LED0闪烁 */
    }

    lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
    lcd_show_string(30, 70, 200, 16, 16, "IAP TEST", RED);
    lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
    lcd_show_string(30, 110, 200, 16, 16, "KEY_UP:#1 Copy APP2QSPI!", RED);
    lcd_show_string(30, 130, 200, 16, 16, "KEY1:Copy APP2FLASH & run", RED);
    lcd_show_string(30, 150, 200, 16, 16, "KEY0:Run SRAM APP", RED);

    while (1)
    {
        if (g_usart_rx_cnt)
        {
            if (oldcount == g_usart_rx_cnt)   /* 新周期内,没有收到任何数据,认为本次数据接收完成 */
            {
                applenth = g_usart_rx_cnt;
                oldcount = 0;
                g_usart_rx_cnt = 0;
                printf("用户程序接收完成!\r\n");
                printf("代码长度:%dBytes\r\n", applenth);
            }
            else oldcount = g_usart_rx_cnt;
        }

        t++;
        delay_ms(10);

        if (t == 30)
        {
            LED0_TOGGLE();
            t = 0;

            if (clearflag)
            {
                clearflag--;

                if (clearflag == 0)
                {
                    lcd_fill(30, 210, 240, 210 + 16, WHITE);    /* 清除显示 */
                }
            }
        }

        key = key_scan(0);

        /* 注意:QSPI FLASH不做任何校验和检查,所以请确保数据正确!否则将运行出错 */
        if (key == WKUP_PRES)   /* WKUP按下,更新固件到外部QSPI FLASH */
        {
            if (applenth)
            {
                printf("开始更新固件...\r\n");
                lcd_show_string(30, 210, 200, 16, 16, "Copying APP2QSPI...", BLUE);
                norflash_write((uint8_t *)g_usart_rx_buf, QSPI_APP1_ADDR - 0X90000000, applenth); /* 写入QSPI FLASH! */
                lcd_show_string(30, 210, 200, 16, 16, "Copy APP Successed!!", BLUE);
                printf("固件更新完成!\r\n");
            }
            else
            {
                printf("没有可以更新的固件!\r\n");
                lcd_show_string(30, 210, 200, 16, 16, "No APP!", BLUE);
            }

            clearflag = 7; /* 标志更新了显示,并且设置7*300ms后清除显示 */
        }

        if (key == KEY1_PRES)	/* KEY1按键按下,更新固件到内部FLASH，更新完后自动运行FLASH APP代码 */
            /* 注意:如果APP还有QSPI FLASH部分代码，则必须先更新QSPI FLASH */
        {
            if (applenth)
            {
                printf("开始更新固件...\r\n");
                lcd_show_string(30, 210, 200, 16, 16, "Copying APP2FLASH...", BLUE);

                if (((*(volatile uint32_t *)(SRAM_APP1_ADDR + 4)) & 0xFF000000) == 0x08000000)   /* 判断是否为0X08XXXXXX */
                {
                    iap_write_appbin(FLASH_APP1_ADDR, g_usart_rx_buf, applenth);    /* 更新FLASH代码 */
                    lcd_show_string(30, 210, 200, 16, 16, "Copy APP Successed!!", BLUE);
                    printf("固件更新完成!\r\n");
                    delay_ms(500);
                    printf("开始执行FLASH用户代码!!\r\n\r\n");
                    delay_ms(10);

                    if (((*(volatile uint32_t *)(FLASH_APP1_ADDR + 4)) & 0xFF000000) == 0x08000000)   /* 判断是否为0X08XXXXXX */
                    {
                        iap_load_app(FLASH_APP1_ADDR);/* 执行FLASH APP代码 */
                    }
                    else
                    {
                        printf("非FLASH应用程序,无法执行!\r\n");
                        lcd_show_string(30, 210, 200, 16, 16, "Illegal FLASH APP!", BLUE);
                    }
                }
                else
                {
                    lcd_show_string(30, 210, 200, 16, 16, "Illegal FLASH APP!  ", BLUE);
                    printf("非FLASH应用程序!\r\n");
                }
            }
            else if (((*(volatile uint32_t *)(FLASH_APP1_ADDR + 4)) & 0xFF000000) == 0x08000000)  	/* 判断FLASH里面是否有APP,有的话执行 */
            {
                printf("开始执行FLASH用户代码!!\r\n\r\n");
                delay_ms(10);
                iap_load_app(FLASH_APP1_ADDR);/* 执行FLASH APP代码 */

            }
            else
            {
                printf("没有可以更新的固件!\r\n");
                lcd_show_string(30, 210, 200, 16, 16, "No APP!", BLUE);
            }

            clearflag = 7; /* 标志更新了显示,并且设置7*300ms后清除显示 */
        }

        if (key == KEY0_PRES)  	/* KEY0按下 */
        {
            printf("开始执行SRAM用户代码!!\r\n\r\n");
            delay_ms(10);

            if (((*(volatile uint32_t *)(SRAM_APP1_ADDR + 4)) & 0xFF000000) == 0x24000000)   /* 判断是否为0X24XXXXXX */
            {
                iap_load_app(SRAM_APP1_ADDR);/* SRAM地址 */
            }
            else
            {
                printf("非SRAM应用程序,无法执行!\r\n");
                lcd_show_string(30, 210, 200, 16, 16, "Illegal SRAM APP!", BLUE);
            }

            clearflag = 7; /* 标志更新了显示,并且设置7*300ms后清除显示 */
        }

    }
}







