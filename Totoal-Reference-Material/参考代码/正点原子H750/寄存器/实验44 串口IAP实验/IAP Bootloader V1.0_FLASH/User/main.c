/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-05
 * @brief       串口IAP 实验-IAP Bootloader V1.0_FLASH
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



/* 例程说明:
 * 本例程用于将IAP Bootloader V1.0_ITCM工程生产的.c数组(由.bin文件生成),存放在QSPI FLASH
 * 然后把QSPI FLASH的这个代码加载到ITCM(0X0000 0000开始,64KB大小)里面,然后跳转到ITCM运行,
 * 执行:IAP Bootloader V1.0_ITCM,完成BootLoader功能!
 */


#define BOOTLOADER_RUN_ADDR     0x00000000      /* Bootloader运行地址,即ITCM首地址 */


typedef  void (*iapfun)(void);          /* 定义一个函数类型的参数 */
iapfun jump2app;                        /* 假函数,用于跳转 */

extern const unsigned char acApp2[];    /* 在bl_itcm.c里面定义,存放IAP Bootloader V1.0_ITCM固件数组 */
extern uint32_t bl_itcm_size;           /* 在bl_itcm.c里面定义,存放IAP Bootloader V1.0_ITCM固件大小 */


/**
 * @brief       跳转到应用程序段
 * @param       appxaddr : 用户代码起始地址
 * @retval      无
 */
void iap_load_app(uint32_t appxaddr)
{
    /* 检查栈顶地址是否合法.ITCM BootLoader的内存放在DTCM里面 */
    if (((*(volatile  uint32_t *)appxaddr) & 0x2FF00000) == 0x20000000)
    {
        jump2app = (iapfun) * (volatile uint32_t *)(appxaddr + 4);  /* 用户代码区第二个字为程序开始地址(复位地址) */
        sys_msr_msp(*(volatile uint32_t *)appxaddr);                /* 初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址) */
        jump2app();                                                 /* 跳转到APP */
    }
}


int main(void)
{
    volatile uint8_t *pbr = BOOTLOADER_RUN_ADDR; /* 指针指向Bootloader运行首地址 */
    uint32_t i = 0;

    sys_stm32_clock_init(240, 2, 2, 4); /* 设置时钟, 480Mhz */
    delay_init(480);                    /* 延时初始化 */
    usart_init(120, 115200);            /* 串口初始化为115200 */
    led_init();                         /* 初始化LED */
    mpu_memory_protection();            /* 保护相关存储区域 */
    printf("ITCM BootLoader size is:%d\r\n", bl_itcm_size);

    for (i = 0; i < bl_itcm_size; i++)
    {
        pbr[i] = acApp2[i]; /* 搬运数据到BOOTLOADER_RUN_ADDR */
    }

    if (((*(volatile uint32_t *)(BOOTLOADER_RUN_ADDR + 4)) & 0xFF000000) == 0x00000000) /* 判断是否为0X00XXXXXX */
    {
        printf("Run ITCM BootLoader...\r\n\r\n");
        delay_ms(10);
        iap_load_app(BOOTLOADER_RUN_ADDR);  /* 运行ITCM BootLoader */
    }
    else
    {
        printf("ITCM BootLoader addr error!\r\n");
    }

    while (1)
    {
        printf("Error!\r\n");
        LED0_TOGGLE();          /* LED0闪烁 */
        delay_ms(500);
    }
}




















