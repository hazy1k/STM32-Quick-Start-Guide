#include "dma.h"
#include "usart.h"

extern UART_HandleTypeDef g_uart1_handle;
DMA_HandleTypeDef dma_handle;

// 串口dma传输初始化（存储器->外设）
void dam_uart_init(DMA_Stream_TypeDef *dma_stream_handle, uint32_t ch)
{
    if((uint32_t)dma_stream_handle > (uint32_t)DMA2)
    {
        __HAL_RCC_DMA2_CLK_ENABLE();
    }
    else
    {
        __HAL_RCC_DMA1_CLK_ENABLE();
    }
    __HAL_LINKDMA(&g_uart1_handle, hdmatx, dma_handle); // 将DMA和USART1绑定
    /*TX DMA configuration*/
    dma_handle.Instance = dma_stream_handle; // 数据流选择
    dma_handle.Init.Channel = ch; // 通道选择
    dma_handle.Init.Direction = DMA_MEMORY_TO_PERIPH; // 传输方向：存储器到外设
    dma_handle.Init.PeriphInc = DMA_PINC_DISABLE; // 外设地址不增
    dma_handle.Init.MemInc = DMA_MINC_ENABLE; // 存储器地址增
    dma_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    dma_handle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    dma_handle.Init.Mode = DMA_NORMAL; // 正常模式
    dma_handle.Init.Priority = DMA_PRIORITY_LOW; // 优先级低
    dma_handle.Init.FIFOMode = DMA_FIFOMODE_DISABLE; // 禁用FIFO模式
    dma_handle.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL; // 传输阈值
    dma_handle.Init.MemBurst = DMA_MBURST_SINGLE; // 单次传输
    dma_handle.Init.PeriphBurst = DMA_PBURST_SINGLE; // 单次传输
    HAL_DMA_DeInit(&dma_handle); // 复位DMA
    HAL_DMA_Init(&dma_handle);
}
