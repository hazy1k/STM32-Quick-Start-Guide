/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-05
 * @brief       手写识别 实验
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
#include "./MALLOC/malloc.h"
#include "./FATFS/exfuns/exfuns.h"
#include "./TEXT/text.h"
#include "./BSP/MPU/mpu.h"
#include "./BSP/LED/led.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/KEY/key.h"
#include "./BSP/SDMMC/sdmmc_sdcard.h"
#include "./BSP/NORFLASH/norflash_ex.h"
#include "./BSP/TOUCH/touch.h"
#include "./ATKNCR/atk_ncr.h"


/* 最大记录的轨迹点数(输入数据) */
atk_ncr_point ncr_input_buf[200];


/**
 * @brief       画粗线
 * @param       x1,y1: 起点坐标
 * @param       x2,y2: 终点坐标
 * @param       size : 线条粗细程度
 * @param       color: 线的颜色
 * @retval      无
 */
void lcd_draw_bline(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t size, uint16_t color)
{
    uint16_t t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, row, col;

    if (x1 < size || x2 < size || y1 < size || y2 < size) return;

    delta_x = x2 - x1;  /* 计算坐标增量 */
    delta_y = y2 - y1;
    row = x1;
    col = y1;

    if (delta_x > 0) incx = 1;       /* 设置单步方向 */
    else if (delta_x == 0) incx = 0; /* 垂直线 */
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }

    if (delta_y > 0)
    {
        incy = 1;
    }
    else if (delta_y == 0)
    {
        incy = 0;   /* 水平线 */
    }
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }

    if ( delta_x > delta_y) distance = delta_x; /* 选取基本增量坐标轴 */
    else distance = delta_y;

    for (t = 0; t <= distance + 1; t++ )    /* 画线输出 */
    {
        lcd_fill_circle(row, col, size, color); /* 画点 */
        xerr += delta_x ;
        yerr += delta_y ;

        if (xerr > distance)
        {
            xerr -= distance;
            row += incx;
        }

        if (yerr > distance)
        {
            yerr -= distance;
            col += incy;
        }
    }
}

