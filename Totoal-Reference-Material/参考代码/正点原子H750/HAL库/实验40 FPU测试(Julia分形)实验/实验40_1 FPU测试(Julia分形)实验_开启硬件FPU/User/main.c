/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-05
 * @brief       FPU测试(Julia分形) 实验
 *   @note      40_1, 本版本为开启硬件FPU版本
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
#include "./BSP/KEY/key.h"
#include "./BSP/TIMER/btim.h"


/* FPU模式提示 */
#if __FPU_USED==1
#define SCORE_FPU_MODE                  "FPU On"
#else
#define SCORE_FPU_MODE                  "FPU Off"
#endif

#define     ITERATION           128			/* 迭代次数 */
#define     REAL_CONSTANT       0.285f		/* 实部常量 */
#define     IMG_CONSTANT        0.01f		/* 虚部常量 */

/* 颜色表 */
uint16_t g_color_map[ITERATION];

/* 缩放因子列表 */
const uint16_t zoom_ratio[] =
{
    120, 110, 100, 150, 200, 275, 350, 450,
    600, 800, 1000, 1200, 1500, 2000, 1500,
    1200, 1000, 800, 600, 450, 350, 275, 200,
    150, 100, 110,
};

/**
 * @brief       初始化颜色表
 * @param       clut     : 颜色表指针
 * @retval      无
 */
void julia_clut_init(uint16_t *clut)
{
    uint32_t i = 0x00;
    uint16_t  red = 0, green = 0, blue = 0;

    for (i = 0; i < ITERATION; i++) /* 产生颜色表 */
    {
        /* 产生RGB颜色值 */
        red = (i * 8 * 256 / ITERATION) % 256;
        green = (i * 6 * 256 / ITERATION) % 256;
        blue = (i * 4 * 256 / ITERATION) % 256;
        
        /* 将RGB888,转换为RGB565 */
        red = red >> 3;
        red = red << 11;
        green = green >> 2;
        green = green << 5;
        blue = blue >> 3;
        clut[i] = red + green + blue;
    }
}

/* RGB LCD 缓存*/
uint16_t g_lcdbuf[800];

/**
 * @brief        产生Julia分形图形
 * @param       size_x   : 屏幕x方向的尺寸
 * @param       size_y   : 屏幕y方向的尺寸
 * @param       offset_x : 屏幕x方向的偏移
 * @param       offset_y : 屏幕y方向的偏移
 * @param       zoom     : 缩放因子
 * @retval      无
 */
void julia_generate_fpu(uint16_t size_x, uint16_t size_y, uint16_t offset_x, uint16_t offset_y, uint16_t zoom)
{
    uint8_t i;
    uint16_t x, y;
    float tmp1, tmp2;
    float num_real, num_img;
    float radius;

    for (y = 0; y < size_y; y++)
    {
        for (x = 0; x < size_x; x++)
        {
            num_real = y - offset_y;
            num_real = num_real / zoom;
            num_img = x - offset_x;
            num_img = num_img / zoom;
            i = 0;
            radius = 0;

            while ((i < ITERATION - 1) && (radius < 4))
            {
                tmp1 = num_real * num_real;
                tmp2 = num_img * num_img;
                num_img = 2 * num_real * num_img + IMG_CONSTANT;
                num_real = tmp1 - tmp2 + REAL_CONSTANT;
                radius = tmp1 + tmp2;
                i++;
            }

            LCD->LCD_RAM = g_color_map[i]; /* 绘制到屏幕 */
        }
    }
}

uint8_t g_timeout;

int main(void)
{
    uint8_t key;
    uint8_t i = 0;
    uint8_t autorun = 0;
    float time;
    char buf[50];

    sys_cache_enable();                 /* 打开L1-Cache */
    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(240, 2, 2, 4); /* 设置时钟, 480Mhz */
    delay_init(480);                    /* 延时初始化 */
    usart_init(115200);                 /* 串口初始化为115200 */
    mpu_memory_protection();            /* 保护相关存储区域 */
    led_init();                         /* 初始化LED */
    key_init();                         /* 初始化按键 */
    lcd_init();                         /* 初始化LCD */
    btim_timx_int_init(65535, 24000 - 1);   /* 10Khz计数频率,最大计时6.5秒超出 */
    
    lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
    lcd_show_string(30, 70, 200, 16, 16, "FPU TEST", RED);
    lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
    lcd_show_string(30, 110, 200, 16, 16, "KEY0:+    KEY1:-", RED);     /* 显示提示信息 */
    lcd_show_string(30, 130, 200, 16, 16, "KEY_UP:AUTO/MANUL", RED);    /* 显示提示信息 */
    delay_ms(1200);
    
    julia_clut_init(g_color_map);       /* 初始化颜色表 */

    while (1)
    {
        key = key_scan(0);

        switch (key)
        {
            case KEY0_PRES:
                i++;

                if (i > sizeof(zoom_ratio) / 2 - 1)i = 0; /* 限制范围 */

                break;

            case KEY1_PRES:
                if (i)i--;
                else i = sizeof(zoom_ratio) / 2 - 1;

                break;

            case WKUP_PRES:
                autorun = !autorun; /* 自动/手动 */
                break;
        }

        if (autorun == 1)   /* 自动时,自动设置缩放因子 */
        {
            i++;

            if (i > sizeof(zoom_ratio) / 2 - 1)
            {
                i = 0;      /* 限制范围 */
            }
        }

        lcd_set_window(0, 0, lcddev.width, lcddev.height);  /* 设置窗口 */
        lcd_write_ram_prepare();
        
        BTIM_TIMX_INT->CNT = 0;  /* 重设TIM3定时器的计数器值 */
        g_timeout = 0;
        
        julia_generate_fpu(lcddev.width, lcddev.height, lcddev.width / 2, lcddev.height / 2, zoom_ratio[i]);
        
        time = BTIM_TIMX_INT->CNT + (uint32_t)g_timeout * 65536;
        
        sprintf(buf, "%s: zoom:%d  runtime:%0.1fms\r\n", SCORE_FPU_MODE, zoom_ratio[i], time / 10);
        lcd_show_string(5, lcddev.height - 5 - 12, lcddev.width - 5, 12, 12, buf, RED); /* 显示当前运行情况 */
        printf("%s", buf); /* 输出到串口 */
        LED0_TOGGLE();
    }
}


















