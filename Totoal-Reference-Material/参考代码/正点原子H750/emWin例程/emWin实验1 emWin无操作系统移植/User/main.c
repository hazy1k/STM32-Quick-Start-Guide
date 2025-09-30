/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.4
 * @date        2022-01-04
 * @brief       无操作系统---emWin移植实验
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
#include "./BSP/TOUCH/touch.h"
#include "./BSP/TIMER/btim.h"
#include "./BSP/KEY/key.h"
#include "./MALLOC/malloc.h"
#include "GUI.h"
#include "GUIDEMO.h"
#include "WM.h"



int main(void)
{
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
    tp_dev.init();                      /* 触摸屏初始化 */
    btim_timx_int_init(1000-1,240-1);   /* 定时1ms提供emwin时基 */

    GUI_Init();                         /* emWin 初始化 */
    GUIDEMO_Main();
    
    while(1);
}