int main(void)
{
    uint8_t i = 0;
    uint8_t tcnt = 0;
    char sbuf[10];
    uint8_t key;
    uint16_t pcnt = 0;
    uint8_t mode = 4;                   /* 默认是混合模式 */
    uint16_t lastpos[2];                /* 最后一次的数据 */

    sys_cache_enable();                 /* 打开L1-Cache */
    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(240, 2, 2, 4); /* 设置时钟, 480Mhz */
    delay_init(480);                    /* 延时初始化 */
    usart_init(115200);                 /* 串口初始化为115200 */
    mpu_memory_protection();            /* 保护相关存储区域 */
    led_init();                         /* 初始化LED */
    lcd_init();                         /* 初始化LCD */
    key_init();                         /* 初始化按键 */
    tp_dev.init();                      /* 初始化触摸屏 */
    my_mem_init(SRAMIN);                /* 初始化内部内存池(AXI) */
    my_mem_init(SRAM12);                /* 初始化SRAM12内存池(SRAM1+SRAM2) */
    my_mem_init(SRAM4);                 /* 初始化SRAM4内存池(SRAM4) */
    my_mem_init(SRAMDTCM);              /* 初始化DTCM内存池(DTCM) */
    my_mem_init(SRAMITCM);              /* 初始化ITCM内存池(ITCM) */
    exfuns_init();                      /* 为fatfs相关变量申请内存 */
    f_mount(fs[0], "0:", 1);            /* 挂载SD卡 */
    f_mount(fs[1], "1:", 1);            /* 挂载FLASH */

    alientek_ncr_init();                /* 初始化手写识别 */

    while (fonts_init())                /* 检查字库 */
    {
        lcd_show_string(60, 50, 200, 16, 16, "Font Error!", RED);
        delay_ms(200);
        lcd_fill(60, 50, 240, 66, WHITE); /* 清除显示 */
        delay_ms(200);
    }

RESTART:
    text_show_string(60, 10, 200, 16, "正点原子STM32开发板", 16, 0, RED);
    text_show_string(60, 30, 200, 16, "手写识别实验", 16, 0, RED);
    text_show_string(60, 50, 200, 16, "ATOM@ALIENTEK", 16, 0, RED);
    text_show_string(60, 70, 200, 16, "KEY0:MODE KEY1:Adjust", 16, 0, RED);
    text_show_string(60, 90, 200, 16, "识别结果:", 16, 0, RED);
    lcd_draw_rectangle(19, 114, lcddev.width - 20, lcddev.height - 5, RED);

    text_show_string(96, 207, 200, 16, "手写区", 16, 0, BLUE);
    tcnt = 100;

    while (1)
    {
        key = key_scan(0);

        if (key == KEY1_PRES && (tp_dev.touchtype & 0X80) == 0)
        {
            tp_adjust();        /* 屏幕校准 */
            lcd_clear(WHITE);
            goto RESTART;       /* 重新加载界面 */
        }

        if (key == KEY0_PRES)
        {
            lcd_fill(20, 115, 219, 314, WHITE); /* 清除当前显示 */
            mode++;

            if (mode > 4)mode = 1;

            switch (mode)
            {
                case 1:
                    text_show_string(80, 207, 200, 16, "仅识别数字", 16, 0, BLUE);
                    break;

                case 2:
                    text_show_string(64, 207, 200, 16, "仅识别大写字母", 16, 0, BLUE);
                    break;

                case 3:
                    text_show_string(64, 207, 200, 16, "仅识别小写字母", 16, 0, BLUE);
                    break;

                case 4:
                    text_show_string(88, 207, 200, 16, "全部识别", 16, 0, BLUE);
                    break;
            }

            tcnt = 100;
        }

        tp_dev.scan(0); /* 扫描 */

        if (tp_dev.sta & TP_PRES_DOWN)  /* 有按键被按下 */
        {
            delay_ms(1);    /* 必要的延时, 否则老认为有按键按下 */
            tcnt = 0;       /* 松开时的计数器清空 */

            if ((tp_dev.x[0] < (lcddev.width - 20 - 2) && tp_dev.x[0] >= (20 + 2)) && (tp_dev.y[0] < (lcddev.height - 5 - 2) && tp_dev.y[0] >= (115 + 2)))
            {
                if (lastpos[0] == 0XFFFF)
                {
                    lastpos[0] = tp_dev.x[0];
                    lastpos[1] = tp_dev.y[0];
                }

                lcd_draw_bline(lastpos[0], lastpos[1], tp_dev.x[0], tp_dev.y[0], 2, BLUE);  /* 画线 */
                lastpos[0] = tp_dev.x[0];
                lastpos[1] = tp_dev.y[0];

                if (pcnt < 200) /* 总点数少于200 */
                {
                    if (pcnt)
                    {
                        if ((ncr_input_buf[pcnt - 1].y != tp_dev.y[0]) && (ncr_input_buf[pcnt - 1].x != tp_dev.x[0])) /* x,y不相等 */
                        {
                            ncr_input_buf[pcnt].x = tp_dev.x[0];
                            ncr_input_buf[pcnt].y = tp_dev.y[0];
                            pcnt++;
                        }
                    }
                    else
                    {
                        ncr_input_buf[pcnt].x = tp_dev.x[0];
                        ncr_input_buf[pcnt].y = tp_dev.y[0];
                        pcnt++;
                    }
                }
            }
        }
        else    /* 按键松开了 */
        {
            lastpos[0] = 0XFFFF;
            tcnt++;
            delay_ms(10);
            /* 延时识别 */
            i++;

            if (tcnt == 40)
            {
                if (pcnt)   /* 有有效的输入 */
                {
                    printf("总点数:%d\r\n", pcnt);
                    alientek_ncr(ncr_input_buf, pcnt, 6, mode, sbuf);
                    printf("识别结果:%s\r\n", sbuf);
                    pcnt = 0; 
                    lcd_show_string(60 + 72, 90, 200, 16, 16, sbuf, BLUE);
                }

                lcd_fill(20, 115, lcddev.width - 20 - 1, lcddev.height - 5 - 1, WHITE);
            }
        }

        if (i == 30)
        {
            i = 0;
            LED0_TOGGLE();
        }
    }
}









