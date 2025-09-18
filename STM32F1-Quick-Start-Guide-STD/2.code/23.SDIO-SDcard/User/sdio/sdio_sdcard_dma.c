#include "sdio_sdcard_dma.h"
#include "sdio_sdcard.h" // 确保包含此头文件
#include "stm32f10x_dma.h"
#include "stm32f10x_sdio.h"
#include "delay.h"

// DMA传输状态
static volatile DMA_TransferStatus dma_status = DMA_TRANSFER_OK;
static volatile uint8_t transfer_complete = 0;

// 初始化SDIO DMA
void SD_DMA_Init(void) {
    DMA_InitTypeDef DMA_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    // 使能DMA2时钟
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
    
    // 配置DMA通道4 (SDIO RX)
    DMA_DeInit(DMA2_Channel4);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&SDIO->FIFO;
    DMA_InitStructure.DMA_MemoryBaseAddr = 0; // 在传输时设置
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = 0; // 在传输时设置
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA2_Channel4, &DMA_InitStructure);
    
    // 配置DMA通道4中断
    NVIC_InitStructure.NVIC_IRQChannel = DMA2_Channel4_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    DMA_ITConfig(DMA2_Channel4, DMA_IT_TC, ENABLE);
    
    // 配置DMA通道5 (SDIO TX)
    DMA_DeInit(DMA2_Channel5);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&SDIO->FIFO;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_Init(DMA2_Channel5, &DMA_InitStructure);
    
    // 配置DMA通道5中断
    NVIC_InitStructure.NVIC_IRQChannel = DMA2_Channel4_5_IRQn;
    NVIC_Init(&NVIC_InitStructure);
    DMA_ITConfig(DMA2_Channel5, DMA_IT_TC, ENABLE);
}

// 使用DMA读取块
SDStatus SD_DMA_ReadBlocks(uint8_t *buffer, uint32_t block, uint16_t count) {
    SDIO_DataInitTypeDef SDIO_DataInitStructure;
    uint32_t timeout;
    
    // 发送读命令
    SDStatus status = SD_SendCommand(count > 1 ? SD_CMD18_READ_MULTIPLE_BLOCK : SD_CMD17_READ_SINGLE_BLOCK, 
                                   block, SDIO_Response_Short);
    if (status != SD_OK) return status;
    
    // 配置数据路径
    SDIO_DataInitStructure.SDIO_DataTimeOut = 0xFFFFFF;
    SDIO_DataInitStructure.SDIO_DataLength = count * 512;
    SDIO_DataInitStructure.SDIO_DataBlockSize = SDIO_DataBlockSize_512b;
    SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToSDIO;
    SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
    SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
    SDIO_DataConfig(&SDIO_DataInitStructure);
    
    // 配置DMA
    DMA_Cmd(DMA2_Channel4, DISABLE);
    DMA2_Channel4->CMAR = (uint32_t)buffer;
    DMA2_Channel4->CNDTR = (count * 512) / 4; // 以字为单位
    DMA_Cmd(DMA2_Channel4, ENABLE);
    
    // 使能DMA传输
    SDIO_DMACmd(ENABLE);
    
    // 等待传输完成
    transfer_complete = 0;
    timeout = 0xFFFFFF;
    while (!transfer_complete && timeout--) {
        if (SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_RXOVERR)) {
            SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_RXOVERR);
            return SD_ERROR_DATA_CRC;
        }
    }
    
    if (!timeout) return SD_TIMEOUT;
    if (dma_status != DMA_TRANSFER_OK) return SD_ERROR;
    
    return SD_OK;
}

// 使用DMA写入块
SDStatus SD_DMA_WriteBlocks(uint8_t *buffer, uint32_t block, uint16_t count) {
    SDIO_DataInitTypeDef SDIO_DataInitStructure;
    uint32_t timeout;
    
    // 发送写命令
    SDStatus status = SD_SendCommand(count > 1 ? SD_CMD25_WRITE_MULTIPLE_BLOCK : SD_CMD24_WRITE_SINGLE_BLOCK, 
                                   block, SDIO_Response_Short);
    if (status != SD_OK) return status;
    
    // 配置数据路径
    SDIO_DataInitStructure.SDIO_DataTimeOut = 0xFFFFFF;
    SDIO_DataInitStructure.SDIO_DataLength = count * 512;
    SDIO_DataInitStructure.SDIO_DataBlockSize = SDIO_DataBlockSize_512b;
    SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToCard;
    SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
    SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
    SDIO_DataConfig(&SDIO_DataInitStructure);
    
    // 配置DMA
    DMA_Cmd(DMA2_Channel5, DISABLE);
    DMA2_Channel5->CMAR = (uint32_t)buffer;
    DMA2_Channel5->CNDTR = (count * 512) / 4; // 以字为单位
    DMA_Cmd(DMA2_Channel5, ENABLE);
    
    // 使能DMA传输
    SDIO_DMACmd(ENABLE);
    
    // 等待传输完成
    transfer_complete = 0;
    timeout = 0xFFFFFF;
    while (!transfer_complete && timeout--) {
        if (SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_TXUNDERR)) {
            SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_TXUNDERR);
            return SD_ERROR_DATA_CRC;
        }
    }
    
    if (!timeout) return SD_TIMEOUT;
    if (dma_status != DMA_TRANSFER_OK) return SD_ERROR;
    
    return SD_OK;
}

// DMA传输完成回调
void SD_DMA_TransferCompleteCallback(void) {
    transfer_complete = 1;
    dma_status = DMA_TRANSFER_OK;
}

// DMA2通道4和5中断处理
void DMA2_Channel4_5_IRQHandler(void) {
    // 检查通道4传输完成
    if (DMA_GetITStatus(DMA2_IT_TC4)) {
        DMA_ClearITPendingBit(DMA2_IT_TC4);
        DMA_Cmd(DMA2_Channel4, DISABLE);
        SD_DMA_TransferCompleteCallback();
    }
    
    // 检查通道5传输完成
    if (DMA_GetITStatus(DMA2_IT_TC5)) {
        DMA_ClearITPendingBit(DMA2_IT_TC5);
        DMA_Cmd(DMA2_Channel5, DISABLE);
        SD_DMA_TransferCompleteCallback();
    }
}
