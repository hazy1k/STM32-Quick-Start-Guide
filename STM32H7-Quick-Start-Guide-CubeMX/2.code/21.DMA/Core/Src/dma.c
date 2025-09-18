#include "dma.h"
#include "usart.h"

DMA_HandleTypeDef dma_handle;
extern UART_HandleTypeDef huart1;

void MX_DMA_Init(DMA_Stream_TypeDef *dma_stream_handle, uint32_t ch)
{
  // 启用DMA时钟
  __HAL_RCC_DMA2_CLK_ENABLE();

  // 初始化DMA句柄
  dma_handle.Instance = dma_stream_handle;
  dma_handle.Init.Request = ch;
  dma_handle.Init.Direction = DMA_MEMORY_TO_PERIPH;
  dma_handle.Init.PeriphInc = DMA_PINC_DISABLE;
  dma_handle.Init.MemInc = DMA_MINC_ENABLE;
  dma_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  dma_handle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
  dma_handle.Init.Mode = DMA_NORMAL;
  dma_handle.Init.Priority = DMA_PRIORITY_MEDIUM;
  dma_handle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
  dma_handle.Init.MemBurst = DMA_MBURST_SINGLE;
  dma_handle.Init.PeriphBurst = DMA_PBURST_SINGLE;

  // 初始化DMA
  HAL_DMA_DeInit(&dma_handle);
  if (HAL_DMA_Init(&dma_handle) != HAL_OK)
  {
    Error_Handler();
  }

  // 链接DMA到USART
  __HAL_LINKDMA(&huart1, hdmatx, dma_handle);

  // 启用DMA传输完成中断
  __HAL_DMA_ENABLE_IT(&dma_handle, DMA_IT_TC);

  // 设置DMA中断优先级并启用
  HAL_NVIC_SetPriority(DMA2_Stream7_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream7_IRQn);
}

