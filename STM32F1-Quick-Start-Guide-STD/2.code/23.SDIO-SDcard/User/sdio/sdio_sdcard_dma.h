#ifndef __SDIO_SDCARD_DMA_H
#define __SDIO_SDCARD_DMA_H

#include "stm32f10x.h"
#include "sdio_sdcard.h"  // 包含SDStatus定义

// DMA传输状态
typedef enum {
    DMA_TRANSFER_OK = 0,
    DMA_TRANSFER_ERROR,
    DMA_TRANSFER_TIMEOUT
} DMA_TransferStatus;

// 初始化SDIO DMA
void SD_DMA_Init(void);

// 使用DMA读取块
SDStatus SD_DMA_ReadBlocks(uint8_t *buffer, uint32_t block, uint16_t count);

// 使用DMA写入块
SDStatus SD_DMA_WriteBlocks(uint8_t *buffer, uint32_t block, uint16_t count);

// DMA传输完成回调
void SD_DMA_TransferCompleteCallback(void);

#endif /* __SDIO_SDCARD_DMA_H */
