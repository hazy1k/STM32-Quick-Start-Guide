#include "stm32f10x.h"
#include "led.h"
#include "beep.h"
#include "SysTick.h"
#include "usart.h"
#include "dma.h"
#include <stdio.h>

// DMA要传输的数据-存储在内部的FLASH中
uint32_t Send_DMA_BUFFER[DMA_BUFFER_SIZE] = {0x11223344, 0x55667788, 0x99AABBCC, 0xDDCCEEFF};
// DMA接收到的数据-存储在内部的SRAM中
uint32_t Rece_DMA_BUFFER[DMA_BUFFER_SIZE];

// 判断两个数据是否相同
uint8_t Buffer_Compare(uint32_t *pBuffer1, uint32_t *pBuffer2, uint16_t BufferLength)
{
  while(BufferLength--)
  {
    if(*pBuffer1 != *pBuffer2)
    {
      return 0;
    }
    pBuffer1++;
    pBuffer2++;
  }
  return 1;
}

int main(void) 
{
  uint8_t Buffer_Result;
  SysTick_Init();
  BSP_LED_Init();
  BSP_Beep_Init();
  USARTx_Init();
  Delay_ms(2000);
  BSP_DMA_Init();
  
  while(DMA_GetFlagStatus(DMA_FLAG_TC) == RESET); // 等待DMA传输完成
  
  Buffer_Result = Buffer_Compare(Send_DMA_BUFFER, Rece_DMA_BUFFER, DMA_BUFFER_SIZE);
  
  if(Buffer_Result == 1)
  {
    LED_ON(BLUE_LED_Pin);
    BEEP_OFF();
    printf("DMA_Transfer_Complete\r\n");
    
    // 打印详细数据
    printf("Sent Buffer Address: %p\r\n", Send_DMA_BUFFER);
    printf("Sent Data:\t\t");
    for(int i = 0; i < DMA_BUFFER_SIZE; i++) {
        printf("%08X ", Send_DMA_BUFFER[i]);
    }
    
    printf("\r\nReceived Buffer Address: %p\r\n", Rece_DMA_BUFFER);
    printf("Received Data:\t");
    for(int i = 0; i < DMA_BUFFER_SIZE; i++) {
        printf("%08X ", Rece_DMA_BUFFER[i]);
    }
    printf("\r\n");
  }
  else
  {
    LED_ON(RED_LED_Pin);
    BEEP_ON();
    printf("DMA_Transfer_Error\r\n");
  }
  
  while(1);
}
