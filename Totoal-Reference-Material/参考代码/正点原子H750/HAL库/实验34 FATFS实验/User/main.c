/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-03
 * @brief       FATFS 实验
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
#include "./BSP/MPU/mpu.h"
#include "./BSP/LED/led.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/KEY/key.h"
#include "./BSP/SDMMC/sdmmc_sdcard.h"
#include "./FATFS/exfuns/exfuns.h"


int main(void)
{
    uint32_t total, free;
    uint8_t t = 0;
    uint8_t res = 0;

    sys_cache_enable();                 /* 打开L1-Cache */
    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(240, 2, 2, 4); /* 设置时钟, 480Mhz */
    delay_init(480);                    /* 延时初始化 */
    usart_init(115200);                 /* 串口初始化为115200 */
    usmart_dev.init(240);               /* 初始化USMART */
    mpu_memory_protection();            /* 保护相关存储区域 */
    led_init();                         /* 初始化LED */
    lcd_init();                         /* 初始化LCD */
    key_init();                         /* 初始化按键 */
    my_mem_init(SRAMIN);                /* 初始化内部内存池(AXI) */
    my_mem_init(SRAM12);                /* 初始化SRAM12内存池(SRAM1+SRAM2) */
    my_mem_init(SRAM4);                 /* 初始化SRAM4内存池(SRAM4) */
    my_mem_init(SRAMDTCM);              /* 初始化DTCM内存池(DTCM) */
    my_mem_init(SRAMITCM);              /* 初始化ITCM内存池(ITCM) */

    lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
    lcd_show_string(30, 70, 200, 16, 16, "FATFS TEST", RED);
    lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
    lcd_show_string(30, 110, 200, 16, 16, "Use USMART for test", RED);

    while (sd_init())   /* 检测不到SD卡 */
    {
        lcd_show_string(30, 150, 200, 16, 16, "SD Card Error!", RED);
        delay_ms(500);
        lcd_show_string(30, 150, 200, 16, 16, "Please Check! ", RED);
        delay_ms(500);
        LED0_TOGGLE();  /* LED0闪烁 */
    }

    exfuns_init();                  /* 为fatfs相关变量申请内存 */
    f_mount(fs[0], "0:", 1);        /* 挂载SD卡 */
    res = f_mount(fs[1], "1:", 1);  /* 挂载FLASH. */

    if (res == 0X0D)                /* FLASH磁盘,FAT文件系统错误,重新格式化FLASH */
    {
        lcd_show_string(30, 150, 200, 16, 16, "Flash Disk Formatting...", RED);         /* 格式化FLASH */
        res = f_mkfs("1:", 0, 0, FF_MAX_SS);            /* 格式化FLASH,1:,盘符;0,使用默认格式化参数 */

        if (res == 0)
        {
            f_setlabel((const TCHAR *)"1:ALIENTEK");    /* 设置Flash磁盘的名字为：ALIENTEK */
            lcd_show_string(30, 150, 200, 16, 16, "Flash Disk Format Finish", RED);     /* 格式化完成 */
        }
        else lcd_show_string(30, 150, 200, 16, 16, "Flash Disk Format Error ", RED);    /* 格式化失败 */

        delay_ms(1000);
    }

    lcd_fill(30, 150, 240, 150 + 16, WHITE);        /* 清除显示 */

    while (exfuns_get_free("0", &total, &free))     /* 得到SD卡的总容量和剩余容量 */
    {
        lcd_show_string(30, 150, 200, 16, 16, "SD Card Fatfs Error!", RED);
        delay_ms(200);
        lcd_fill(30, 150, 240, 150 + 16, WHITE);    /* 清除显示 */
        delay_ms(200);
        LED0_TOGGLE();                              /* LED0闪烁 */
    }

    lcd_show_string(30, 150, 200, 16, 16, "FATFS OK!", BLUE);
    lcd_show_string(30, 170, 200, 16, 16, "SD Total Size:     MB", BLUE);
    lcd_show_string(30, 190, 200, 16, 16, "SD Free Size:     MB", BLUE);
    lcd_show_num(30 + 8 * 14, 170, total >> 10, 5, 16, BLUE);   /* 显示SD卡总容量 MB */
    lcd_show_num(30 + 8 * 14, 190, free >> 10, 5, 16, BLUE);    /* 显示SD卡剩余容量 MB */

    while (1)
    {
        t++;
        delay_ms(200);
        LED0_TOGGLE();  /* LED0闪烁 */
    }
}









