#include "dma.h"

void DMA_Config(void)
{
    // 1.定义一个DMA初始化结构体
    DMA_InitTypeDef DMA_InitStructure;
    // 2.使能DMA时钟
    RCC_AHBPeriphClockCmd(DMA_CLK, ENABLE);
    // 3.选择源数据地址和目标数据地址（我们这个工程源是FLASH，目标是SRAM）
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)Send_DMA_BUFFER;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)Rece_DMA_BUFFER;
    // 4.设置传输方向和大小（我们工程使用的外设道存储器，这里的外设指内部的FLASH）
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = DMA_BUFFER_SIZE;
    // 5.设置外设地址递增和内存地址递增
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    // 6.设置外设和内存数据单位
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word; // 外设数据单位为字
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
    // 7.设置模式（一次或者循环模式）
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    // 8.设置优先级
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    // 9.初始化
    DMA_InitStructure.DMA_M2M = DMA_M2M_Enable;
    DMA_Init(DMA_CHx, &DMA_InitStructure);
    DMA_ClearFlag(DMA_FLAG_TC);
    DMA_Cmd(DMA_CHx, ENABLE);
}
