#include "bsp_init.h"
#include "stdio.h"
#include "dma.h"

const uint8_t STRING_TO_SEND[] = "This STM32F407 DMA Transfer Test"; 
#define SEND_BUFF_SIZE ((sizeof(STRING_TO_SEND)+2)*200)
uint8_t send_buff[SEND_BUFF_SIZE];

int main(void)
{
    uint8_t key = 0;
    uint16_t i = 0, j = 0, len, remain;
    uint8_t mask = 0;
    float pb = 0;
    bsp_init();
    LCD_ShowString(30,50,200,16,16,"STM32 DMA Transfer Test");

    // 只初始化一次
    dam_uart_init(DMA2_Stream7, DMA_CHANNEL_4);

    // 填充数据
    len = sizeof(STRING_TO_SEND) - 1; //实际内容长度，不包含\0
    j = 0;
    for(i = 0; i < SEND_BUFF_SIZE; i++)
    {
        if(j >= len)
        {
            if(mask == 0)
            {
                send_buff[i] = 0x0d;
                mask++;
            }
            else
            {
                send_buff[i] = 0x0a;
                mask = 0;
                j = 0;
            }
        }
        else
        {
            send_buff[i] = STRING_TO_SEND[j];
            j++;
            mask = 0;
        }
    }

    while(1)
    {
        key = key_scan(0);
        if(key == KEY0_Press)
        {
            printf("\r\nDMA DATA:\r\n");
            LCD_ShowString(30,130,200,16,16, "Start DMA Transfer...");
            LCD_ShowString(30,150,200,16,16,"   %");

            // 启动DMA传输
            HAL_UART_Transmit_DMA(&g_uart1_handle, send_buff, SEND_BUFF_SIZE);

            // 等待传输完成
            while(__HAL_DMA_GET_COUNTER(&dma_handle) != 0) // DMA还未完成
            {
                remain = __HAL_DMA_GET_COUNTER(&dma_handle);
                pb = 1.0f - ((float)remain / SEND_BUFF_SIZE);
                LCD_ShowNum(30,150,(int)(pb*100),3,16);
            }

            // DMA完成，清标志
            __HAL_DMA_CLEAR_FLAG(&dma_handle, DMA_FLAG_TCIF3_7);
            LCD_ShowNum(30,150,100,3,16);
            LCD_ShowString(30,130,200,16,16, "DMA Transfer Complete!");
            HAL_UART_DMAStop(&g_uart1_handle); // 释放DMA
        }

        // 可控制LED闪烁等
        i++;
        if(i % 10 == 0)
        {
          LED_TOGGLE(LED0_GPIO_Pin);
        }
        delay_ms(10);
    }
}
