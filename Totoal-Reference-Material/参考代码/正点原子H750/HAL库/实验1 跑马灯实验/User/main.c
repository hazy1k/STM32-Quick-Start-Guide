/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-03-12
 * @brief       跑马灯 实验
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


int main(void)
{
    sys_cache_enable();                  /* 打开L1-Cache */
    HAL_Init();                          /* 初始化HAL库 */
    sys_stm32_clock_init(240, 2, 2, 4);  /* 设置时钟, 480Mhz */
    delay_init(480);                     /* 延时初始化 */
    led_init();                          /* 初始化LED */

    while (1)
    {
        LED2(1);                         /* LED2(BLUE)  灭 */
        LED0(0);                         /* LED0(RED)   亮 */
        delay_ms(500);
        LED0(1);                         /* LED0(RED)   灭 */
        LED1(0);                         /* LED1(GREEN) 亮 */
        delay_ms(500);
        LED1(1);                         /* LED1(GREEN) 灭 */
        LED2(0);                         /* LED2(BLUE)  亮 */
        delay_ms(500);
    }
}
