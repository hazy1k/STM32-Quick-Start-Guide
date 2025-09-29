/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-03-29
 * @brief       QSPI 实验
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
#include "./BSP/NORFLASH/norflash.h"
#include "./BSP/NORFLASH/norflash_ex.h"


/* 要写入到FLASH的字符串数组 */
const uint8_t g_text_buf[] = {"MiniPRO STM32H7 QSPI TEST"};

#define TEXT_SIZE       sizeof(g_text_buf)  /* TEXT字符串长度 */

int main(void)
{
    uint8_t key;
    uint16_t i = 0;
    uint8_t datatemp[TEXT_SIZE];
    uint32_t flashsize;
    uint16_t id = 0;

    sys_stm32_clock_init(240, 2, 2, 4); /* 设置时钟, 480Mhz */
    delay_init(480);                    /* 延时初始化 */
    usart_init(120, 115200);            /* 串口初始化为115200 */
    usmart_dev.init(240);               /* 初始化USMART */
    mpu_memory_protection();            /* 保护相关存储区域 */
    led_init();                         /* 初始化LED */
    lcd_init();                         /* 初始化LCD */
    key_init();                         /* 初始化按键 */
    /* 
     * 不需要调用norflash_init函数了,因为sys.c里面的sys_qspi_enable_memmapmode函数已
     * 经初始化了QSPI接口,如果再调用,则内存映射模式的设置被破坏,导致QSPI代码执行异常！
     * 除非不用分散加载，所有代码放内部FLASH，才可以调用该函数！否则将导致异常！
     */
    //norflash_init();
    
    lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
    lcd_show_string(30, 70, 200, 16, 16, "QSPI TEST", RED);
    lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
    lcd_show_string(30, 110, 200, 16, 16, "KEY1:Write  KEY0:Read", RED);    /* 显示提示信息 */

    id = norflash_ex_read_id(); /* 读取FLASH ID */
    
    while ((id == 0) || (id == 0XFFFF)) /* 检测不到FLASH芯片 */
    {
        lcd_show_string(30, 130, 200, 16, 16, "FLASH Check Failed!", RED);
        delay_ms(500);
        lcd_show_string(30, 130, 200, 16, 16, "Please Check!      ", RED);
        delay_ms(500);
        LED0_TOGGLE();      /* LED0闪烁 */
    }
 
    lcd_show_string(30, 130, 200, 16, 16, "QSPI FLASH Ready!", BLUE);
    flashsize = 16 * 1024 * 1024;   /* FLASH 大小为16M字节 */ 

    while (1)
    {
        key = key_scan(0);

        if (key == KEY1_PRES)   /* KEY1按下,写入 */
        {
            lcd_fill(0, 150, 239, 319, WHITE);  /* 清除半屏 */
            lcd_show_string(30, 150, 200, 16, 16, "Start Write FLASH....", BLUE);
            sprintf((char *)datatemp, "%s%d", (char *)g_text_buf, i);
            norflash_ex_write((uint8_t *)datatemp, flashsize - 100, TEXT_SIZE);    /* 从倒数第100个地址处开始,写入SIZE长度的数据 */
            lcd_show_string(30, 150, 200, 16, 16, "FLASH Write Finished!", BLUE);  /* 提示传送完成 */
        }

        if (key == KEY0_PRES)   /* KEY0按下,读取字符串并显示 */
        {
            lcd_show_string(30, 150, 200, 16, 16, "Start Read FLASH... . ", BLUE);
            norflash_ex_read(datatemp, flashsize - 100, TEXT_SIZE);                 /* 从倒数第100个地址处开始,读出SIZE个字节 */
            lcd_show_string(30, 150, 200, 16, 16, "The Data Readed Is:   ", BLUE);  /* 提示传送完成 */
            lcd_show_string(30, 170, 200, 16, 16, (char *)datatemp, BLUE);          /* 显示读到的字符串 */
        }

        i++;

        if (i == 20)
        {
            LED0_TOGGLE();      /* LED0闪烁 */
            i = 0;
        }
        
        delay_ms(10);
    }
}



















