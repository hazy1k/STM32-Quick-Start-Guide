#include "dma.h"

void BSP_DMA_Init(void)
{
    DMA_InitTypeDef DMA_InitStructure;
    RCC_AHBPeriphClockCmd(DMA_CLK, ENABLE);
    // 选择源数据地址和目标数据地址，本工程为存储器到存储器
    // 数据源为FLASH，目标SRAM
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)Send_DMA_BUFFER;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)Rece_DMA_BUFFER;
    // 设置数据的传输方向和大小
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; // 源数据地址为存储器
    DMA_InitStructure.DMA_BufferSize = DMA_BUFFER_SIZE; 
    // 设置外设地址和内存地址递增
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    // 设置数据单位和传输模式以及优先级
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    // 初始化
    DMA_InitStructure.DMA_M2M = DMA_M2M_Enable; // 存储器到存储器
    DMA_Init(DMA_CHx, &DMA_InitStructure);
    DMA_ClearFlag(DMA_FLAG_TC);
    DMA_Cmd(DMA_CHx, ENABLE);
}
