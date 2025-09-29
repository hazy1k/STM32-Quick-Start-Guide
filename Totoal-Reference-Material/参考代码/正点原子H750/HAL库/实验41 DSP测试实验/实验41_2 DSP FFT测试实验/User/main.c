/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-05
 * @brief       DSP FFT 实验
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
#include "./BSP/TIMER/btim.h"
#include "./CMSIS/DSP/Include/arm_math.h"


/* FFT长度，默认是1024点FFT 
 * 可选范围为: 16, 64, 256, 1024.
 */
#define FFT_LENGTH      1024


float fft_inputbuf[FFT_LENGTH * 2];     /* FFT输入数组 */
float fft_outputbuf[FFT_LENGTH];        /* FFT输出数组 */

uint8_t g_timeout;

int main(void)
{
    float time;
    char buf[50];
    arm_cfft_radix4_instance_f32 scfft;
    uint8_t key, t = 0;
    uint16_t i;

    sys_cache_enable();                 /* 打开L1-Cache */
    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(240, 2, 2, 4); /* 设置时钟, 480Mhz */
    delay_init(480);                    /* 延时初始化 */
    usart_init(115200);                 /* 串口初始化为115200 */
    mpu_memory_protection();            /* 保护相关存储区域 */
    led_init();                         /* 初始化LED */
    lcd_init();                         /* 初始化LCD */
    key_init();                         /* 初始化按键 */
    btim_timx_int_init(65535, 240 - 1); /* 1Mhz计数频率,最大计时65ms溢出 */

    lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
    lcd_show_string(30, 70, 200, 16, 16, "DSP FFT TEST", RED);
    lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
    lcd_show_string(30, 130, 200, 16, 16, "KEY0:Run FFT", RED); /* 显示提示信息 */
    lcd_show_string(30, 160, 200, 16, 16, "FFT runtime:", RED); /* 显示提示信息 */

    arm_cfft_radix4_init_f32(&scfft, FFT_LENGTH, 0, 1); /* 初始化scfft结构体，设定FFT相关参数 */

    while (1)
    {
        key = key_scan(0);

        if (key == KEY0_PRES)
        {
            for (i = 0; i < FFT_LENGTH; i++)   /* 生成信号序列 */
            {
                fft_inputbuf[2 * i] = 100 +
                                      10 * arm_sin_f32(2 * PI * i / FFT_LENGTH) +
                                      30 * arm_sin_f32(2 * PI * i * 4 / FFT_LENGTH) +
                                      50 * arm_cos_f32(2 * PI * i * 8 / FFT_LENGTH);    /* 生成输入信号实部 */
                fft_inputbuf[2 * i + 1] = 0; /* 虚部全部为0 */
            }

            BTIM_TIMX_INT->CNT = 0;; /* 重设BTIM_TIMX_INT定时器的计数器值 */
            g_timeout = 0;

            arm_cfft_radix4_f32(&scfft, fft_inputbuf);                      /* FFT计算（基4） */
            
            time = BTIM_TIMX_INT->CNT + (uint32_t)g_timeout * 65536;        /* 计算所用时间 */
            sprintf((char *)buf, "%0.3fms\r\n", time / 1000);
            lcd_show_string(30 + 12 * 8, 160, 100, 16, 16, buf, BLUE);      /* 显示运行时间 */
            
            arm_cmplx_mag_f32(fft_inputbuf, fft_outputbuf, FFT_LENGTH);     /* 把运算结果复数求模得幅值 */
            
            printf("\r\n%d point FFT runtime:%0.3fms\r\n", FFT_LENGTH, time / 1000);
            printf("FFT Result:\r\n");

            for (i = 0; i < FFT_LENGTH; i++)
            {
                printf("fft_outputbuf[%d]:%f\r\n", i, fft_outputbuf[i]);
            }
        }
        else
        {
            delay_ms(10);
        }
        
        t++;

        if ((t % 20) == 0)
        {
            LED0_TOGGLE();
        }
    }
}















