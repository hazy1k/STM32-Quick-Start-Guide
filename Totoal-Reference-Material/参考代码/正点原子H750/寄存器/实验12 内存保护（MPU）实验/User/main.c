/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-03-22
 * @brief       内存保护(MPU) 实验
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


#if !(__ARMCC_VERSION >= 6010050)   /* 不是AC6编译器，即使用AC5编译器时 */
uint8_t mpudata[128] __attribute__((at(0X20002000)));  /* 定义一个数组 */
#else
uint8_t mpudata[128] __attribute__((section(".bss.ARM.__at_0X20002000"))); /* 定义一个数组 */
#endif

int main(void)
{
    uint8_t key = 0;
    uint8_t t = 0; 
    sys_stm32_clock_init(240, 2, 2, 4); /* 设置时钟, 480Mhz */
    delay_init(480);                    /* 延时初始化 */
    usart_init(120, 115200);            /* 串口初始化为115200 */
    led_init();                         /* 初始化LED */
    key_init();                         /* 初始化按键 */
    printf("\r\n\r\nMPU closed!\r\n");  /* 提示MPU关闭 */

    while (1)
    {
        key = key_scan(0);

        if (key == WKUP_PRES)           /* 使能MPU保护数组 mpudata */
        {
            mpu_set_protection(0X20002000, 128, 0, 0, MPU_REGION_PRIV_RO_URO, 0, 0, 1);	/* 只读,禁止共用,禁止catch,允许缓冲 */
            printf("MPU open!\r\n");    /* 提示MPU打开 */
        }
        else if (key == KEY0_PRES)      /* 向数组中写入数据，如果开启了MPU保护的话会进入内存访问错误！ */
        {
            printf("Start Writing data...\r\n");
            sprintf((char *)mpudata, "MPU test array %d", t);
            printf("Data Write finshed!\r\n");
        }
        else if (key == KEY1_PRES)      /* 从数组中读取数据，不管有没有开启MPU保护都不会进入内存访问错误！ */
        {
            printf("Array data is:%s\r\n", mpudata);
        }
        else 
        {
            delay_ms(10);
        }
        
        t++;

        if ((t % 50) == 0) LED0_TOGGLE();   /* LED0取反 */
    }
}












