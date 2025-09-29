/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-03-28
 * @brief       DAC DMA输出正弦波 实验
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

#include "math.h"
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




uint16_t g_dac_sin_buf[4096];    /* 发送数据缓冲区 */


/**
 * @brief       产生正弦波序列
 *   @note      需保证: maxval > samples/2
 *
 * @param       maxval : 最大值(0 < maxval < 2048)
 * @param       samples: 采样点的个数
 *
 * @retval      无
 */
void dac_creat_sin_buf(uint16_t maxval, uint16_t samples)
{
    uint8_t i;
    float inc = (2 * 3.1415962) / samples;      /* 计算增量（一个周期DAC_SIN_BUF个点）*/
    float outdata = 0;

    for (i = 0; i < samples; i++)
    {
        outdata = maxval * (1 + sin(inc * i));  /* 计算以dots个点为周期的每个点的值，放大maxval倍，并偏移到正数区域 */
        if (outdata > 4095) outdata = 4095;     /* 上限限定 */ 
        //printf("%f\r\n",outdata);
        g_dac_sin_buf[i] = outdata;
    }
}

/**
 * @brief       通过USMART设置正弦波输出参数,方便修改输出频率.
 * @param       arr : TIM7的自动重装载值
 * @param       psc : TIM7的分频系数
 * @retval      无
 */
void dac_dma_sin_set(uint16_t arr, uint16_t psc)
{
    dac_dma_wave_enable(DMA2_Stream6, 100, arr, psc);
}

int main(void)
{
    uint16_t adcx;
    float temp;
    uint8_t t = 0;
    uint8_t key;

    sys_stm32_clock_init(240, 2, 2, 4); /* 设置时钟, 480Mhz */
    delay_init(480);                    /* 延时初始化 */
    usart_init(120, 115200);            /* 串口初始化为115200 */
    usmart_dev.init(240);               /* 初始化USMART */
    mpu_memory_protection();            /* 保护相关存储区域 */
    led_init();                         /* 初始化LED */
    lcd_init();                         /* 初始化LCD */
    key_init();                         /* 初始化按键 */
    adc_init();                         /* 初始化ADC */
    
    /* 初始化DAC通道1 DMA波形输出 */
    dac_dma_wave_init(1, DMA2_Stream6, 67, (uint32_t)&DAC1->DHR12R1, (uint32_t)g_dac_sin_buf);  

    lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
    lcd_show_string(30, 70, 200, 16, 16, "DAC DMA Sine WAVE TEST", RED);
    lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
    lcd_show_string(30, 110, 200, 16, 16, "KEY0:5Khz  KEY1:50Khz", RED);

    lcd_show_string(30, 130, 200, 16, 16, "DAC VAL:", BLUE);
    lcd_show_string(30, 150, 200, 16, 16, "DAC VOL:0.000V", BLUE);
    lcd_show_string(30, 170, 200, 16, 16, "ADC VOL:0.000V", BLUE);

    dac_creat_sin_buf(2048, 100);
    dac_dma_wave_enable(DMA2_Stream6, 100, 100 - 1, 24 - 1);

    while (1)
    {
        t++;
        key = key_scan(0);  /* 按键扫描 */

        if (key == KEY0_PRES)       /* 高采样率 , 约1Khz波形 */
        {
            dac_creat_sin_buf(2048, 100);
            dac_dma_wave_enable(DMA2_Stream6, 100, 20 - 1, 24 - 1); /* 500Khz触发频率, 100个点, 得到最高5KHz的正弦波. */
        }
        else if (key == KEY1_PRES)  /* 低采样率 , 约1Khz波形 */
        {
            dac_creat_sin_buf(2048, 10);
            dac_dma_wave_enable(DMA2_Stream6, 10, 20 - 1, 24 - 1);  /* 500Khz触发频率, 10个点, 可以得到最高50KHz的正弦波. */
        } 
        
        adcx = DAC1->DHR12R1;       /* 获取DAC1_OUT1的输出状态 */
        lcd_show_xnum(94, 130, adcx, 4, 16, 0, BLUE);       /* 显示DAC寄存器值 */
        
        temp = (float)adcx * (3.3 / 4096);                  /* 得到DAC电压值 */
        adcx = temp;
        lcd_show_xnum(94, 150, temp, 1, 16, 0, BLUE);       /* 显示电压值整数部分 */
        
        temp -= adcx;
        temp *= 1000;
        lcd_show_xnum(110, 150, temp, 3, 16, 0X80, BLUE);   /* 显示电压值的小数部分 */
        
        adcx = adc_get_result_average(ADC_ADCX_CHY, 20);    /* 得到ADC通道19的转换结果 */
        temp = (float)adcx * (3.3 / 65536);                 /* 得到ADC电压值(adc是16bit的) */
        adcx = temp;
        lcd_show_xnum(94, 170, temp, 1, 16, 0, BLUE);       /* 显示电压值整数部分 */
        
        temp -= adcx;
        temp *= 1000;
        lcd_show_xnum(110, 170, temp, 3, 16, 0X80, BLUE);   /* 显示电压值的小数部分 */
        
        if (t == 40)         /* 定时时间到了 */
        {
            LED0_TOGGLE();  /* LED0闪烁 */
            t = 0;
        }

        delay_ms(5);
    }
}














