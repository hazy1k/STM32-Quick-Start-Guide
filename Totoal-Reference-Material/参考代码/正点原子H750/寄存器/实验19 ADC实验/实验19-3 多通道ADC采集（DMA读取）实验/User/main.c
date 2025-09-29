/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-03-23
 * @brief       多通道ADC采集(DMA读取) 实验
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


#define ADC_DMA_BUF_SIZE        50 * 6      /* ADC DMA采集 BUF大小, 应等于ADC通道数的整数倍 */
uint16_t g_adc_dma_buf[ADC_DMA_BUF_SIZE];   /* ADC DMA BUF */

extern uint8_t g_adc_dma_sta;               /* DMA传输状态标志, 0,未完成; 1, 已完成 */

int main(void)
{
    uint16_t i,j;
    uint16_t adcx;
    uint32_t sum;
    float temp;

    sys_stm32_clock_init(240, 2, 2, 4); /* 设置时钟, 480Mhz */
    delay_init(480);                    /* 延时初始化 */
    usart_init(120, 115200);            /* 串口初始化为115200 */
    mpu_memory_protection();            /* 保护相关存储区域 */
    led_init();                         /* 初始化LED */
    lcd_init();                         /* 初始化LCD */

    adc_nch_dma_init((uint32_t)&ADC1->DR, (uint32_t)&g_adc_dma_buf);    /* 初始化ADC DMA采集 */


    lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
    lcd_show_string(30, 70, 200, 16, 16, "ADC DMA TEST", RED);
    lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);

    lcd_show_string(30, 130, 200, 12, 12, "ADC1_CH14_VAL:", BLUE);
    lcd_show_string(30, 142, 200, 12, 12, "ADC1_CH14_VOL:0.000V", BLUE); /* 先在固定位置显示小数点 */
    
    lcd_show_string(30, 160, 200, 12, 12, "ADC1_CH15_VAL:", BLUE);
    lcd_show_string(30, 172, 200, 12, 12, "ADC1_CH15_VOL:0.000V", BLUE); /* 先在固定位置显示小数点 */

    lcd_show_string(30, 190, 200, 12, 12, "ADC1_CH16_VAL:", BLUE);
    lcd_show_string(30, 202, 200, 12, 12, "ADC1_CH16_VOL:0.000V", BLUE); /* 先在固定位置显示小数点 */

    lcd_show_string(30, 220, 200, 12, 12, "ADC1_CH17_VAL:", BLUE);
    lcd_show_string(30, 232, 200, 12, 12, "ADC1_CH17_VOL:0.000V", BLUE); /* 先在固定位置显示小数点 */

    lcd_show_string(30, 250, 200, 12, 12, "ADC1_CH18_VAL:", BLUE);
    lcd_show_string(30, 262, 200, 12, 12, "ADC1_CH18_VOL:0.000V", BLUE); /* 先在固定位置显示小数点 */

    lcd_show_string(30, 280, 200, 12, 12, "ADC1_CH19_VAL:", BLUE);
    lcd_show_string(30, 292, 200, 12, 12, "ADC1_CH19_VOL:0.000V", BLUE); /* 先在固定位置显示小数点 */

    adc_dma_enable(ADC_DMA_BUF_SIZE);   /* 启动ADC DMA采集 */

    while (1)
    {
        if (g_adc_dma_sta == 1)
        {
            /* 清除D Cache数据 */
            SCB_InvalidateDCache();

            /* 循环显示通道14~通道19的结果 */
            for(j = 0; j < 6; j++)  /* 遍历6个通道 */
            {
                sum = 0; /* 清零 */
                for (i = 0; i < ADC_DMA_BUF_SIZE / 6; i++)  /* 每个通道采集了50次数据,进行50次累加 */
                {
                    sum += g_adc_dma_buf[(6 * i) + j];  /* 相同通道的转换数据累加 */
                }
                adcx = sum / (ADC_DMA_BUF_SIZE / 6);    /* 取平均值 */
                
                /* 显示结果 */
                lcd_show_xnum(114, 130 + (j * 30), adcx, 5, 12, 0, BLUE);   /* 显示ADCC采样后的原始值 */

                temp = (float)adcx * (3.3 / 65536); /* 获取计算后的带小数的实际电压值，比如3.1111 */
                adcx = temp;    /* 赋值整数部分给adcx变量，因为adcx为u16整形 */
                lcd_show_xnum(114, 142 + (j * 30), adcx, 1, 12, 0, BLUE);   /* 显示电压值的整数部分，3.1111的话，这里就是显示3 */

                temp -= adcx;   /* 把已经显示的整数部分去掉，留下小数部分，比如3.1111-3=0.1111 */
                temp *= 1000;   /* 小数部分乘以1000，例如：0.1111就转换为111.1，相当于保留三位小数。 */
                lcd_show_xnum(126, 142 + (j * 30), temp, 3, 12, 0X80, BLUE);/* 显示小数部分（前面转换为了整形显示），这里显示的就是111. */
            }
 
            g_adc_dma_sta = 0;  /* 清除DMA采集完成状态标志 */
            adc_dma_enable(ADC_DMA_BUF_SIZE);   /* 启动下一次ADC DMA采集 */
        }

        LED0_TOGGLE();
        delay_ms(100);
    }
}


















