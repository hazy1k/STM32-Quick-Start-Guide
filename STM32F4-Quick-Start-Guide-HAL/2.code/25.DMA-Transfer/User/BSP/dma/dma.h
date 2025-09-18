#ifndef __DMA_H
#define __DMA_H

#include "sys.h"

extern DMA_HandleTypeDef dma_handle; 
void dam_uart_init(DMA_Stream_TypeDef *dma_stream_handle, uint32_t ch);

#endif /* __DMA_H */
