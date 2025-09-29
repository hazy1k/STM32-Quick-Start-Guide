/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-06
 * @brief       U盘(Host) 实验
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
#include "./MALLOC/malloc.h"
#include "./FATFS/exfuns/exfuns.h"
#include "./TEXT/text.h"
#include "./BSP/MPU/mpu.h"
#include "./BSP/LED/led.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/KEY/key.h"
#include "./BSP/SDMMC/sdmmc_sdcard.h"
#include "./PICTURE/piclib.h"
#include "usbh_core.h"
#include "usbh_msc.h"


USBH_HandleTypeDef hUSBHost;    /* USB Host处理结构体 */


static void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id)
{
    uint32_t total, free;
    uint8_t res = 0;
    printf("id:%d\r\n", id);

    switch (id)
    {
        case HOST_USER_SELECT_CONFIGURATION:
            break;

        case HOST_USER_DISCONNECTION:
            f_mount(0, "2:", 1);        /* 卸载U盘 */
            text_show_string(30, 140, 200, 16, "设备连接中...", 16, 0, RED);
            lcd_fill(30, 160, 239, 220, WHITE);
            break;

        case HOST_USER_CLASS_ACTIVE:
            text_show_string(30, 140, 200, 16, "设备连接成功!", 16, 0, RED);
            f_mount(fs[2], "2:", 1);    /* 重新挂载U盘 */
            res = exfuns_get_free("2:", &total, &free);

            if (res == 0)
            {
                lcd_show_string(30, 160, 200, 16, 16, "FATFS OK!", BLUE);
                lcd_show_string(30, 180, 200, 16, 16, "U Disk Total Size:     MB", BLUE);
                lcd_show_string(30, 200, 200, 16, 16, "U Disk  Free Size:     MB", BLUE);
                lcd_show_num(174, 180, total >> 10, 5, 16, BLUE);   /* 显示U盘总容量 MB */
                lcd_show_num(174, 200, free >> 10, 5, 16, BLUE);
            }
            else
            {
                printf("U盘存储空间获取失败\r\n");
            }

            break;

        case HOST_USER_CONNECTION:
            break;

        default:
            break;
    }
}

int main(void)
{
    uint8_t t = 0;

    sys_stm32_clock_init(240, 2, 2, 4); /* 设置时钟, 480Mhz */
    delay_init(480);                    /* 延时初始化 */
    usart_init(120, 115200);            /* 串口初始化为115200 */
    usmart_dev.init(240);               /* 初始化USMART */
    mpu_memory_protection();            /* 保护相关存储区域 */
    lcd_init();                         /* 初始化LCD */
    led_init();                         /* 初始化LED */
    key_init();                         /* 初始化按键 */
    my_mem_init(SRAMIN);                /* 初始化内部内存池(AXI) */
    my_mem_init(SRAM12);                /* 初始化SRAM12内存池(SRAM1+SRAM2) */
    my_mem_init(SRAM4);                 /* 初始化SRAM4内存池(SRAM4) */
    my_mem_init(SRAMDTCM);              /* 初始化DTCM内存池(DTCM) */
    my_mem_init(SRAMITCM);              /* 初始化ITCM内存池(ITCM) */
    exfuns_init();                      /* 为fatfs相关变量申请内存 */
    f_mount(fs[0], "0:", 1);            /* 挂载SD卡 */
    f_mount(fs[1], "1:", 1);            /* 挂载FLASH */

    piclib_init();                      /* 初始化画图 */
    
    while (fonts_init())         /* 检查字库 */
    {
        lcd_show_string(30, 50, 200, 16, 16, "Font Error!", RED);
        delay_ms(200);
        lcd_fill(30, 50, 240, 66, WHITE);   /* 清除显示 */
        delay_ms(200);
    }

    text_show_string(30, 50, 200, 16, "STM32", 16, 0, RED);
    text_show_string(30, 70, 200, 16, "USB U盘 实验", 16, 0, RED);
    text_show_string(30, 90, 200, 16, "ATOM@ALIENTEK", 16, 0, RED);
    text_show_string(30, 120, 200, 16, "设备连接中...", 16, 0, RED);

    USBH_Init(&hUSBHost, USBH_UserProcess, 0);
    USBH_RegisterClass(&hUSBHost, USBH_MSC_CLASS);
    USBH_Start(&hUSBHost);

    while (1)
    {
        USBH_Process(&hUSBHost);
        delay_ms(10);
        t++;

        if (t == 50)
        {
            t = 0;
            LED0_TOGGLE();
        }
    }
}



