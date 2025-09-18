#include "dma.h"

void USART_DMA_Init(void)
{
    DMA_InitTypeDef DMA_InitStructure;
    
    // 1. 使能DMA时钟
    RCC_AHBPeriphClockCmd(DMA_CLK, ENABLE);
    
    // 2. 配置DMA参数
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)USART_DR_ADDRESS;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)Send_DMA_BUFFER;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;  // 内存->外设
    DMA_InitStructure.DMA_BufferSize = DMA_BUFFER_SIZE;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; // 外设地址固定
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;         // 内存地址递增
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;       // 普通模式(单次传输)
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;        // 禁用内存到内存
    
    DMA_Init(USART_DMA_CHx, &DMA_InitStructure);
    
    // 3. 清除传输完成标志
    DMA_ClearFlag(DMA_TC_FLAG);
}

// 启动DMA传输
void DMA_StartTransfer(void)
{
    DMA_Cmd(USART_DMA_CHx, DISABLE);   // 先禁用通道
    DMA_SetCurrDataCounter(USART_DMA_CHx, DMA_BUFFER_SIZE); // 重置数据计数器
    DMA_Cmd(USART_DMA_CHx, ENABLE);    // 启用通道
}

// 检查传输是否完成
uint8_t DMA_IsTransferComplete(void)
{
    return (DMA_GetFlagStatus(DMA_TC_FLAG) == SET);
}