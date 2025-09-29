/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-02
 * @brief       FLASH模拟EEPROM 实验
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


/* 要写入到STM32 FLASH的字符串数组 */
const uint8_t g_text_buf[] = {"STM32 FLASH TEST"};

#define TEXT_LENTH sizeof(g_text_buf)           /* 数组长度 */

/*SIZE表示字长(4字节), 大小必须是4的整数倍, 如果不是的话, 强制对齐到4的整数倍 */
#define SIZE  TEXT_LENTH / 4 + ((TEXT_LENTH % 4) ? 1 : 0)

#define FLASH_SAVE_ADDR     0X08004000          /* 设置FLASH 保存地址(必须大于用户代码区地址范围,且为4的倍数) */


int main(void)
{
    uint8_t key = 0;
    uint16_t i = 0;
    uint8_t datatemp[SIZE];
    
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

    lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
    lcd_show_string(30, 70, 200, 16, 16, "FLASH EEPROM TEST", RED);
    lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
    lcd_show_string(30, 110, 200, 16, 16, "KEY1:Write  KEY0:Read", RED);

    while (1)
    {
        key = key_scan(0);

        if (key == KEY1_PRES)   /* KEY1按下,写入STM32 FLASH */
        {
            lcd_fill(0, 150, 239, 319, WHITE);  /* 清除半屏 */
            lcd_show_string(30, 150, 200, 16, 16, "Start Write FLASH....", RED);
            stmflash_write(FLASH_SAVE_ADDR, (uint32_t *)g_text_buf, SIZE);
            lcd_show_string(30, 150, 200, 16, 16, "FLASH Write Finished!", RED); /* 提示传送完成 */
        }

        if (key == KEY0_PRES)   /* KEY0按下,读取字符串并显示 */
        {
            lcd_show_string(30, 150, 200, 16, 16, "Start Read FLASH.... ", RED);
            stmflash_read(FLASH_SAVE_ADDR, (uint32_t *)datatemp, SIZE);
            lcd_show_string(30, 150, 200, 16, 16, "The Data Readed Is:  ", RED); /* 提示传送完成 */
            lcd_show_string(30, 170, 200, 16, 16, (char*)datatemp, BLUE);    /* 显示读到的字符串 */
        }

        i++;
        delay_ms(10);

        if (i == 20)
        {
            LED0_TOGGLE();  /* 提示系统正在运行 */
            i = 0;
        }
    }
}







