/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-03-23
 * @brief       DMA 实验
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
#include "./BSP/DMA/dma.h"


const uint8_t TEXT_TO_SEND[] = {"正点原子 MiniPRO STM32H7 DMA 串口实验"};   /* 要循环发送的字符串 */

#define SEND_BUF_SIZE       (sizeof(TEXT_TO_SEND) + 2) * 200   /* 发送数据长度, 等于sizeof(TEXT_TO_SEND) + 2的200倍. */

uint8_t g_sendbuf[SEND_BUF_SIZE];       /* 发送数据缓冲区 */

int main(void)
{
    uint8_t  key = 0;
    uint16_t i, k;
    uint16_t len;
    uint8_t  mask = 0;
    float pro = 0;                      /* 进度 */

    sys_cache_enable();                 /* 打开L1-Cache */
    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(240, 2, 2, 4); /* 设置时钟, 480Mhz */
    delay_init(480);                    /* 延时初始化 */
    usart_init(115200);                 /* 串口初始化为115200 */
    mpu_memory_protection();            /* 保护相关存储区域 */
    led_init();                         /* 初始化LED */
    lcd_init();                         /* 初始化LCD */
    key_init();                         /* 初始化按键 */

    dma_init(DMA2_Stream7, DMA_REQUEST_USART1_TX);  /* 初始化DMA */

    lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
    lcd_show_string(30, 70, 200, 16, 16, "DMA TEST", RED);
    lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
    lcd_show_string(30, 110, 200, 16, 16, "KEY0:Start", RED);

    len = sizeof(TEXT_TO_SEND);
    k = 0;
    
    for (i = 0; i < SEND_BUF_SIZE; i++) /* 填充ASCII字符集数据 */
    {
        if (k >= len)   /* 入换行符 */
        {
            if (mask)
            {
                g_sendbuf[i] = 0x0a;
                k = 0;
            }
            else
            {
                g_sendbuf[i] = 0x0d;
                mask++;
            }
        }
        else     /* 复制TEXT_TO_SEND语句 */
        {
            mask = 0;
            g_sendbuf[i] = TEXT_TO_SEND[k];
            k++;
        }
    }
 
    i = 0;

    while (1)
    {
        key = key_scan(0);

        if (key == KEY0_PRES)   /* KEY0按下 */
        {
            printf("\r\nDMA DATA:\r\n");
            lcd_show_string(30, 130, 200, 16, 16, "Start Transimit....", BLUE);
            lcd_show_string(30, 150, 200, 16, 16, "   %", BLUE);    /* 显示百分号 */

            HAL_UART_Transmit_DMA(&g_uart1_handle,g_sendbuf,SEND_BUF_SIZE); /* 开始一次DMA传输！ */

            /* 等待DMA传输完成，此时我们来做另外一些事情，比如点灯  
             * 实际应用中，传输数据期间，可以执行另外的任务 
             */
            while (1)
            {
                if (__HAL_DMA_GET_FLAG(&g_dma_handle, DMA_FLAG_TCIF3_7)) /* 等待DMA2_Stream7传输完成 */
                {
                    __HAL_DMA_CLEAR_FLAG(&g_dma_handle, DMA_FLAG_TCIF3_7); /* 清除DMA2_Stream7传输完成标志 */
                    HAL_UART_DMAStop(&g_uart1_handle);      /* 传输完成以后关闭串口DMA */
                    break;
                }

                pro = __HAL_DMA_GET_COUNTER(&g_dma_handle);   /* 得到当前还剩余多少个数据 */
                len = SEND_BUF_SIZE;        /* 总长度 */
                pro = 1 - (pro / len);      /* 得到百分比 */
                pro *= 100;                 /* 扩大100倍 */
                lcd_show_num(30, 150, pro, 3, 16, BLUE);
            }
            lcd_show_num(30, 150, 100, 3, 16, BLUE);    /* 显示100% */
            lcd_show_string(30, 130, 200, 16, 16, "Transimit Finished!", BLUE); /* 提示传送完成 */
        }

        i++;
        delay_ms(10);

        if (i == 20)
        {
            LED0_TOGGLE();  /* LED0闪烁,提示系统正在运行 */
            i = 0;
        }
    }
}












