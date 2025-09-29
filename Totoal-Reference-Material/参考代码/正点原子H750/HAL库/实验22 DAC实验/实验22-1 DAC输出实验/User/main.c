/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-03-28
 * @brief       DAC输出 实验
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
#include "./BSP/ADC/adc.h"
#include "./BSP/DAC/dac.h"


extern ADC_HandleTypeDef g_adc_handle; /* ADC句柄 */

int main(void)
{
    uint32_t adcx;
    float temp;
    uint8_t t = 0;
    uint16_t dacval = 0;
    uint8_t key;

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
    adc_init();                         /* 初始化ADC */
    dac_init(1);                        /* 初始化DAC1_OUT1通道 */

    lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
    lcd_show_string(30, 70, 200, 16, 16, "DAC TEST", RED);
    lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
    lcd_show_string(30, 110, 200, 16, 16, "WK_UP:+  KEY1:-", RED);

    lcd_show_string(30, 150, 200, 16, 16, "DAC VAL:", BLUE);
    lcd_show_string(30, 170, 200, 16, 16, "DAC VOL:0.000V", BLUE);
    lcd_show_string(30, 190, 200, 16, 16, "ADC VOL:0.000V", BLUE);

    HAL_DAC_SetValue(&g_dac_handle, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 0);/* 初始值为0 */

    while (1)
    {
        t++;
        key = key_scan(0);          /* 按键扫描 */

        if (key == WKUP_PRES)
        {
            if (dacval < 4000)dacval += 200;

            HAL_DAC_SetValue(&g_dac_handle, DAC_CHANNEL_1, DAC_ALIGN_12B_R, dacval);/* 输出增大200 */
        }
        else if (key == KEY1_PRES)
        {
            if (dacval > 200)dacval -= 200;
            else dacval = 0;

            HAL_DAC_SetValue(&g_dac_handle, DAC_CHANNEL_1, DAC_ALIGN_12B_R, dacval);/* 输出减少200 */
        }

        if (t == 10 || key == KEY1_PRES || key == WKUP_PRES)        /* WKUP/KEY1按下了,或者定时时间到了 */
        {
            adcx = HAL_DAC_GetValue(&g_dac_handle, DAC_CHANNEL_1);  /* 读取前面设置DAC1_OUT1的值 */
            lcd_show_xnum(94, 150, adcx, 4, 16, 0, BLUE);           /* 显示DAC寄存器值 */
            
            temp = (float)adcx * (3.3 / 4096);                      /* 得到DAC电压值 */
            adcx = temp;
            lcd_show_xnum(94, 170, temp, 1, 16, 0, BLUE);           /* 显示电压值整数部分 */
            
            temp -= adcx;
            temp *= 1000;
            lcd_show_xnum(110, 170, temp, 3, 16, 0X80, BLUE);       /* 显示电压值的小数部分 */
            
            adcx=adc_get_result_average(ADC_ADCX_CHY, 10);          /* 得到ADC通道19的转换结果 */
            temp = (float)adcx * (3.3 / 65536);                     /* 得到ADC电压值(adc是16bit的) */
            adcx = temp;
            lcd_show_xnum(94, 190, temp, 1, 16, 0, BLUE);           /* 显示电压值整数部分 */
            
            temp -= adcx;
            temp *= 1000;
            lcd_show_xnum(110, 190, temp, 3, 16, 0X80, BLUE);       /* 显示电压值的小数部分 */
            
            LED0_TOGGLE();  /* LED0闪烁 */
            t = 0;
        }

        delay_ms(10);
    }
}







