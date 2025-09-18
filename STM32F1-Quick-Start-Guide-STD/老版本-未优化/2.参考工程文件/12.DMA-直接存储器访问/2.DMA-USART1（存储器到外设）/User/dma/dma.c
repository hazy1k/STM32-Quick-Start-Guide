#include "dma.h"

uint8_t SendBuff[SendBuff_Size]; // 发送缓冲区

// 本工程是内存到外设（USART1->DR)
void USARTx_DMA_Init(void)
{
    DMA_InitTypeDef DMA_InitStructure;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    // 设置DMA源数据地址
    DMA_InitStructure.DMA_PeripheralBaseAddr = USART_DR_ADDRESS;
    // 设置内存地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)SendBuff;
    // 设置方向
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    // 设置传输长度
    DMA_InitStructure.DMA_BufferSize = SendBuff_Size;
    //外设地址不增，内存地址递增
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    // 设置外设数据单位和内存数据单位
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    // 一次模式
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
    // 禁止内存到内存的传输
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(USART_TX_DMA_CHANNEL, &DMA_InitStructure);
    DMA_Cmd(USART_TX_DMA_CHANNEL, ENABLE);
}
